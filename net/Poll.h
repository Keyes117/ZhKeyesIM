#ifndef NET_POLL_H
#define NET_POLL_H
#include "IOMultiplex.h"

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <poll.h>
#endif

#include <vector>
#include <unordered_map>

class Poll : public IOMultiplex
{
public:
    Poll() = default;
    virtual ~Poll() = default;

    virtual void poll(uint32_t timeOutUs, std::vector<EventDispatcher*>& triggerEventDispatchers) override;

    virtual void registerReadEvent(SOCKET fd, EventDispatcher* dispatcher) override;
    virtual void registerWriteEvent(SOCKET fd, EventDispatcher* dispatcher) override;

    virtual void unRegisterReadEvent(SOCKET fd, EventDispatcher* dispatcher) override;
    virtual void unRegisterWriteEvent(SOCKET fd, EventDispatcher* dispatcher) override;

    virtual void unRegisterAllEvent(SOCKET fd, EventDispatcher* dispatcher) override;
private:
    std::vector<pollfd> m_pollfds;
    std::unordered_map<SOCKET, EventDispatcher*> m_EventMap;
};

#endif