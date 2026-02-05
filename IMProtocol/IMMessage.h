#ifndef IMPROTOCOL_IMMESSAGE_H_
#define IMPROTOCOL_IMMESSAGE_H_

#include <climits>
#include <memory>
#include "IMProtocol/IMProtocol.h"

namespace ZhKeyesIM{
namespace Protocol {

class IMMessage
{
public:
    /**
     * @brief ??????
     * @param type IMProtocol.h ?????
     * @param seqId ???
     * @param body //???????????BInaryWrite ?????????
     */
    IMMessage(MessageType type = MessageType::UNKNOWN, uint32_t seqId = INT_MAX, const std::string& body = " ");


    // Getter/Setter
    MessageType getType() const { return static_cast<MessageType>(m_header.type); }
    void setType(MessageType type) { m_header.type = static_cast<uint16_t>(type); }

    uint32_t getSeqId() const { return m_header.seqId; }
    void setSeqId(uint32_t seqId) { m_header.seqId = seqId; }

    uint8_t getVersion() const { return m_header.version; }
    void setVersion(uint8_t version) { m_header.version = version; }

    uint8_t getFlags() const { return m_header.flags; }
    void setFlags(uint8_t flags) { m_header.flags = flags; }

    bool isCompressed() const {
        return (m_header.flags & Flags::COMPRESSED) != 0;
    }

    bool isEncrypted() const {
        return (m_header.flags & Flags::ENCRYPTED) != 0;
    }

    void setCompressed(bool compressed) {
        if (compressed) {
            m_header.flags |= Flags::COMPRESSED;
        }
        else {
            m_header.flags &= ~Flags::COMPRESSED;
        }
    }

    void setEncrypted(bool encrypted) {
        if (encrypted) {
            m_header.flags |= Flags::ENCRYPTED;
        }
        else {
            m_header.flags &= ~Flags::ENCRYPTED;
        }
    }


    uint32_t getLength() const { return m_header.length; }

    const std::string& getBody() const { return m_body; }
    void setBody(const std::string& body)
    {
        m_body = body;
        m_header.length = HEADER_SIZE + m_body.size();
    }

    size_t getBodySize() const { return m_body.size(); }
    bool hasBody() const { return !m_body.empty(); }

    const MessageHeader& getHeader() const { return m_header; }
    void setHeader(const MessageHeader& header) {
        m_header = header;
        // 更新长度字段以匹配 body
        m_header.length = HEADER_SIZE + m_body.size();
    }

    bool isValid() const {
        return m_header.magic == PROTOCOL_MAGIC &&
            m_header.length >= HEADER_SIZE &&
            m_header.length <= MAX_PACKET_SIZE &&
            m_header.length == HEADER_SIZE + m_body.size();
    }

    /**
     * @brief 清空消息内容
     */
    void clear() {
        m_header = MessageHeader{};
        m_body.clear();
    }

    /**
     * @brief 获取完整消息大小（头部 + 消息体）
     */
    size_t getTotalSize() const {
        return HEADER_SIZE + m_body.size();
    }

    std::string serialize() const;
    static bool deserialize(const std::string& data, IMMessage& out);
    static std::shared_ptr<IMMessage> deserializeFromBuffer(const char* data, size_t len);


private:
    MessageHeader m_header;
    std::string m_body;
};
}}

#endif