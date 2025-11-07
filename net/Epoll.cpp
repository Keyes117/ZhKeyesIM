#include "Epoll.h"


#include <string.h>

#include "Logger.h"
#include "Util.h"

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
    // 清理所有资源
    for (auto& pair : m_readContexts) {
        pair.second->cleanup();
    }
    m_readContexts.clear();

    for (auto& pair : m_writeContexts) {
        pair.second->cleanup();
    }
    m_writeContexts.clear();

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

    if (n < 0)
    {
        LOG_ERROR("epoll_wait failed: errno = %d", errno);
    }

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

    auto it = m_readContexts.find(fd);
    if (it != m_readContexts.end()) {
        // 如果之前的操作还在进行，先取消
        if (HasOverlappedIoCompleted(&it->second->overlapped) == FALSE) {
            CancelIoEx((HANDLE)fd, &it->second->overlapped);
        }
        it->second->cleanup();
        m_readContexts.erase(it);
    }



    m_EventMap[fd] = dispatcher;

    HANDLE hIocp = CreateIoCompletionPort((HANDLE)fd, m_iocp, (ULONG_PTR)fd, 0);
    if (hIocp == NULL) {
        int error = GetLastError();
        // 如果 socket 已经关联到同一个 IOCP，不会返回 NULL
        // 所以这里返回 NULL 说明 socket 可能无效或已关联到其他 IOCP
        LOG_ERROR("Failed to associate socket with IOCP, error: %d, fd: %d", error, fd);
        return;
    }

    // 检查返回的 IOCP 是否是我们期望的
    if (hIocp != m_iocp) {
        LOG_ERROR("Socket already associated with different IOCP, fd: %d", fd);
        return;
    }

    int socketType = SOCK_STREAM;
    int optLen = sizeof(socketType);
    if (::getsockopt(fd, SOL_SOCKET, SO_TYPE, (char*)&socketType, &optLen) != 0)
    {
        LOG_ERROR("Failed to get socket type, error: %d", GetSocketError());
        socketType = SOCK_STREAM;
    }

    // 创建新的读上下文
    auto context = std::make_unique<OverlappedContext>(true);

    // 分配缓冲区
    context->buffer = new char[8192];
    context->wsaBuf.buf = context->buffer;
    context->wsaBuf.len = 8192;

    DWORD bytesReceived = 0;
    DWORD flags = 0;

    // 启动异步读操作
    int result = SOCKET_ERROR;
    if (socketType == SOCK_STREAM)
    {
        result = WSARecv(fd, &context->wsaBuf, 1, &bytesReceived, &flags,
            &context->overlapped, NULL);
    }
    else
    {
        sockaddr_in fromAddr;
        int fromLen = sizeof(fromAddr);
        result = WSARecvFrom(fd, &context->wsaBuf, 1, &bytesReceived, &flags,
            (sockaddr*)&fromAddr, &fromLen, &context->overlapped, NULL);
    }


    // 如果操作不是立即完成且没有错误，保存上下文
    if (result == SOCKET_ERROR)
    {
        int error = WSAGetLastError();
        if (error != WSA_IO_PENDING)
        {
            // 真正的错误，清理资源
            context->cleanup();

            // 对于 UDP 套接字，某些错误可以忽略或特殊处理
            if (socketType == SOCK_DGRAM)
            {
                // UDP 套接字在未连接状态下，某些错误可能是正常的
                // 但 WSAENOTCONN 对于 UDP 来说不应该出现，可能是套接字状态问题
                if (error == WSAENOTCONN)
                {
                    LOG_WARN("UDP socket WSARecvFrom returned WSAENOTCONN, fd: %d. This may be normal for unconnected UDP socket.", fd);
                    // 对于 wakeUpSocket，可以忽略这个错误，继续注册
                    // 但需要确保套接字状态正确
                }
                else
                {
                    LOG_ERROR("UDP socket WSARecvFrom failed, error: %d, fd: %d", error, fd);
                }
            }
            else
            {
                // TCP 套接字的错误处理
                if (error == WSAENOTCONN || error == WSAECONNRESET || error == WSAECONNABORTED)
                {
                    LOG_INFO("TCP socket disconnected, error: %d, fd: %d", error, fd);
                    if (dispatcher)
                    {
                        dispatcher->onClose();
                    }
                }
                else
                {
                    LOG_ERROR("TCP socket WSARecv failed, error: %d, fd: %d", error, fd);
                }
            }
            return;
        }
    }

    // 保存上下文（IO操作已提交）
    m_readContexts[fd] = std::move(context);
