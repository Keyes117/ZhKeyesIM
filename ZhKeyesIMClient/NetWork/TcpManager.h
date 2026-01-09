#ifndef ZHKEYESIMCLIENT_NETWORK_TCPMANAGER_H_
#define ZHKEYESIMCLIENT_NETWORK_TCPMANAGER_H_


#include <memory>

#include "net/Buffer.h"
#include "net/EventLoop.h"
#include "net/TCPClient.h"
#include "net/TCPConnection.h"

#include "IMProtocol/IMMessageSender.h"

class TcpManager : public ZhKeyesIM::Protocol::IMMessageSender
{
public:
    using ConnectionCallback = std::function<void()>;
    using ConnectionFailedCallback = std::function<void(const std::string&)>;

    TcpManager(std::shared_ptr<EventLoop> eventLoop);
    ~TcpManager() = default;

    bool connect(const std::string& ip, uint16_t port);

    bool authenticate(const std::string& token, uint32_t uid);

    void setConnectCallback(ConnectionCallback&& onSuccess)
    {
        m_connectionCallback = std::move(onSuccess);
    }
    void setConnectFailedCallback(ConnectionFailedCallback&& onFailed)
    {
        m_connectFailedCallback = std::move(onFailed);
    }

    virtual bool sendMessage(std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg);
private:
    void releaseConnectCallback();

    void onTcpResponse(Buffer& recvBuf);

    void onConnected(std::shared_ptr<TCPConnection> spConn);
    void onConnectFailed();
private:
    std::shared_ptr<EventLoop>  m_spEventLoop;
    std::unique_ptr<TCPClient>  m_spTcpClient;


    ConnectionCallback m_connectionCallback;
    ConnectionFailedCallback m_connectFailedCallback;
};


#endif