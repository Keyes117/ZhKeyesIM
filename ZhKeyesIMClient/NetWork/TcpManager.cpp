#include "TcpManager.h"

#include "Logger.h"
TcpManager::TcpManager(std::shared_ptr<EventLoop> eventLoop):
    m_spEventLoop(eventLoop)
{
}

bool TcpManager::connect(const std::string& ip, uint16_t port)
{
    m_spTcpClient = std::make_unique<TCPClient>(m_spEventLoop);

    if (!m_spTcpClient->init(ip, port))
    {
        LOG_ERROR("TcpManager: TcpClient³õÊ¼»¯Ê§°Ü");
        return false;
    }

    return m_spTcpClient->connect();
}
