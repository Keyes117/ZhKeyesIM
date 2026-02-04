#ifndef ZHKEYESIMSERVER_IMSERVER_H_
#define ZHKEYESIMSERVER_IMSERVER_H_

#include "net/TCPServer.h"
#include "IMProtocol/IMMessage.h"
#include "IMProtocol/IMMessageDispatcher.h"

#include "infrastructure/MySqlManager.h"
#include "infrastructure/RedisManager.h"

#include "IMSession.h"
#include "util/ConfigManager.h"

#include "Controller/IMUserController.h"

#include "Service/IMUserService.h"


class IMServer
{
public:
    IMServer();
    ~IMServer() = default;

    bool init(const ZhKeyes::Util::ConfigManager& configManager);

    void start();

    bool handleMsg(std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg,
            std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender
        );

    void setUserSession(uint32_t uid, std::weak_ptr<IMSession>);

private:
    void onConnected(std::shared_ptr<TCPConnection> spConn);
    void onDisConnected(SOCKET socket);

    void registerHandler();

private:
    ZhKeyesIM::Protocol::IMMessageDispatcher m_dispatcher;
    
    // =============== infrastructure ===================
    std::shared_ptr<MySqlManager>   m_spMySqlManager;
    std::shared_ptr<RedisManager>   m_spRedisManager;

    // =============== repository ===================


    // =============== Service ===================
    std::shared_ptr<IMUserService> m_spUserService;

    // =============== Controller ===================
    std::shared_ptr<IMUserController> m_spUserController;     

    std::unique_ptr<TCPServer> m_spTcpServer;
    std::unordered_map<uint32_t, std::weak_ptr<IMSession>>     m_userIdToSession;
    std::unordered_map<IMSession::SessionID, std::shared_ptr<IMSession>>    m_sessions;
    std::unordered_map<SOCKET, IMSession::SessionID> m_socketToSession;
    std::vector<std::shared_ptr<IMSession>> m_pendingToDeleteSessions;
    mutable std::mutex      m_sessionMutex;

};


#endif