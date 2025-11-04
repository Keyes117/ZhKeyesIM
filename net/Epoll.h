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

#include <memory>
#include <vector>
#include <unordered_map>

#ifdef _WIN32

struct OverlappedContext
{
    OVERLAPPED overlapped;
    WSABUF wsaBuf;
    char* buffer;
    bool isReadContext;


    OverlappedContext(bool isRead):
        buffer(nullptr),isReadContext(isRead)
    {
        ZeroMemory(&overlapped, sizeof(OVERLAPPED));
        wsaBuf.buf = nullptr;
        wsaBuf.len = 0;
    }

    ~OverlappedContext()
    {
        cleanup();
    }

    void cleanup()
    {
        if (buffer)
        {
            delete[] buffer;
            buffer = nullptr;
            wsaBuf.buf = nullptr;
            wsaBuf.len = 0;
        }
    }

    // 禁止拷贝
    OverlappedContext(const OverlappedContext&) = delete;
    OverlappedContext& operator=(const OverlappedContext&) = delete;
};

#endif

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

    void cleanupSocketResources(SOCKET fd);

    HANDLE m_iocp;
    std::unordered_map<SOCKET, EventDispatcher*> m_EventMap;
    std::vector<OVERLAPPED_ENTRY> m_completionEntries;

    // 管理每个 socket 的读上下文
    std::unordered_map<SOCKET, std::unique_ptr<OverlappedContext>> m_readContexts;
    // 管理每个 socket 的写上下文
    std::unordered_map<SOCKET, std::unique_ptr<OverlappedContext>> m_writeContexts;
#else
    int m_epollfd;
    std::unordered_map<SOCKET, int32_t>  m_fdEventFlag;
#endif
};

#endif