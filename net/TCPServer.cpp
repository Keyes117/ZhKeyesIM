#include "TCPServer.h"

#ifdef _WIN32
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif
TCPServer::TCPServer()
    :m_acceptor(&m_baseEventLoop)
{

}

TCPServer::~TCPServer()
{
    shutdown();
}

bool TCPServer::init(int32_t threadNum, const std::string& ip, uint16_t port)
{
    LOG_INFO("server initializing...");
#ifdef _WIN32
    WSADATA wsaData;
    int wsaRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaRet != 0)
    {
        std::cout << "WSAStartup failed: %d" << wsaRet << std::endl;
        return false;
    }
#endif
    m_threadPool.start(threadNum);
    m_ip = ip;
    m_port = port;

    m_baseEventLoop.init(IOMultiplexType::Select);
    m_acceptor.setAcceptCallback(std::bind(&TCPServer::onAccept, this, std::placeholders::_1));

    if (!m_acceptor.startListen(m_ip, m_port))
    {
        LOG_ERROR("server is listening Ip:%s,port%" PRIu16 "!");
        return false;
    }
    return true;

    LOG_INFO("server initialized");
}



void TCPServer::start()
{
    std::cout << "base EventLoop ThreadId" << std::this_thread::get_id() << std::endl;
    m_baseEventLoop.setThreadID(std::this_thread::get_id());

    m_baseEventLoop.run();
    LOG_INFO("set server Ip:%s, port:%" PRId16 "...", m_ip, m_port);
}

void TCPServer::shutdown()
{
#ifdef _WIN32
    WSACleanup();
#endif
    m_threadPool.stop();
    m_acceptor.stopListen();
    LOG_INFO("server shutdown");

}

void TCPServer::onAccept(SOCKET clientSocket)
{
    LOG_INFO("accept client connection,client socket %d", static_cast<int>(clientSocket));
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


