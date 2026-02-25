#include "IMServer.h"

#include "Logger.h"

IMServer::IMServer():
    m_spRedisManager(std::make_shared<RedisManager>()),
    m_spMySqlManager(std::make_shared<MySqlManager>()),
    m_spTcpServer(std::make_unique<TCPServer>()),
    m_spChatGrpcClient(std::make_shared<ChatGrpcClient>()),
    m_spChatGrpcServer(std::make_unique<ChatGrpcServer>()),
    m_serverPort(0),
    m_grpcPort(0)
{
}


bool IMServer::init(const ZhKeyes::Util::ConfigManager& config)
{
    auto serverNameOpt = config.getSafe<std::string>({ "IMServer", "serverName" });
    auto ipOpt = config.getSafe<std::string>({ "IMServer", "ip" });
    auto portOpt = config.getSafe<uint16_t>({ "IMServer", "port" });
    auto threadNumOpt = config.getSafe<int32_t>({ "IMServer", "threadNum" });
    auto IOTypeOpt = config.getSafe<int>({ "IMServer", "IOType" });
    auto grpcPortOpt = config.getSafe<uint16_t>({ "IMServer", "grpcPort" });

    if (!ipOpt || !portOpt || !threadNumOpt || !IOTypeOpt || !serverNameOpt ||!grpcPortOpt)
    {
        LOG_ERROR("IMServer 获取配置信息失败");
        return false;
    }

    m_serverName = *serverNameOpt;
    m_serverIp = *ipOpt;
    m_serverPort = *portOpt;
    m_grpcPort = *grpcPortOpt;
    int threadNum = *threadNumOpt;
    IOMultiplexType IOType = static_cast<IOMultiplexType>(*IOTypeOpt);

    if (!m_spTcpServer->init(threadNum, m_serverIp, m_serverPort, IOType))
    {
        LOG_ERROR("IMServer 初始化失败");
        return false;
    }

    m_spTcpServer->setConnectionCallback(std::bind(&IMServer::onConnected, this, std::placeholders::_1));
    m_spTcpServer->setDisConnectionCallback(std::bind(&IMServer::onDisConnected, this, std::placeholders::_1));
    
    if (!m_spRedisManager->init(config))
    {
        LOG_ERROR("Redis客户端 初始化失败");
        return false;
    }

    if (!m_spMySqlManager->init(config))
    {
        LOG_ERROR("Mysql客户端 初始化失败");
        return false;
    }

    // ================== Repository ==================
    m_spUserRepo = std::make_shared<IMUserRepository>(m_spRedisManager, m_spMySqlManager);

    // ================== Service ==================
    m_spUserService = std::make_shared<IMUserService>(m_spUserRepo,m_serverName ,
        m_serverIp , m_serverPort, m_grpcPort);

    m_spUserService->setNotifyCallback(
        [this](uint32_t targetUid, const ZhKeyesIM::Protocol::IMMessage& msg) -> bool {
            return notifyUser(targetUid, msg);
        });

    // ================== Controller ==================
    m_spUserController = std::make_shared<IMUserController>(m_spUserService);
    
    if (!m_spChatGrpcServer->start(m_serverIp, m_grpcPort,
        [this](uint32_t targetUid, const std::string& messageData) -> bool {
            return onForwardMessage(targetUid, messageData);
        }))
    {
        LOG_ERROR("ChatGrpcServer 启动失败");
        return false;
    }


    registerHandler();
    return true;
}

void IMServer::start()
{
    m_spTcpServer->start();
}

bool IMServer::handleMsg(std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg,
    std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender)
{
    return m_dispatcher.dispatch(msg, sender);
}

void IMServer::setUserSession(uint32_t uid, std::weak_ptr<IMSession> session)
{
    std::lock_guard<std::mutex> lock(m_userMutex);
    m_userIdToSession[uid] = session;
}

void IMServer::removeUserSession(uint32_t uid)
{
    std::lock_guard<std::mutex> lock(m_userMutex);
    m_userIdToSession.erase(uid);
}

