#include "TCPServer.h"



TCPServer::TCPServer()
    :m_acceptor(&m_baseEventLoop)
{
}

bool TCPServer::init(int32_t threadNum, const std::string& ip, uint16_t port)
{
    LOG_INFO("初始化服务器...");
    m_threadPool.start(threadNum);
    m_ip = ip;
    m_port = port;

    m_baseEventLoop.init(IOMultiplexType::Select);
    m_acceptor.setAcceptCallback(std::bind(&TCPServer::onAccept, this, std::placeholders::_1));

    if (!m_acceptor.startListen(m_ip, m_port))
    {
        LOG_ERROR("设置监听Ip:%s,port%" PRIu16 " 失败!");
        return false;
    }
    return true;

    LOG_INFO("初始化成功！");
}



void TCPServer::start()
{
    std::cout << "base EventLoop ThreadId" << std::this_thread::get_id() << std::endl;
    m_baseEventLoop.setThreadID(std::this_thread::get_id());

    m_baseEventLoop.run();
    LOG_INFO("服务开始运行，监听Ip:%s, port:%" PRId16 "...", m_ip, m_port);
}

void TCPServer::shutdown()
{

    m_threadPool.stop();
    m_acceptor.stopListen();
    LOG_INFO("服务端已关闭");
}

void TCPServer::onAccept(SOCKET clientSocket)
{
    LOG_INFO("收到连接,客户端Socket %d", static_cast<int>(clientSocket));
    auto spEventLoop = m_threadPool.getNextEventLoop();
    auto spConnection = std::make_shared<TCPConnection>(clientSocket, spEventLoop);


    m_connections[clientSocket] = spConnection;
    spConnection->setCloseCallBack(std::bind(&TCPServer::onDisConnected, this, clientSocket));
    spConnection->startRead();
    if (m_connectionCallback)
    {
        m_connectionCallback(spConnection);
    }

}

void TCPServer::onConnected(std::shared_ptr<TCPConnection>& spConn)
{
}

void TCPServer::onDisConnected(SOCKET clientSocket)
{
    m_connections.erase(clientSocket);
}


