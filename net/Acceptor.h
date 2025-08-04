#pragma once

#ifndef NET_ACCEPTOR_H_
#define NET_ACCEPTOR_H_

#ifdef _WIN32

#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#endif

#include <functional>
#include <string>

#include "common.h"
#include "EventDispatcher.h"
#include "EventLoop.h"
#include "net_export.h"

using AcceptCallback = std::function<void(SOCKET clientSocket)>;

class NET_API Acceptor final : public EventDispatcher
{
public:
    Acceptor(EventLoop* pEventLoop);
    ~Acceptor();

    void setAcceptCallback(AcceptCallback&& callback)
    {
        m_acceptCallback = std::move(callback);
    }

    bool startListen(const std::string& ip, int16_t port);
    void stopListen();
    SOCKET getListenSocket() const { return m_listenSocket; }

    virtual void onRead() override;
    virtual void onWrite() override;
    virtual void onClose() override;

    virtual void enableRead(bool isEnabled) override;
    virtual void enableWrite(bool isEnabled) override;

private:
    bool createListenSocket();
    bool setSocketOption();
    bool bindAddress(const std::string& ip, int16_t port);
    bool startListening();

private:
    SOCKET          m_listenSocket;
    bool            m_listening{ false };
    bool            m_enableRead{ false };

    EventLoop* m_EventLoop;
    AcceptCallback      m_acceptCallback;
};

#endif

