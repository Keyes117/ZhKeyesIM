#include "Poll.h"

#include <algorithm>

void Poll::poll(uint32_t timeOutUs, std::vector<EventDispatcher*>& triggerEventDispatchers)
{
    if (m_pollfds.empty())
    {
        return;
    }

    int timeOutMs = timeOutUs / 1000;

#ifdef _WIN32
    int numEvents = WSAPoll(m_pollfds.data(), m_pollfds.size(), timeOutMs);
#else
    int numEvents = ::poll(m_pollfds.data(), m_pollfds.size(), timeOutMs);
#endif

    if (numEvents > 0)
    {
        for (const auto& pfd : m_pollfds)
        {
            if (pfd.revents != 0)
            {
                auto it = m_EventMap.find(pfd.fd);
                if (it != m_EventMap.end())
                {
                    if (pfd.revents & POLLIN)
                        it->second->enableRead(true);
                    if (pfd.revents & POLLOUT)
                        it->second->enableWrite(true);
                    triggerEventDispatchers.push_back(it->second);
                }
            }
        }
    }
}

void Poll::registerReadEvent(SOCKET fd, EventDispatcher* dispatcher)
{
    auto it = std::find_if(m_pollfds.begin(), m_pollfds.end(),
        [fd](const pollfd& pfd) {return pfd.fd == fd; });

    if (it != m_pollfds.end())
    {
        it->events |= POLLIN;
    }
    else
    {
        pollfd pfd;
        pfd.fd = fd;
        pfd.events = POLLIN;
        pfd.revents = 0;
        m_pollfds.push_back(std::move(pfd));
    }
    m_EventMap[fd] = dispatcher;
}

void Poll::registerWriteEvent(SOCKET fd, EventDispatcher* dispatcher)
{
    auto it = std::find_if(m_pollfds.begin(), m_pollfds.end(),
        [fd](const pollfd& pfd) {return pfd.fd == fd; });

    if (it != m_pollfds.end())
    {
        it->events |= POLLOUT;
    }
    else
    {
        pollfd pfd;
        pfd.fd = fd;
        pfd.events = POLLOUT;
        pfd.revents = 0;
        m_pollfds.push_back(std::move(pfd));
    }

    m_EventMap[fd] = dispatcher;
}

void Poll::unRegisterReadEvent(SOCKET fd, EventDispatcher* dispatcher)
{
    auto it = std::find_if(m_pollfds.begin(), m_pollfds.end(),
        [fd](const pollfd& pfd) {return pfd.fd == fd; });

    if (it != m_pollfds.end())
    {
        it->events &= ~POLLIN;
    }

    auto itr = std::find_if(m_pollfds.begin(), m_pollfds.end(),
        [fd](const pollfd& pfd) {return pfd.fd == fd; });

    if (itr != m_pollfds.end() && !(it->events & POLLOUT))
    {
        m_EventMap.erase(fd);
        m_pollfds.erase(it);
    }
}

void Poll::unRegisterWriteEvent(SOCKET fd, EventDispatcher* dispatcher)
{
    auto it = std::find_if(m_pollfds.begin(), m_pollfds.end(),
        [fd](const pollfd& pfd) {return pfd.fd == fd; });

    if (it != m_pollfds.end())
    {
        it->events &= ~POLLOUT;
    }

    auto itr = std::find_if(m_pollfds.begin(), m_pollfds.end(),
        [fd](const pollfd& pfd) {return pfd.fd == fd; });

    if (itr != m_pollfds.end() && !(it->events & POLLIN))
    {
        m_EventMap.erase(fd);
        m_pollfds.erase(it);
    }
}

void Poll::unRegisterAllEvent(SOCKET fd, EventDispatcher* dispatcher)
{
    auto it = std::find_if(m_pollfds.begin(), m_pollfds.end(),
        [fd](const pollfd& pfd) { return pfd.fd == fd; });

    if (it != m_pollfds.end()) {
        m_pollfds.erase(it);
    }
    m_EventMap.erase(fd);
}