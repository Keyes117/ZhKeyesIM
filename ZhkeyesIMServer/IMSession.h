#ifndef ZHKEYESIMSERVER_IMSESSION_H_
#define ZHKEYESIMSERVER_IMSESSION_H_

#include <memory>

#include "TCPConnection.h"

class IMServer;

class IMSession
{
public:
    using SessionID = uint32_t;

    IMSession(IMServer* server, std::shared_ptr<TCPConnection> spConn);
    ~IMSession() = default;;


    SessionID getSessionId() { return m_sessionId; }

private:
    static uint32_t generateID();

private:
    IMServer* m_pServer;
    std::shared_ptr<TCPConnection> m_spConn;
    SessionID m_sessionId;
};


#endif