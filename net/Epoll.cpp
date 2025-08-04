#include "Epoll.h"
#include "Util.h"

#include <string.h>

Epoll::Epoll()
#ifdef _WIN32
    :m_iocp(NULL)
#else
    : m_epollfd(-1)
#endif
{
#ifdef _WIN32
    m_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (m_iocp == NULL)
    {
        Util::crash();
    }
    m_completionEntries.resize(64);
#else

    m_epollfd = ::epoll_create1(EPOLL_CLOEXEC);
    if (m_epollfd == -1)
    {
        Util::crash();
    }

#endif
}

Epoll::~Epoll()
{
#ifdef _WIN32
    if (m_iocp)
    {
        CloseHandle(m_iocp);
    }
#else
    if (m_epollfd != -1)
    {
        closesocket(m_epollfd);
    }

#endif
}

void Epoll::poll(uint32_t timeOutUs, std::vector<EventDispatcher*>& triggerEventDispatchers)
{
#ifdef _WIN32
    //Window iocp实现
    ULONG numEntries = 0;
    DWORD timeoutMs = timeOutUs / 1000;

    if (GetQueuedCompletionStatusEx(m_iocp,
        m_completionEntries.data(),
        m_completionEntries.size(),
        &numEntries,
        timeoutMs,
        FALSE))
    {
        for (ULONG i = 0; i < numEntries; i++) {
            SOCKET sock = (SOCKET)m_completionEntries[i].lpCompletionKey;
            auto it = m_EventMap.find(sock);
            if (it != m_EventMap.end()) {
                triggerEventDispatchers.push_back(it->second);
            }
        }
    }
#else
    struct epoll_event events[1024];
    int timeoutMs = timeOutUs / 1000;

    bool enableRead = false;
    bool enableWrite = false;

    int n = ::epoll_wait(m_epollfd, events, 1024, timeoutMs);
    for (int i = 0; i < n; i++)
    {
        if (events[i].events & EPOLLIN)
            enableRead = true;
        else
            enableRead = false;

        if (events[i].events & EPOLLOUT)
            enableWrite = true;
        else
            enableWrite = false;

        EventDispatcher* dispatcher = static_cast<EventDispatcher*>(events[i].data.ptr);

        dispatcher->enableRead(enableRead);
        dispatcher->enableWrite(enableWrite);

        triggerEventDispatchers.push_back(dispatcher);
    }
#endif // _WIN32

}

void  Epoll::registerReadEvent(SOCKET fd, EventDispatcher* dispatcher)
{
#ifdef _WIN32

    m_EventMap[fd] = dispatcher;

    OVERLAPPED* overlapped = new OVERLAPPED();
    ZeroMemory(overlapped, sizeof(OVERLAPPED));

    CreateIoCompletionPort((HANDLE)fd, m_iocp, (ULONG_PTR)fd, 0);

    WSABUF wsaBuf;
    DWORD bytesReceived;
    DWORD flags = 0;
    wsaBuf.buf = new char[8192];
    wsaBuf.len = 8192;

    WSARecv(fd, &wsaBuf, 1, &bytesReceived, &flags, overlapped, NULL);
#else
    int32_t eventFlag = 0;

    auto iter = m_fdEventFlag.find(fd);
    if (iter == m_fdEventFlag.end())
    {
        eventFlag |= EPOLLIN;
        m_fdEventFlag[fd] = eventFlag;
    }
    else
    {
        eventFlag = iter->second;
        if (eventFlag & EPOLLIN)
            return;

        eventFlag |= EPOLLIN;
        m_fdEventFlag[fd] = eventFlag;
    }


    struct epoll_event iEvent;
    memset(&iEvent, 0, sizeof(iEvent));
    iEvent.events = eventFlag;

    iEvent.data.ptr = dispatcher;
    if (::epoll_ctl(m_epollfd, EPOLL_CTL_ADD, fd, &iEvent) < 0)
    {
        //TODO:: 打印错误日志
        Util::crash();
    }
#endif
}
void  Epoll::registerWriteEvent(SOCKET fd, EventDispatcher* dispatcher)
{
#ifdef _WIN32

    m_EventMap[fd] = dispatcher;

    // 创建重叠IO结构
    OVERLAPPED* overlapped = new OVERLAPPED();
    ZeroMemory(overlapped, sizeof(OVERLAPPED));

    // 关联socket到IOCP
    CreateIoCompletionPort((HANDLE)fd, m_iocp, (ULONG_PTR)fd, 0);
#else
    int32_t eventFlag = 0;

    auto iter = m_fdEventFlag.find(fd);
    if (iter == m_fdEventFlag.end())
    {
        eventFlag |= EPOLLOUT;
        m_fdEventFlag[fd] = eventFlag;
    }
    else
    {
        eventFlag = iter->second;
        if (eventFlag & EPOLLOUT)
            return;

        eventFlag |= EPOLLOUT;
        m_fdEventFlag[fd] = eventFlag;
    }


    struct epoll_event iEvent;
    memset(&iEvent, 0, sizeof(iEvent));
    iEvent.events = eventFlag;

    iEvent.data.ptr = dispatcher;
    if (::epoll_ctl(m_epollfd, EPOLL_CTL_ADD, fd, &iEvent) < 0)
    {
        //TODO:: 打印错误日志
        Util::crash();
    }
#endif
}

