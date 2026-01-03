#include "TCPClient.h"

#include "Logger.h"


TCPClient::TCPClient(std::shared_ptr<EventLoop> eventLoop)
    :m_spEventLoop(eventLoop)
{
}

TCPClient::~TCPClient()
{
    cleanup();
}

bool TCPClient::init(const std::string& serverIP, uint16_t serverPort,
    uint32_t timeoutMs)
{
    LOG_INFO("TCPClient Initializing...");

    if (m_initialized.load())
    {
        LOG_INFO("TCPClient has been already initialized ");
        return false;
    }


    if (serverIP.empty() || serverPort == 0)
    {
        LOG_WARN("illegal value of serverIp or serverPort");
        return false;
    }


    m_serverIP = serverIP;
    m_serverPort = serverPort;
    m_timeoutMs = timeoutMs;

    // EventLoop应该在外部统一初始化
    // m_spEventLoop->init(IOMultiplexType::Epoll);
    m_initialized.store(true);
    LOG_INFO("TCPClient initialized compeletely");
    return true;
}


bool TCPClient::connect()
{

    if (!m_initialized.load())
        return false;

    if (m_connecting.load() || m_connected.load())
    {
        LOG_ERROR("Illegal status of TCPClient");
        return false;
    }

    LOG_INFO("TCPConnector Creating...");
    if (!createConnector())
    {
        LOG_ERROR("Failed to create TCPConnector!");
        return false;
    }

    m_connecting.store(true);

    m_spConnector->setConnectCallback(
        std::bind(&TCPClient::onConnected, this, std::placeholders::_1)
    );

    m_spConnector->setConnectFailedCallback(
        std::bind(&TCPClient::onConnectFailed, this)
    );

    LOG_INFO("Conneting Ip:%s,port: %" PRIu16, m_serverIP.c_str(), m_serverPort);
    if (!m_spConnector->startConnect(m_serverIP, m_serverPort, m_timeoutMs))
    {
        m_connecting.store(false);
        LOG_INFO("failed to connect");
        return false;
    }

    return true;
}

void TCPClient::cancelConnect()
{
    LOG_INFO("TCPClient canceled connect...");
    if (!m_connecting.load())
        return;

    if (m_spConnector)
        m_spConnector->cancelConnect();

    m_connecting.store(false);
}

void TCPClient::disconnect()
{
    LOG_INFO("客户端断开连接...");
    if (m_spConnection)
        m_spConnection.reset();

    m_connected.store(false);
    cancelConnect();
}

bool TCPClient::send(const char* data, size_t len)
{
    if (!m_connected.load() || !m_spConnection)  // 修复逻辑错误
    {
        return false;
    }

    return m_spConnection->send(data, len);
}

bool TCPClient::send(const std::string& data)
{
    return send(data.c_str(), data.length());
}

void TCPClient::onConnected(SOCKET socket)
{
    m_connecting.store(false);
    m_connected.store(true);

    m_spConnection = std::make_shared<TCPConnection>(socket, m_spEventLoop);

    if (m_spConnection) {
        m_spConnection->setCloseCallback(
            std::bind(&TCPClient::onDisconnected, this)
        );

        m_spConnection->startRead();
    }

    m_spConnector.reset();

    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_connectionCallback && m_spConnection) {
        try {
            m_connectionCallback(m_spConnection);
        }
        catch (const std::exception& e) {

        }
    }
}

void TCPClient::onConnectFailed()
{

    m_connecting.store(false);
    m_connected.store(false);


    m_spConnector.reset();

    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_connectionFailedCallback) {
        try {
            m_connectionFailedCallback();
        }
        catch (const std::exception& e) {

        }
    }
}

void TCPClient::onDisconnected()
{
    m_connected.store(false);
    m_spConnection.reset();
}

void TCPClient::cleanup()
{
    disconnect();

    m_spConnection.reset();
    m_spConnector.reset();
}

bool TCPClient::createConnector()
{
    if (m_spConnector) {
        return true;
    }


    try {
        m_spConnector = std::make_unique<TCPConnector>(m_spEventLoop);
    }
    catch (const std::exception&) {
        return false;
    }

    return true;
}
