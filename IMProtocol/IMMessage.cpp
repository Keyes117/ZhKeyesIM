#include "IMMessage.h"

#include "IMProtocol/BinaryReader.h"
#include "IMProtocol/BinaryWriter.h"

ZhKeyesIM::Protocol::IMMessage::IMMessage(MessageType type, uint32_t seqId, const std::string& body) 
    : m_header(), 
        m_body(body)
{
    m_header.type = static_cast<uint16_t>(type);
    m_header.seqId = seqId;
    m_header.length = HEADER_SIZE + body.size();
}

std::string ZhKeyesIM::Protocol::IMMessage::serialize() const
{
    BinaryWriter writer(HEADER_SIZE + m_body.size());

    // 头部固定 16 字节，使用固定长度编码（BinaryWriter 内部已做字节序转换）
    writer.writeUInt32(m_header.magic);          // Magic
    writer.writeUInt32(m_header.length);         // Length（含头部）
    writer.writeUInt16(m_header.type);           // Type
    writer.writeUInt16(m_header.seqId);          // SeqId
    writer.writeUInt16(m_header.reserve);        // Reserve
    writer.writeUInt16(0);
    // 注意：此时已经写了 4+4+2+2+2 = 14 字节
    // 但 HEADER_SIZE 定义为 16，所以还有 2 字节空位。
    // 目前 MessageHeader 结构里只有 14 字节字段（magic+length+type+seqId+reserve，
    // 这里按顺序再写一个 uint16 即可。


    // 写入 body（原样字节，不做额外长度前缀）
    if (!m_body.empty()) {
        writer.writeBytes(m_body.data(), m_body.size());
    }

    return writer.getData();
}

bool ZhKeyesIM::Protocol::IMMessage::deserialize(const std::string& data, IMMessage& out)
{
    if (data.size() < HEADER_SIZE) {
        return false;
    }

    BinaryReader reader(data);

    MessageHeader header{};
    uint16_t empty;
    if (!reader.readUInt32(header.magic))   return false;
    if (!reader.readUInt32(header.length))  return false;
    if (!reader.readUInt16(header.type))    return false;
    if (!reader.readUInt16(header.seqId))   return false;
    if (!reader.readUInt16(header.reserve)) return false;
    if (!reader.readUInt16(empty)) return false;

    // 魔数校验
    if (header.magic != PROTOCOL_MAGIC) {
        return false;
    }

    // 长度校验
    if (header.length < HEADER_SIZE || header.length > MAX_PACKET_SIZE) {
        return false;
    }
    if (data.size() != header.length) {
        // 这里要求 data 刚好是一条完整消息
        return false;
    }

    // 读取 Body
    std::string body;
    size_t bodyLen = header.length - HEADER_SIZE;
    if (bodyLen > 0) {
        body.resize(bodyLen);
        if (!reader.readBytes(&body[0], bodyLen)) {
            return false;
        }
    }

    // 构造输出对象
    out.m_header = header;
    out.m_body = std::move(body);
    return true;
}

std::shared_ptr<ZhKeyesIM::Protocol::IMMessage>
        ZhKeyesIM::Protocol::IMMessage::deserializeFromBuffer(const char* data, size_t len)
{
    if (!data || len < HEADER_SIZE) {
        return nullptr; // 数据不足以解析头部
    }

    BinaryReader reader(data, len);

    MessageHeader header{};

    if (!reader.readUInt32(header.magic))   return nullptr;
    if (!reader.readUInt32(header.length))  return nullptr;
    if (!reader.readUInt16(header.type))    return nullptr;
    if (!reader.readUInt16(header.seqId))   return nullptr;
    if (!reader.readUInt16(header.reserve)) return nullptr;

    if (header.magic != PROTOCOL_MAGIC) {
        return nullptr;
    }

    if (header.length < HEADER_SIZE || header.length > MAX_PACKET_SIZE) {
        return nullptr;
    }

    if (len < header.length) {
        // 缓冲区里还没有一整条消息，调用方应继续等待更多数据
        return nullptr;
    }

    size_t bodyLen = header.length - HEADER_SIZE;
    std::string body;
    if (bodyLen > 0) {
        body.resize(bodyLen);
        if (!reader.readBytes(&body[0], bodyLen)) {
            return nullptr;
        }
    }

    auto msg = std::make_shared<IMMessage>();
    msg->m_header = header;
    msg->m_body = std::move(body);
    return msg;
}
