#include "IMSession.h"

IMSession::IMSession(IMServer* server, std::shared_ptr<TCPConnection> spConn)
    :m_pServer(server),
    m_spConn(spConn)
{
    m_sessionId = generateID();
}

uint32_t IMSession::generateID()
{
    static std::atomic<uint32_t> s_sessionID = 0;
    uint32_t result = ++s_sessionID;
    return result;
}
