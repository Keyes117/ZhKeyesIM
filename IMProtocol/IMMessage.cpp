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
    // 确保长度字段是最新的
    MessageHeader header = m_header;
    header.length = HEADER_SIZE + m_body.size();

    BinaryWriter writer(header.length);

    // 按照协议格式写入头部
    writer.writeUInt32(header.magic);          // Magic: 4 bytes
    writer.writeUInt32(header.length);         // Length: 4 bytes (包括头部)
    writer.writeUInt8(header.version);         // Version: 1 byte
    writer.writeUInt16(header.type);           // Type: 2 bytes
    writer.writeUInt32(header.seqId);          // SeqId: 4 bytes
    writer.writeUInt8(header.flags);           // Flags: 1 byte
    writer.writeUInt16(header.reserve);        // Reserve: 2 bytes
    // 总计：4+4+1+2+4+1+2 = 18 字节

    // 写入 body
    if (!m_body.empty()) {
        writer.writeBytes(m_body.data(), m_body.size());
    }

    return writer.getData();
}

bool ZhKeyesIM::Protocol::IMMessage::deserialize(const std::string& data, IMMessage& out)
{
    if (data.size() < HEADER_SIZE) {
        return false;  // 数据不足以包含头部
    }

    BinaryReader reader(data);

    MessageHeader header{};

    // 按照新协议格式读取头部（18字节）
    if (!reader.readUInt32(header.magic))   return false;
    if (!reader.readUInt32(header.length))  return false;
    if (!reader.readUInt8(header.version))  return false;
    if (!reader.readUInt16(header.type))    return false;
    if (!reader.readUInt32(header.seqId))  return false;
    if (!reader.readUInt8(header.flags))    return false;
    if (!reader.readUInt16(header.reserve)) return false;

    // magic校验
    if (header.magic != PROTOCOL_MAGIC) {
        return false;
    }

    // 长度校验
    if (header.length < HEADER_SIZE || header.length > MAX_PACKET_SIZE) {
        return false;
    }

    // 检查数据是否完整（必须刚好是一个完整的消息）
    if (data.size() != header.length) {
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

    // 设置输出
    out.m_header = header;
    out.m_body = std::move(body);
    return true;
}

std::shared_ptr<ZhKeyesIM::Protocol::IMMessage>
        ZhKeyesIM::Protocol::IMMessage::deserializeFromBuffer(const char* data, size_t len)
{
    if (!data || len < HEADER_SIZE) {
        return nullptr;  // 数据不足以包含头部
    }

    BinaryReader reader(data, len);

    MessageHeader header{};

    // 按照新协议格式读取头部（18字节）
    if (!reader.readUInt32(header.magic))   return nullptr;
    if (!reader.readUInt32(header.length))  return nullptr;
    if (!reader.readUInt8(header.version))  return nullptr;
    if (!reader.readUInt16(header.type))    return nullptr;
    if (!reader.readUInt32(header.seqId))   return nullptr;
    if (!reader.readUInt8(header.flags))    return nullptr;
    if (!reader.readUInt16(header.reserve)) return nullptr;

    // 魔数校验
    if (header.magic != PROTOCOL_MAGIC) {
        return nullptr;
    }

    // 长度校验
    if (header.length < HEADER_SIZE || header.length > MAX_PACKET_SIZE) {
        return nullptr;
    }

    // 检查缓冲区是否包含完整的消息
    if (len < header.length) {
        // 缓冲区还没有一个完整的消息，返回 nullptr 表示需要等待更多数据
        return nullptr;
    }

    // 读取 Body
    size_t bodyLen = header.length - HEADER_SIZE;
    std::string body;
    if (bodyLen > 0) {
        body.resize(bodyLen);
        if (!reader.readBytes(&body[0], bodyLen)) {
            return nullptr;
        }
    }

    // 创建消息对象
    auto msg = std::make_shared<IMMessage>();
    msg->m_header = header;
    msg->m_body = std::move(body);
    return msg;
}
