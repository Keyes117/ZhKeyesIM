#include "TCPConnector.h"

#ifdef _WIN32
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <error.h>

#endif
#include <iostream>

#include "TCPClient.h"
#include "Logger.h"


TCPConnector::TCPConnector(const std::shared_ptr<EventLoop>& spEventLoop)
    :m_spEventLoop(spEventLoop),
    m_socket(INVALID_SOCKET),
    m_serverPort(0),
    m_connectTimeoutMs(5000),
    m_timeOutTimerId(-1),
    m_isConnecting(false)
{
}

TCPConnector::~TCPConnector()
{
    cleanup();
}

bool TCPConnector::startConnect(const std::string& serverIP, uint16_t serverPort, uint32_t timeoutMs)
{
    if (m_isConnecting.load())
        return false;

    m_serverIP = serverIP;
    m_serverPort = serverPort;
    m_connectTimeoutMs = timeoutMs;


    if (!createSocket())
    {
        LOG_ERROR("TCPConnector create Socket error!");
        return false;
    }


    if (!setSocketOption())
    {
        LOG_ERROR("TCPConnector set SocketOption error!");
        cleanup();
        return false;
    }

    if (!connect())
    {
        cleanup();
        return false;
    }

    return true;
}

void TCPConnector::cancelConnect()
{
    if (!m_isConnecting.load())
        return;

    m_isConnecting.store(false);
    cleanup();

}

void TCPConnector::onRead()
{
}
void TCPConnector::onWrite()
{

    if (!m_isConnecting.load())
        return;

    checkConnectResult();
}
void TCPConnector::onClose()
{
    if (m_isConnecting.load())
    {
        m_isConnecting.store(false);

        if (m_connectFailedCallback)
            m_connectFailedCallback();
    }

    cleanup();
}

bool TCPConnector::createSocket()
{
#ifdef _WIN32
    m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == INVALID_SOCKET)
    {
        return false;
    }

    u_long mode = 1;
    if (::ioctlsocket(m_socket, FIONBIO, &mode) == SOCKET_ERROR)
    {
        return false;
    }
#else

    m_socket = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (m_socket == INVALID_SOCKET)
    {
        return false;
    }
#endif

    return true;
}


bool TCPConnector::setSocketOption()
{
    int optval = 1;
    int noDelay = 1;

    // ���� TCP_NODELAY
    if (::setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY,
        reinterpret_cast<const char*>(&noDelay), sizeof(noDelay)) == SOCKET_ERROR)
    {
        return false;
    }

    // ���� SO_REUSEADDR
    if (::setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR,
        reinterpret_cast<const char*>(&optval), sizeof(optval)) == SOCKET_ERROR)
    {
        return false;
    }

    return true;
}

bool TCPConnector::connect()
{
    sockaddr_in serverAddr = { 0 };
    struct hostent* pHostent = NULL;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(m_serverPort);

#ifdef _WIN32
    if (::inet_pton(AF_INET, m_serverIP.c_str(), &serverAddr.sin_addr) <= 0)
    {
        return false;
    }
#else

    if (inet_aton(m_serverIP.c_str(), &serverAddr.sin_addr) == 0)
    {
        return false;
    }
#endif

    int result = ::connect(m_socket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));

#ifdef _WIN32

    if (result == SOCKET_ERROR)
    {
        int error = GetSocketError();
        if (error == WSAEWOULDBLOCK || error == WSAEINPROGRESS)
        {

            m_isConnecting.store(true);
            m_connectStartTime = std::chrono::steady_clock::now();
            m_spEventLoop->registerWriteEvent(m_socket, this);

            m_timeOutTimerId = m_spEventLoop->addTimer(
                m_connectTimeoutMs,
                1,
                std::bind(&TCPConnector::onConnectionTimeout, this)
            );

            return true;
        }
        else
        {
            return false;
        }
    }

#else

    if (result < 0)
    {
        if (errno == EINPROGRESS)
        {
            m_isConnecting.store(true);
            m_connectStartTime = std::chrono::steady_clock::now();
            m_spEventLoop->registerWriteEvent(m_socket, this);


            return true;
        }
        else
        {
            return false;
        }
    }

#endif

    m_isConnecting.store(false);
    if (m_connectCallback)
        m_connectCallback(m_socket);

    m_socket = INVALID_SOCKET;

    return true;

}

void TCPConnector::checkConnectResult()
{
    if (!m_isConnecting.load())
        return;

    auto elapsed = std::chrono::steady_clock::now() - m_connectStartTime;
    if (elapsed > std::chrono::milliseconds(m_connectTimeoutMs))
    {
        LOG_INFO("connecting time out");
        m_isConnecting.store(false);
        if (m_connectFailedCallback)
            m_connectFailedCallback();


        cleanup();
        return;
    }

    int error = 0;
    socklen_t len = sizeof(error);
    if (::getsockopt(m_socket, SOL_SOCKET, SO_ERROR,
        reinterpret_cast<char*>(&error), &len) == SOCKET_ERROR)
    {
        m_isConnecting.store(false);

        if (m_connectFailedCallback)
            m_connectFailedCallback();

        cleanup();
        return;
    }

    if (error == 0)
    {
        LOG_INFO("connection successful");
        m_isConnecting.store(false);

        if (m_enableWrite)
        {
            m_spEventLoop->unregisterWriteEvent(m_socket, this);
        }

        if (m_connectCallback)
        {
            SOCKET connectedSocket = m_socket;
            m_socket = INVALID_SOCKET;
            m_connectCallback(connectedSocket);
        }
    }
    else
    {
        m_isConnecting.store(false);

        if (m_connectFailedCallback)
            m_connectFailedCallback();

        cleanup();
    }
}



void TCPConnector::cleanup()
{
    if (m_socket != INVALID_SOCKET && m_spEventLoop )
    {
        m_spEventLoop->unregisterWriteEvent(m_socket, this);
    }

    closesocket(m_socket);
    m_isConnecting.store(false);
    m_socket = INVALID_SOCKET;

}

void TCPConnector::onConnectionTimeout()
{
    m_spEventLoop->removeTimer(m_timeOutTimerId);
    m_timeOutTimerId = -1;

    if (m_isConnecting.load())
    {
        if (m_connectFailedCallback)
            m_connectFailedCallback();
    }

    //m_spEventLoop->unregisterWriteEvent(m_socket,this);
}


