#ifndef ZHKEYESIMSERVER_IMSERVER_H_
#define ZHKEYESIMSERVER_IMSERVER_H_

#include "TCPServer.h"

#include "ConfigManager.h"
#include "IMSession.h"

class IMServer
{
public:
    IMServer();
    ~IMServer();

    bool init(const ZhKeyes::Util::ConfigManager& configManager);


private:
    void onConnected(std::shared_ptr<TCPConnection> spConn);
    void onDisConnected(IMSession::SessionID sessionId);



private:
    std::unique_ptr<TCPServer> m_spTcpServer;

    std::unordered_map<uint32_t, std::weak_ptr<IMSession>>     m_userIdToSession;
    std::unordered_map<IMSession::SessionID, std::shared_ptr<IMSession>>    m_sessions;
    std::unordered_map<SOCKET, IMSession::SessionID> m_socketToSession;
    std::vector<std::shared_ptr<IMSession>> m_pendingToDeleteSessions;
    mutable std::mutex      m_sessionMutex;

};


#endif