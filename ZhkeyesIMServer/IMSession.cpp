#include "IMSession.h"

#include "IMProtocol/IMProtocol.h"
#include "IMProtocol/IMMessage.h"
#include "IMProtocol/BinaryWriter.h"
#include "IMProtocol/BinaryReader.h"

#include "IMServer.h"
#include "Logger.h"

IMSession::IMSession(IMServer* server, std::shared_ptr<TCPConnection> spConn)
    :m_pServer(server),
    m_spConn(spConn)
{
    m_sessionId = generateID();

    spConn->setReadCallback(std::bind(&IMSession::onRead,this,std::placeholders::_1));
    spConn->setWriteCallback(std::bind(&IMSession::onWrite, this));

}

bool IMSession::sendMessage(const ZhKeyesIM::Protocol::IMMessage& msg)
{
    std::string msgData = msg.serialize();
    return m_spConn->send(msgData);
}

void IMSession::setUserInfo(uint32_t uid, const std::string& token)
{
    m_uid = uid;
    m_token = token;
}

uint32_t IMSession::generateID()
{
    static std::atomic<uint32_t> s_sessionID = 0;
    uint32_t result = ++s_sessionID;
    return result;
}

void IMSession::onRead(Buffer& buf)
{
    while (true)
    {
        if (buf.readableBytes() < ZhKeyesIM::Protocol::HEADER_SIZE)
            break;

        const char* data = buf.peek();
        size_t      len = buf.readableBytes();

        auto msg = ZhKeyesIM::Protocol::IMMessage::deserializeFromBuffer(data, len);
        if (!msg)
            break;

        auto self = shared_from_this();
        m_pServer->handleMsg(msg,self);

        size_t msgLen = msg->getLength();
        buf.retrieve(msgLen);

    }
}

void IMSession::onWrite()
{
}
