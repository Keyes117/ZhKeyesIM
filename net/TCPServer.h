#ifndef NET_TCPSERVER_H_
#define NET_TCPSERVER_H_

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "Logger.h"

#include "Acceptor.h"
#include "common.h"
#include "TCPConnection.h"
#include "ThreadPool.h"



#include "net_export.h"

using ConnectionCallback = std::function<void(std::shared_ptr<TCPConnection>& spConn)>;
using DisConnectioncallback = std::function<void(const std::shared_ptr<TCPConnection>& spConn)>;

class NET_API TCPServer
{
public:
    TCPServer();
    ~TCPServer();

    bool init(int32_t threadNum, const std::string& ip, uint16_t port);
    void start();
    void shutdown();

    void setConnectionCallback(ConnectionCallback&& callback)
    {
        m_connectionCallback = std::move(callback);
    }

    void setDisConnectionCallback(DisConnectioncallback&& callback)
    {
        m_disConnectionCallback = std::move(callback);
    }

    EventLoop& getBaseEventLoop()
    {
        return m_baseEventLoop;
    }

    void onAccept(SOCKET clientfd);
    void onConnected(std::shared_ptr<TCPConnection>& spConn);
    void onDisConnected(SOCKET clientfd);

private:

    std::string             m_ip;
    uint16_t                m_port;

    ThreadPool              m_threadPool;
    EventLoop               m_baseEventLoop;
    Acceptor                m_acceptor;

    std::mutex              m_connectionMutex;

    std::unordered_map<SOCKET, std::shared_ptr<TCPConnection>> m_connections;

    ConnectionCallback      m_connectionCallback;
    DisConnectioncallback   m_disConnectionCallback;
};

#endif