#else
    int32_t eventFlag = 0;

    int operation;

    auto iter = m_fdEventFlag.find(fd);
    if (iter == m_fdEventFlag.end())
    {
        eventFlag |= EPOLLIN;
        m_fdEventFlag[fd] = eventFlag;
        //fd 未注册， 则使用 EPOLL_CTL_ADD
        operation = EPOLL_CTL_ADD;
    }
    else
    {
        eventFlag = iter->second;
        if (eventFlag & EPOLLIN)
            return;

        eventFlag |= EPOLLIN;
        m_fdEventFlag[fd] = eventFlag;
        //已经存在 则使用EPOLL_CTL_MOD
        operation = EPOLL_CTL_MOD;
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
void  Epoll::registerWriteEvent(SOCKET fd, EventDispatcher* dispatcher)
{
#ifdef _WIN32

    m_EventMap[fd] = dispatcher;

    // 如果已经存在写上下文，先清理
    auto it = m_writeContexts.find(fd);
    if (it != m_writeContexts.end()) {
        // 如果之前的操作还在进行，先取消
        if (HasOverlappedIoCompleted(&it->second->overlapped) == FALSE) {
            CancelIoEx((HANDLE)fd, &it->second->overlapped);
        }
        it->second->cleanup();
        m_writeContexts.erase(it);
    }

    m_EventMap[fd] = dispatcher;

    // 创建新的写上下文（写操作不需要预先分配缓冲区，在实际发送时分配）
    auto context = std::make_unique<OverlappedContext>(false);

    CreateIoCompletionPort((HANDLE)fd, m_iocp, (ULONG_PTR)fd, 0);

    // 写操作的上下文已经准备好，缓冲区在实际发送时分配
    m_writeContexts[fd] = std::move(context);

#else
    int32_t eventFlag = 0;
    int operation;


    auto iter = m_fdEventFlag.find(fd);
    if (iter == m_fdEventFlag.end())
    {
        eventFlag |= EPOLLOUT;
        m_fdEventFlag[fd] = eventFlag;
        operation = EPOLL_CTL_ADD;
    }
    else
    {
        eventFlag = iter->second;
        if (eventFlag & EPOLLOUT)
            return;

        eventFlag |= EPOLLOUT;
        m_fdEventFlag[fd] = eventFlag;
        operation = EPOLL_CTL_MOD;
    }


    struct epoll_event iEvent;
    memset(&iEvent, 0, sizeof(iEvent));
    iEvent.events = eventFlag;

    iEvent.data.ptr = dispatcher;
    if (::epoll_ctl(m_epollfd, operation, fd, &iEvent) < 0)
    {
        //TODO:: 打印错误日志
        LOG_ERROR("epoll_ctl failed: operation=%d, fd=%d, errno=%d", operation, fd, errno);
        Util::crash();
    }
#endif
}

void  Epoll::unRegisterReadEvent(SOCKET fd, EventDispatcher* dispatcher)
{
#ifdef _WIN32

    // 清理读上下文资源
    cleanupSocketResources(fd);

    // 从事件映射中移除
    if (m_writeContexts.find(fd) == m_writeContexts.end()) {
        // 如果没有写上下文，也从事件映射中移除
        m_EventMap.erase(fd);
    }
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
        LOG_ERROR("epoll_ctl failed: operation=%d, fd=%d, errno=%d", operation, fd, errno);
        Util::crash();
    }
#endif
}
void  Epoll::unRegisterWriteEvent(SOCKET fd, EventDispatcher* dispatcher)
{
#ifdef _WIN32
    // 清理写上下文资源
    auto writeIt = m_writeContexts.find(fd);
    if (writeIt != m_writeContexts.end()) {
        if (HasOverlappedIoCompleted(&writeIt->second->overlapped) == FALSE) {
            CancelIoEx((HANDLE)fd, &writeIt->second->overlapped);
        }
        DWORD bytesTransferred;
        GetOverlappedResult((HANDLE)fd, &writeIt->second->overlapped, &bytesTransferred, FALSE);
        writeIt->second->cleanup();
        m_writeContexts.erase(writeIt);
    }

    // 从事件映射中移除
    if (m_readContexts.find(fd) == m_readContexts.end()) {
        // 如果没有读上下文，也从事件映射中移除
        m_EventMap.erase(fd);
    }
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
        LOG_ERROR("epoll_ctl failed: operation=%d, fd=%d, errno=%d", operation, fd, errno);
        Util::crash();
    }
#endif
}

void  Epoll::unRegisterAllEvent(SOCKET fd, EventDispatcher* dispatcher)
{
#ifdef _WIN32
    // 清理所有资源
    cleanupSocketResources(fd);

    // 从事件映射中移除
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

#ifdef _WIN32
void Epoll::cleanupSocketResources(SOCKET fd)
{
    // 取消可能正在进行的异步操作
    auto readIt = m_readContexts.find(fd);
    if (readIt != m_readContexts.end())
    {
        // 取消读操作
        if (HasOverlappedIoCompleted(&readIt->second->overlapped) == FALSE)
        {
            CancelIoEx((HANDLE)fd, &readIt->second->overlapped);
        }
        // 等待操作完成或超时
        DWORD bytesTransferred;
        if (GetOverlappedResult((HANDLE)fd, &readIt->second->overlapped, &bytesTransferred, FALSE) == FALSE)
        {
            // 操作已取消或完成
        }
        readIt->second->cleanup();
        m_readContexts.erase(readIt);
    }

    auto writeIt = m_writeContexts.find(fd);
    if (writeIt != m_writeContexts.end())
    {
        // 取消写操作
        if (HasOverlappedIoCompleted(&writeIt->second->overlapped) == FALSE)
        {
            CancelIoEx((HANDLE)fd, &writeIt->second->overlapped);
        }
        // 等待操作完成或超时
        DWORD bytesTransferred;
        if (GetOverlappedResult((HANDLE)fd, &writeIt->second->overlapped, &bytesTransferred, FALSE) == FALSE)
        {
            // 操作已取消或完成
        }
        writeIt->second->cleanup();
        m_writeContexts.erase(writeIt);
    }
}

#endif