#ifndef NET_SELECT_H_
#define NET_SELECT_H_

#include "IOMultiplex.h"

/**
 *   int select(int nfds, fd_set *readfds, fd_set *writefds,
 *               fd_set *exceptfds, struct timeval *timeout);
 */
#include <unordered_map>

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>

#else
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>

#endif // _WIN32

#include "common.h"

class Select : public IOMultiplex
{
public:
    Select();
    virtual ~Select() = default;

    virtual void poll(uint32_t timeOutUs, std::vector<EventDispatcher*>& triggerEventDispatchers) override;

    virtual void registerReadEvent(SOCKET fd, EventDispatcher* dispatcher) override;
    virtual void registerWriteEvent(SOCKET fd, EventDispatcher* dispatcher) override;

    virtual void unRegisterReadEvent(SOCKET fd, EventDispatcher* dispatcher) override;
    virtual void unRegisterWriteEvent(SOCKET fd, EventDispatcher* dispatcher) override;

    virtual void unRegisterAllEvent(SOCKET fd, EventDispatcher* dispatcher) override;

private:

    fd_set  m_readfds;
    fd_set  m_writefds;
    fd_set  m_exceptfds;
    std::unordered_map<SOCKET, EventDispatcher*> m_EventMap;
#ifdef _WIN32
#else
    SOCKET     m_maxfd;

#endif // _WIN32

};

#endif