bool IMServer::notifyUser(uint32_t targetUid, const ZhKeyesIM::Protocol::IMMessage& msg)
{
    // 1. 尝试本地推送
    {
        std::lock_guard<std::mutex> lock(m_sessionMutex);
        auto it = m_userIdToSession.find(targetUid);
        if (it != m_userIdToSession.end())
        {
            auto spSession = it->second.lock();
            if (spSession)
            {
                spSession->sendMessage(msg);
                LOG_INFO("IMServer::notifyUser: local push to uid=%u", targetUid);
                return true;
            }
            else
            {
                // weak_ptr 失效，清理
                m_userIdToSession.erase(it);
            }
        }
    }

    // 2. 查询目标用户所在的服务器
    auto serverInfoOpt = m_spUserRepo->getUserServerMapping(static_cast<int32_t>(targetUid));
    if (!serverInfoOpt)
    {
        LOG_INFO("IMServer::notifyUser: uid=%u offline (no server mapping)", targetUid);
        return false;   // 用户离线
    }

    const ServerInfo& serverInfo = *serverInfoOpt;

    // 如果目标在本服务器但 session 不存在，说明已断线
    if (serverInfo.serverName == m_serverName)
    {
        LOG_INFO("IMServer::notifyUser: uid=%u on this server but session expired", targetUid);
        return false;
    }

    // 3. 跨服务器转发
    if (serverInfo.grpcPort <= 0)
    {
        LOG_WARN("IMServer::notifyUser: uid=%u server %s has no grpcPort",
            targetUid, serverInfo.serverName.c_str());
        return false;
    }

    std::string serializedMsg = msg.serialize();
    bool forwarded = m_spChatGrpcClient->forwardMessage(
        serverInfo.serverIp,
        serverInfo.grpcPort,
        targetUid,
        serializedMsg
    );

    if (forwarded)
    {
        LOG_INFO("IMServer::notifyUser: forwarded to %s(%s:%d) for uid=%u",
            serverInfo.serverName.c_str(), serverInfo.serverIp.c_str(),
            serverInfo.grpcPort, targetUid);
    }
    else
    {
        LOG_WARN("IMServer::notifyUser: forward failed for uid=%u", targetUid);
    }

    return forwarded;
}

void IMServer::onConnected(std::shared_ptr<TCPConnection> spConn)
{
    auto spSession = std::make_shared<IMSession>(this, spConn);
    
    {
        std::lock_guard<std::mutex> lock(m_sessionMutex);
        m_socketToSession.insert(std::make_pair<SOCKET, IMSession::SessionID>(spConn->getSocket(), spSession->getSessionId()));
        m_sessions.insert(std::make_pair<IMSession::SessionID, std::shared_ptr<IMSession>>(spSession->getSessionId(), std::move(spSession)));
    }

}

void IMServer::onDisConnected(SOCKET socket)
{
    std::lock_guard<std::mutex> lock(m_sessionMutex);
    auto socketIter = m_socketToSession.find(socket);

    if (socketIter != m_socketToSession.end())
    {
        IMSession::SessionID sessionID = socketIter->second;

        auto iter = m_sessions.find(sessionID);
        if (iter != m_sessions.end())
        {
            std::shared_ptr<IMSession> spIMSession = iter->second;

            uint32_t uid = spIMSession->getUid();
            if (uid != 0)
            {
                m_userIdToSession.erase(uid);
                LOG_INFO("IMServer::onDisConnected: removed uid=%u session mapping", uid);

               
                std::string sessionIdStr = std::to_string(spIMSession->getSessionId());
                m_spUserRepo->removeUserSessionMapping(static_cast<int32_t>(uid), sessionIdStr);
                m_spUserRepo->decrementServerConnectionCount(m_serverName);
            }

            m_pendingToDeleteSessions.emplace_back(spIMSession);
            m_sessions.erase(iter);
        }

        m_socketToSession.erase(socketIter);
    }
}

bool IMServer::onForwardMessage(uint32_t targetUid, const std::string& messageData)
{
    // 反序列化消息
    ZhKeyesIM::Protocol::IMMessage msg;
    if (!ZhKeyesIM::Protocol::IMMessage::deserialize(messageData, msg))
    {
        LOG_ERROR("IMServer::onForwardMessage: failed to deserialize message for uid=%u", targetUid);
        return false;
    }

    // 查找本地 session 并推送
    std::lock_guard<std::mutex> lock(m_sessionMutex);
    auto it = m_userIdToSession.find(targetUid);
    if (it != m_userIdToSession.end())
    {
        auto spSession = it->second.lock();
        if (spSession)
        {
            spSession->sendMessage(msg);
            LOG_INFO("IMServer::onForwardMessage: delivered to uid=%u", targetUid);
            return true;
        }
    }

    LOG_WARN("IMServer::onForwardMessage: uid=%u not found on this server", targetUid);
    return false;
}

void IMServer::registerHandler()
{
    m_dispatcher.registerHandler(ZhKeyesIM::Protocol::MessageType::AUTH_REQ,
        std::bind(&IMUserController::auth, m_spUserController,
            std::placeholders::_1, std::placeholders::_2));

    m_dispatcher.registerHandler(ZhKeyesIM::Protocol::MessageType::SEARCH_USER_REQ,
        std::bind(&IMUserController::searchUser, m_spUserController,
            std::placeholders::_1, std::placeholders::_2));

    m_dispatcher.registerHandler(ZhKeyesIM::Protocol::MessageType::APPLY_USER_REQ,
        std::bind(&IMUserController::applyFriend, m_spUserController,
            std::placeholders::_1, std::placeholders::_2));

    m_dispatcher.registerHandler(
        ZhKeyesIM::Protocol::MessageType::FETCH_FRIEND_APPLY_LIST_REQ,
        std::bind(&IMUserController::getFriendApplyList, m_spUserController,
            std::placeholders::_1, std::placeholders::_2));

    
}


