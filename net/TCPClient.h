#ifndef NET_TCPCLIENT_H_
#define NET_TCPCLIENT_H_

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "common.h"
#include "EventLoop.h"
#include "net_export.h"
#include "TCPConnection.h"
#include "TCPConnector.h"

using ClientConnectionCallback = std::function<void(std::shared_ptr<TCPConnection>& spConn)>;
using ClientConnectionFailedCallback = std::function<void()>;
using ClientDisconnectionCallback = std::function<void()>;


class NET_API TCPClient
{
public:
    explicit TCPClient(std::shared_ptr<EventLoop> eventLoop);
    ~TCPClient();

    bool init(const std::string& serverIP, uint16_t serverPort,
        uint32_t timeoutMs = 5000);


    bool connect();             //连接
    void cancelConnect();       //取消连接
    void disconnect();          //断开连接

    bool isConnecting() const { return m_connecting.load(); }
    bool isConnected() const { return m_connected.load(); }

    // 获取当前连接
    std::shared_ptr<TCPConnection> getConnection() const { return m_spConnection; }

    bool send(const char* data, size_t len);
    bool send(const std::string& data);

    void setConnectionCallback(ClientConnectionCallback&& callback)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_connectionCallback = std::move(callback);
    }

    void setConnectionFailedCallback(ClientConnectionFailedCallback&& callback)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_connectionFailedCallback = std::move(callback);
    }

    void setDisconnectedCallback(ClientDisconnectionCallback&& callback)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_disconnectionCallback = std::move(callback);
    }
private:
    void onConnected(SOCKET socket);
    void onConnectFailed();
    void onDisconnected();

    void cleanup();
    bool createConnector();
private:

    std::atomic<bool>               m_connecting{ false };
    std::atomic<bool>               m_connected{ false };
    std::atomic<bool>               m_initialized{ false };

    std::string                     m_serverIP{ 0 };
    uint16_t                        m_serverPort{ 0 };
    uint32_t                        m_timeoutMs{ 5000 };


    std::shared_ptr<EventLoop>      m_spEventLoop;
    std::shared_ptr<TCPConnector>   m_spConnector;
    std::shared_ptr<TCPConnection>  m_spConnection;

    ClientConnectionCallback        m_connectionCallback;
    ClientConnectionFailedCallback  m_connectionFailedCallback;
    ClientDisconnectionCallback     m_disconnectionCallback;

    mutable  std::mutex             m_mutex;

private:
    TCPClient(const TCPClient&) = delete;
    TCPClient& operator=(const TCPClient&) = delete;
    TCPClient(TCPClient&&) = delete;
    TCPClient& operator=(TCPClient&&) = delete;

};


#endif