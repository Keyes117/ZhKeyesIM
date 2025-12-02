#include "Select.h"

Select::Select()
#ifndef _WIN32
    :m_maxfd{ -1 }
#endif
{
    FD_ZERO(&m_readfds);
    FD_ZERO(&m_writefds);
    FD_ZERO(&m_exceptfds);
}

void Select::poll(uint32_t timeOutUs, std::vector<EventDispatcher*>& triggerEventDispatchers)
{

    fd_set readfds = m_readfds;
    fd_set writefds = m_writefds;
    fd_set exceptfds = m_exceptfds;

    struct timeval timeout;

    timeout.tv_sec = timeOutUs / 1000000;
    timeout.tv_usec = timeOutUs % 1000000;

#ifdef _WIN32
    int numEvents = ::select(0, &readfds, &writefds, &exceptfds, &timeout);
#else
    int numEvents = ::select(m_maxfd + 1, &readfds, &writefds, &exceptfds, &timeout);
#endif

    //先重置所有读写标志
    for (const auto& pair : m_EventMap)
    {
        if (pair.second)
        {
            pair.second->enableRead(false);
            pair.second->enableWrite(false);
        }
    }

    for (const auto& pair : m_EventMap)
    {
#ifdef _WIN32
        SOCKET socket = pair.first;

#else
        int socket = pair.first;

#endif // _WIN32

        EventDispatcher* dispatcher = pair.second;

        if (FD_ISSET(socket, &readfds) || FD_ISSET(socket, &writefds) || FD_ISSET(socket, &exceptfds))
        {

            if (dispatcher)
            {
                if (FD_ISSET(socket, &readfds))
                    dispatcher->enableRead(true);
                if (FD_ISSET(socket, &writefds))
                    dispatcher->enableWrite(true);
                triggerEventDispatchers.push_back(dispatcher);
            }

        }

    }
}

void Select::registerReadEvent(SOCKET fd, EventDispatcher* dispatcher)
{
#ifdef _WIN32
    FD_SET(fd, &m_readfds);
    m_EventMap[fd] = dispatcher;
#else
    FD_SET(fd, &m_readfds);
    m_EventMap[fd] = dispatcher;
    if (fd > m_maxfd)
    {
        m_maxfd = fd;
    }


#endif
}

void Select::registerWriteEvent(SOCKET fd, EventDispatcher* dispatcher)
{
#ifdef _WIN32
    FD_SET(fd, &m_writefds);
    m_EventMap[fd] = dispatcher;
#else
    FD_SET(fd, &m_writefds);
    m_EventMap[fd] = dispatcher;
    if (fd > m_maxfd)
    {
        m_maxfd = fd;
    }
#endif
}

void Select::unRegisterReadEvent(SOCKET fd, EventDispatcher* dispatcher)
{
#ifdef _WIN32

    FD_CLR(fd, &m_readfds);

    if (!FD_ISSET(fd, &m_writefds))
    {
        m_EventMap.erase(fd);
    }
#else
    FD_CLR(fd, &m_readfds);

    if (!FD_ISSET(fd, &m_writefds))
    {
        m_EventMap.erase(fd);

        if (fd == m_maxfd)
        {
            m_maxfd = -1;
            for (const auto& pair : m_EventMap)
            {
                if (pair.first > m_maxfd)
                {
                    m_maxfd = pair.first;
                }
            }
        }
    }
#endif
}

void Select::unRegisterWriteEvent(SOCKET fd, EventDispatcher* dispatcher)
{
#ifdef _WIN32

    FD_CLR(fd, &m_writefds);

    if (!FD_ISSET(fd, &m_readfds))
    {
        m_EventMap.erase(fd);
    }
#else
    FD_CLR(fd, &m_writefds);

    if (!FD_ISSET(fd, &m_readfds))
    {
        m_EventMap.erase(fd);

        if (fd == m_maxfd)
        {
            m_maxfd = -1;
            for (const auto& pair : m_EventMap)
            {
                if (pair.first > m_maxfd)
                {
                    m_maxfd = pair.first;
                }
            }
        }
    }
#endif
}

void Select::unRegisterAllEvent(SOCKET fd, EventDispatcher* dispatcher)
{
#ifdef _WIN32
    FD_CLR(fd, &m_readfds);
    FD_CLR(fd, &m_writefds);
    FD_CLR(fd, &m_exceptfds);
    m_EventMap.erase(fd);
#else
    FD_CLR(fd, &m_readfds);
    FD_CLR(fd, &m_writefds);
    FD_CLR(fd, &m_exceptfds);
    m_EventMap.erase(fd);

    // 重新计算maxfd
    if (fd == m_maxfd) {
        m_maxfd = -1;
        for (const auto& pair : m_EventMap) {
            if (pair.first > m_maxfd) {
                m_maxfd = pair.first;
            }
        }
    }
#endif
}

