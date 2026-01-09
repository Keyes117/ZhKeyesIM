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

bool IMSession::sendMessage(std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg)
{

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

        ZhKeyesIM::Protocol::IMMessage msg;
        if (!ZhKeyesIM::Protocol::IMMessage::deserializeFromBuffer(data, len, msg))
            break;

        m_pServer->handleMsg(msg);

        size_t msgLen = msg.getLength();
        buf.retrieve(msgLen);

    }
}

void IMSession::onWrite()
{
}
