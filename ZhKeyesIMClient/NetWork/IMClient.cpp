 #include "IMClient.h"
#include <iostream>

IMClient::IMClient()
{
    m_spMainEventLoop = std::make_shared<EventLoop>();
    m_spClient = std::make_unique<TCPClient>(m_spMainEventLoop);

}
IMClient::~IMClient()
{
    m_spClient->disconnect();
    m_spMainEventLoop->stop();
    if (m_networkThread && m_networkThread->joinable())
        m_networkThread->join();

}

void IMClient::init(const std::string& serverIp, uint16_t nPort)
{
    m_spClient->init(serverIp, nPort);
    m_spMainEventLoop->init(IOMultiplexType::Epoll);
    m_networkThread = std::make_unique<std::thread>(std::bind(&IMClient::networkThreadFunc, this));
    while (!m_eventLoopRunning.load())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    std::cout << "网络线程已启动: " << m_networkThread->get_id() << std::endl;
}

bool IMClient::connect()
{
    return m_spClient->connect();
}

void IMClient::networkThreadFunc()
{

    m_eventLoopRunning.store(true);
    m_spMainEventLoop->run();
    m_eventLoopRunning.store(false);
}
