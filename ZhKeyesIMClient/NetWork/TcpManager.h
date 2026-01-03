#ifndef ZHKEYESIMCLIENT_NETWORK_TCPMANAGER_H_
#define ZHKEYESIMCLIENT_NETWORK_TCPMANAGER_H_


#include <memory>

#include "EventLoop.h"
#include "TCPClient.h"


class TcpManager
{
public:
    TcpManager(std::shared_ptr<EventLoop> eventLoop);
    ~TcpManager() = default;

    bool connect(const std::string& ip, uint16_t port);

private:
    std::shared_ptr<EventLoop>  m_spEventLoop;
    std::unique_ptr<TCPClient>  m_spTcpClient;

};


#endif