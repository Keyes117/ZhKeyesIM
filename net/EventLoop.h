#ifndef NET_EVENTLOOP_H_
#define NET_EVENTLOOP_H_

#include <memory>
#include <thread>

#include "common.h"
#include "Epoll.h"
#include "net_export.h"
#include "Poll.h"
#include "Select.h"
enum class IOMultiplexType
{
    Select = 0,
    Poll,
    Epoll
};

class NET_API EventLoop : public std::enable_shared_from_this<EventLoop>
{
public:
    EventLoop() = default;
    ~EventLoop() = default;

    void init(IOMultiplexType type = IOMultiplexType::Epoll);
    void run();
    void stop();

    void setThreadID(const std::thread::id& threadID)
    {
        m_threadID = threadID;
    }
    std::thread::id getThreadID() const
    {
        return m_threadID;
    }

    void registerReadEvent(SOCKET fd, EventDispatcher* dispatcher);
    void registerWriteEvent(SOCKET fd, EventDispatcher* dispatcher);
    void unregisterReadEvent(SOCKET fd, EventDispatcher* dispatcher);
    void unregisterWriteEvent(SOCKET fd, EventDispatcher* dispatcher);

private:
    bool m_running = false;

    std::unique_ptr<IOMultiplex>    m_spIOMultiplex;

    std::thread::id                 m_threadID;

private:
    EventLoop(const EventLoop&) = delete;
    EventLoop& operator=(const EventLoop&) = delete;
    EventLoop(EventLoop&&) = delete;
    EventLoop& operator=(EventLoop&&) = delete;
};

#endif

