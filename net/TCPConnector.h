#ifndef NET_CONNECTOR_H_
#define NET_CONNECTOR_H_


#include <atomic>
#include <chrono>
#include <functional>
#include <string>

#include "common.h"
#include "EventDispatcher.h"
#include "EventLoop.h"

using ConnectCallback = std::function<void(SOCKET fd)>;
using ConnectFailedCallback = std::function<void()>;

class TCPConnector : EventDispatcher
{
public:
    TCPConnector(const std::shared_ptr<EventLoop>& spEventLoop);
    virtual ~TCPConnector();


    bool startConnect(const std::string& serverIP, uint16_t serverPort, uint32_t timeoutMs = 5000);

    void cancelConnect();

    bool isConnecting() const { return m_isConnecting; }


    void setConnectCallback(ConnectCallback&& callback)
    {
        m_connectCallback = std::move(callback);
    }

    void setConnectFailedCallback(ConnectFailedCallback&& callback)
    {
        m_connectFailedCallback = std::move(callback);
    }

    virtual void enableRead(bool isEnabled) override
    {
        m_enableRead = isEnabled;
    }

    virtual void enableWrite(bool isEnabled) override
    {
        m_enableWrite = isEnabled;
    }

    virtual void onRead() override;
    virtual void onWrite() override;
    virtual void onClose() override;



private:
    bool createSocket();

    bool setSocketOption();

    bool connect();

    void checkConnectResult();

    void cleanup();

private:
    std::shared_ptr<EventLoop>  m_spEventLoop;

    SOCKET                      m_socket;
    std::string                 m_serverIP;
    uint16_t                    m_serverPort;
    uint16_t                    m_connectTimeoutMs;

    std::atomic<bool>           m_isConnecting;
    bool                        m_enableRead;
    bool                        m_enableWrite;

    std::chrono::steady_clock::time_point   m_connectStartTime;

    ConnectCallback             m_connectCallback;
    ConnectFailedCallback       m_connectFailedCallback;


    TCPConnector(const TCPConnector&) = delete;
    TCPConnector& operator=(const TCPConnector&) = delete;
    TCPConnector(TCPConnector&&) = delete;
    TCPConnector& operator=(TCPConnector&&) = delete;
};


#endif
