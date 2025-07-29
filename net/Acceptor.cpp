#include "Acceptor.h"
#include "TCPConnector.h"

Acceptor::Acceptor(EventLoop* pEventLoop)
    :m_listenSocket(INVALID_SOCKET),
    m_listening(false),
    m_enableRead(false),
    m_EventLoop(pEventLoop)

{

}

Acceptor::~Acceptor()
{
    stopListen();
}

bool Acceptor::startListen(const std::string& ip, int16_t port)
{
    if (m_listening) {
        return false;
    }

    if (!createListenSocket()) {
        return false;
    }

    if (!setSocketOption()) {
        close(m_listenSocket);
        m_listenSocket = INVALID_SOCKET;
        return false;
    }

    if (!bindAddress(ip, port)) {
        close(m_listenSocket);
        m_listenSocket = INVALID_SOCKET;
        return false;
    }

    if (!startListening()) {
        close(m_listenSocket);
        m_listenSocket = INVALID_SOCKET;
        return false;
    }

    // 注册读事件到EventLoop
    m_EventLoop->registerReadEvent(m_listenSocket, this);
    m_enableRead = true;
    m_listening = true;

    return true;
}

void Acceptor::stopListen()
{
    if (!m_listening)
        return;

    m_listening = false;
    m_enableRead = false;

    if (m_EventLoop && m_listenSocket != INVALID_SOCKET)
    {
        m_EventLoop->unregisterReadEvent(m_listenSocket, this);
    }

    if (m_listenSocket != INVALID_SOCKET)
    {
        close(m_listenSocket);
        m_listenSocket = INVALID_SOCKET;
    }
}

void Acceptor::onRead()
{
    if (!m_listening)
        return;

#ifdef _WIN32
    sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    SOCKET clientfd = ::accept(m_listenSocket,
        reinterpret_cast<sockaddr*>(&clientAddr),
        &clientAddrLen);

    if (clientfd != INVALID_SOCKET)
    {
        //设置非阻塞
        u_long mode = 1;
        if (::ioctlsocket(clientfd, FIONBIO, &mode) == SOCKET_ERROR)
        {
            close(clientfd);
            return;
        }

        if (m_acceptCallback)
        {
            m_acceptCallback(clientfd);
        }
    }
    else
    {
        int error = GetSocketError();
        if (error != EWOULDBLOCK)
        {
            //TODO:调用日志
        }
    }
#else
    while (true)
    {
        struct sockaddr_in clientAddr;

        socklen_t clientAddrLen = sizeof(clientAddr);

        int clientfd = ::accept4(m_listenSocket, (struct sockaddr*)&clientAddr, &clientAddrLen, SOCK_NONBLOCK);
        if (clientfd > 0)
        {
            //成功接受连接
            m_acceptCallback(clientfd);

        }
        else if (clientfd == -1)
        {
            if (errno == EWOULDBLOCK)
                //没有连接了
                return;
            else
                //出错了
                return;

        }
    }



#endif // _WIN32


}

void Acceptor::onWrite()
{
}

void Acceptor::onClose()
{
    stopListen();
}

void Acceptor::enableRead(bool isEnabled)
{
}

void Acceptor::enableWrite(bool isEnabled)
{
}

bool Acceptor::createListenSocket()
{
#ifdef _WIN32
    m_listenSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_listenSocket == INVALID_SOCKET)
        return false;

    u_long mode = 1;
    if (::ioctlsocket(m_listenSocket, FIONBIO, &mode) == SOCKET_ERROR)
        return false;
#else
    m_listenSocket = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (m_listenSocket == INVALID_SOCKET) {
        return false;
    }
#endif

    return true;
}



bool Acceptor::setSocketOption()
{

    int optval = 1;

    if (::setsockopt(m_listenSocket, SOL_SOCKET, SO_REUSEADDR,
        reinterpret_cast<const char*>(&optval), sizeof(optval)) == SOCKET_ERROR)
    {
        return false;
    }

#ifndef _WIN32
    // Linux下设置端口重用
    if (::setsockopt(m_listenSocket, SOL_SOCKET, SO_REUSEPORT,
        reinterpret_cast<const char*>(&optval), sizeof(optval)) == SOCKET_ERROR) {
        return false;
    }

#endif
    return true;
}

bool Acceptor::bindAddress(const std::string& ip, int16_t port)
{
    sockaddr_in bindaddr;
    bindaddr.sin_family = AF_INET;
    bindaddr.sin_port = ::htons(port);

    if (ip.empty() || ip == "0.0.0.0")
    {
        bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
#ifdef _WIN32
        if (::inet_pton(AF_INET, ip.c_str(), &bindaddr.sin_addr) <= 0)
        {
            return false;
        }
#else
        if (inet_aton(ip.c_str(), &bindaddr.sin_addr) == 0)
        {
            return false;
        }
#endif
    }

    if (::bind(m_listenSocket, reinterpret_cast<sockaddr*>(&bindaddr), sizeof(bindaddr)) == SOCKET_ERROR)
    {
        return false;
    }

    return true;

}

bool Acceptor::startListening()
{
    if (::listen(m_listenSocket, SOMAXCONN) == SOCKET_ERROR)
        return false;

    return true;
}