#include "IMServer.h"

#include "Logger.h"

IMServer::IMServer():
    m_spTcpServer(std::make_unique<TCPServer>())
{
}

IMServer::~IMServer()
{
    m_spTcpServer->shutdown();
}

bool IMServer::init(const ZhKeyes::Util::ConfigManager& config)
{
    auto ipOpt = config.getSafe<std::string>({ "IMServer", "ip" });
    auto portOpt = config.getSafe<uint16_t>({ "IMServer", "port" });
    auto threadNumOpt = config.getSafe<int32_t>({ "IMServer", "threadNum" });
    auto IOTypeOpt = config.getSafe<int>({ "IMServer", "IOType" });

    if (!ipOpt || !portOpt || !threadNumOpt || !IOTypeOpt)
    {
        LOG_ERROR("IMServer 获取配置信息失败");
        return false;
    }

    std::string ip = *ipOpt;
    uint16_t port = *portOpt;
    int threadNum = *threadNumOpt;
    IOMultiplexType IOType = static_cast<IOMultiplexType>(*IOTypeOpt);

    if (!m_spTcpServer->init(threadNum, ip, port, IOType))
    {
        LOG_ERROR("IMServer 初始化失败");
        return false;
    }

    m_spTcpServer->setConnectionCallback(std::bind(&IMServer::onConnected, this, std::placeholders::_1));
    m_spTcpServer->setDisConnectionCallback(std::bind(&IMServer::onDisConnected, this, std::placeholders::_1));
    m_spTcpServer->start();

    
    // ================== Repository ==================


    // ================== Service ==================
    m_spUserService = std::make_shared<IMUserService>();

    // ================== Controller ==================
    m_spUserController = std::make_shared<IMUserController>(m_spUserService);

    return true;
}

bool IMServer::handleMsg(std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg,
    std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender)
{
    return m_dispatcher.dispatch(msg, sender);
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
            m_pendingToDeleteSessions.emplace_back(spIMSession);
            m_sessions.erase(iter);
        }

        m_socketToSession.erase(socketIter);
    }
}

void IMServer::registerHandler()
{
    m_dispatcher.registerHandler(ZhKeyesIM::Protocol::MessageType::AUTH_REQ,
        std::bind(&IMUserController::auth, m_spUserController, std::placeholders::_1, std::placeholders::_2));
}


