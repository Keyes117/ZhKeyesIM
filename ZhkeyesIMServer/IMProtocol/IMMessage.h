#ifndef ZHKEYESIMCLIENT_IMPROTOCOL_IMMESSAGE_H_
#define ZHKEYESIMCLIENT_IMPROTOCOL_IMMESSAGE_H_

#include "IMProtocol/IMProtocol.h"

namespace ZhKeyesIM{
namespace Protocol {

class IMMessage
{
public:
    IMMessage(MessageType type, uint32_t seqId, const std::string& body)
        : m_header()
        , m_body(body)
    {
        m_header.type = static_cast<uint16_t>(type);
        m_header.seqId = seqId;
        m_header.length = HEADER_SIZE + body.size();
    }

    // Getter/Setter
    MessageType getType() const { return static_cast<MessageType>(m_header.type); }
    void setType(MessageType type) { m_header.type = static_cast<uint16_t>(type); }

    uint32_t getSeqId() const { return m_header.seqId; }
    void setSeqId(uint32_t seqId) { m_header.seqId = seqId; }

    uint32_t getLength() const { return m_header.length; }

    const std::string& getBody() const { return m_body; }
    void setBody(const std::string& body)
    {
        m_body = body;
        m_header.length = HEADER_SIZE + m_body.size();
    }

    const MessageHeader& getHeader() const { return m_header; }

private:
    MessageHeader m_header;
    std::string m_body;
};
}}

#endif