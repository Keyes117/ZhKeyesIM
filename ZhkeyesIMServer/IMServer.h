#ifndef ZHKEYESIMSERVER_IMSERVER_H_
#define ZHKEYESIMSERVER_IMSERVER_H_

#include "TCPServer.h"

#include "IMProtocol/IMMessageDispatcher.h"
#include "ConfigManager.h"
#include "IMSession.h"

#include "IMProtocol/IMMessage.h"

class IMServer
{
public:
    IMServer();
    ~IMServer();

    bool init(const ZhKeyes::Util::ConfigManager& configManager);

    bool handleMsg(std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg,
            std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender
        );

private:
    void onConnected(std::shared_ptr<TCPConnection> spConn);
    void onDisConnected(SOCKET socket);

private:
    ZhKeyesIM::Protocol::IMMessageDispatcher m_dispathcer;
    
    std::unique_ptr<TCPServer> m_spTcpServer;

    std::unordered_map<uint32_t, std::weak_ptr<IMSession>>     m_userIdToSession;
    std::unordered_map<IMSession::SessionID, std::shared_ptr<IMSession>>    m_sessions;
    std::unordered_map<SOCKET, IMSession::SessionID> m_socketToSession;
    std::vector<std::shared_ptr<IMSession>> m_pendingToDeleteSessions;
    mutable std::mutex      m_sessionMutex;

};


#endif