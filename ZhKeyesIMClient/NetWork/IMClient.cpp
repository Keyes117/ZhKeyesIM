 #include "IMClient.h"
#include <iostream>

IMClient::IMClient()
{
    m_spMainEventLoop = std::make_shared<EventLoop>();
    m_spTcpClient = std::make_unique<TCPClient>(m_spMainEventLoop);
    m_spHttpClient = std::make_unique<ZhKeyesIM::Net::Http::HttpClient>(m_spMainEventLoop);

}
IMClient::~IMClient()
{
    m_spTcpClient->disconnect();
    m_spMainEventLoop->stop();
    if (m_networkThread && m_networkThread->joinable())
        m_networkThread->join();

}

bool IMClient::init(const ConfigManager& config)
{

    // 使用 getSafe + value_or 提供默认值
    auto tcpIpOpt = config.getSafe<std::string>({"TCPServer", "Ip"});
    auto tcpPortOpt = config.getSafe<std::string>({ "TCPServer", "Port" });
    auto httpIpOpt = config.getSafe<std::string>({ "HttpServer", "Ip" });
    auto httpPortOpt = config.getSafe<std::string>({ "HttpServer", "port" });

    auto typeOpt = config.getSafe<std::string>({ "IOType", "type" });

    if (!tcpIpOpt || !tcpPortOpt || !httpIpOpt || !httpPortOpt || !typeOpt)
    {
        LOG_ERROR("IMClient: 获取IMClient 相关配置失败");
        return false;
    }

    std::string tcpIp = *tcpIpOpt;
    uint16_t tcpPort = static_cast<uint16_t>(std::stoi(*tcpPortOpt));
    std::string httpIp = *httpIpOpt;
    uint16_t tcpPort = static_cast<uint16_t>(std::stoi(*httpPortOpt));

    IOMultiplexType type = static_cast<IOMultiplexType>(std::stoi(*typeOpt));

    if (!m_spTcpClient->init(tcpIp, tcpPort))
        return false;

    if(!m_spHttpClient)

    m_spMainEventLoop->init(type);
    m_networkThread = std::make_unique<std::thread>(std::bind(&IMClient::networkThreadFunc, this));
    while (!m_eventLoopRunning.load())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    std::cout << "网络线程已启动: " << m_networkThread->get_id() << std::endl;

    return true;
}

bool IMClient::connect()
{
    return m_spTcpClient->connect();
}

void IMClient::requestVerificationCode(const std::string& email)
{
    m_spHttpClient->
}

void IMClient::networkThreadFunc()
{

    m_eventLoopRunning.store(true);
    m_spMainEventLoop->run();
    m_eventLoopRunning.store(false);
}
