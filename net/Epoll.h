#ifndef NET_EPOLL_H_
#define NET_EPOLL_H_

#include "IOMultiplex.h"

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <Windows.h>
#else
#include <sys/epoll.h>
#endif

#include <vector>
#include <unordered_map>

class Epoll : public IOMultiplex
{
public:
    Epoll();
    virtual ~Epoll();

    virtual void poll(uint32_t timeOutUs, std::vector<EventDispatcher*>& triggerEventDispatchers) override;

    virtual void registerReadEvent(SOCKET fd, EventDispatcher* dispatcher) override;
    virtual void registerWriteEvent(SOCKET fd, EventDispatcher* dispatcher) override;

    virtual void unRegisterReadEvent(SOCKET fd, EventDispatcher* dispatcher) override;
    virtual void unRegisterWriteEvent(SOCKET fd, EventDispatcher* dispatcher) override;

    virtual void unRegisterAllEvent(SOCKET fd, EventDispatcher* dispatcher) override;

private:
#ifdef _WIN32

    HANDLE m_iocp;
    std::unordered_map<SOCKET, EventDispatcher*> m_EventMap;
    std::vector<OVERLAPPED_ENTRY> m_completionEntries;
#else
    int m_epollfd;
    std::unordered_map<SOCKET, int32_t>  m_fdEventFlag;
#endif
};

#endif