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
    auto threadNumOpt = config.getSafe<int>({ "IMServer", "threadNum" });
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

    m_spTcpServer->start();

    return true;
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

