#ifndef ZHKEYESIMCLIENT_NETWORK_TCPMANAGER_H_
#define ZHKEYESIMCLIENT_NETWORK_TCPMANAGER_H_


#include <memory>

#include "Buffer.h"
#include "EventLoop.h"
#include "TCPClient.h"
#include "TCPConnection.h"


class TcpManager
{
public:
    TcpManager(std::shared_ptr<EventLoop> eventLoop);
    ~TcpManager() = default;

    bool connect(const std::string& ip, uint16_t port);

    bool authenticate(const std::string& token, uint32_t uid);

private:

    void onTcpResponse(Buffer& recvBuf);

    void onConnected(std::shared_ptr<TCPConnection> spConn);
    void onConnectFailed();
private:
    std::shared_ptr<EventLoop>  m_spEventLoop;
    std::unique_ptr<TCPClient>  m_spTcpClient;


    std::function<void()> m_connectionCallback;
};


#endif