void  Epoll::unRegisterReadEvent(SOCKET fd, EventDispatcher* dispatcher)
{
#ifdef _WIN32

    m_EventMap.erase(fd);
#else
    int32_t eventFlag = 0;
    int operation;

    auto iter = m_fdEventFlag.find(fd);
    if (iter == m_fdEventFlag.end())
        return;
    else
    {
        eventFlag = iter->second;
        if (eventFlag & EPOLLIN)
            eventFlag &= ~EPOLLIN;

        if (eventFlag == 0)
        {
            m_fdEventFlag.erase(iter);
            operation = EPOLL_CTL_DEL;
        }
        else
        {
            m_fdEventFlag[fd] = eventFlag;
            operation = EPOLL_CTL_MOD;
        }


    }


    struct epoll_event iEvent;
    memset(&iEvent, 0, sizeof(iEvent));
    iEvent.events = eventFlag;

    iEvent.data.ptr = dispatcher;
    if (::epoll_ctl(m_epollfd, operation, fd, &iEvent) < 0)
    {
        //TODO:: 打印错误日志
        Util::crash();
    }
#endif
}
void  Epoll::unRegisterWriteEvent(SOCKET fd, EventDispatcher* dispatcher)
{
#ifdef _WIN32
    m_EventMap.erase(fd);
#else
    int32_t eventFlag = 0;
    int operation;
    auto iter = m_fdEventFlag.find(fd);
    if (iter == m_fdEventFlag.end())
        return;
    else
    {
        eventFlag = iter->second;
        if (eventFlag & EPOLLOUT)
            eventFlag &= ~EPOLLOUT;

        if (eventFlag == 0)
        {
            m_fdEventFlag.erase(iter);
            operation = EPOLL_CTL_DEL;
        }
        else
        {
            m_fdEventFlag[fd] = eventFlag;
            operation = EPOLL_CTL_MOD;
        }
    }

    struct epoll_event iEvent;
    memset(&iEvent, 0, sizeof(iEvent));
    iEvent.events = eventFlag;

    iEvent.data.ptr = dispatcher;
    if (::epoll_ctl(m_epollfd, operation, fd, &iEvent) < 0)
    {
        //TODO:: 打印错误日志
        Util::crash();
    }
#endif
}

void  Epoll::unRegisterAllEvent(SOCKET fd, EventDispatcher* dispatcher)
{
#ifdef _WIN32
    m_EventMap.erase(fd);
#else
    int32_t eventFlag = 0;
    int operation;

    auto iter = m_fdEventFlag.find(fd);
    if (iter == m_fdEventFlag.end())
        return;
    else
        m_fdEventFlag.erase(iter);

    struct epoll_event iEvent;
    memset(&iEvent, 0, sizeof(iEvent));
    iEvent.events = eventFlag;

    iEvent.data.ptr = dispatcher;
    if (::epoll_ctl(m_epollfd, EPOLL_CTL_DEL, fd, &iEvent) < 0)
    {
        //TODO:: 打印错误日志
        Util::crash();
    }

#endif
}