#include "EventLoop.h"
#include "Logger.h"

void EventLoop::init(IOMultiplexType type)
{
    if (m_running)
        return;

    if (type == IOMultiplexType::Select)
        m_spIOMultiplex = std::make_unique<Select>();
    else if (type == IOMultiplexType::Poll)
        m_spIOMultiplex = std::make_unique<Poll>();
    else if (type == IOMultiplexType::Epoll)
        m_spIOMultiplex = std::make_unique<Epoll>();

    m_running = true;
}

void EventLoop::run()
{
    m_running = true;
    m_threadID = std::this_thread::get_id();

    while (m_running)
    {

        //TODO: 完成定时器和唤醒逻辑

        std::vector<EventDispatcher*> activeDispatchers;
        m_spIOMultiplex->poll(1000000, activeDispatchers);


        for (auto dispatcher : activeDispatchers)
        {
            if (dispatcher)
            {
                dispatcher->onRead();
                dispatcher->onWrite();
            }
        }
    }

    m_running = false;
}
void EventLoop::stop()
{
    m_running = false;
}

void EventLoop::registerReadEvent(SOCKET fd, EventDispatcher* dispatcher)
{
    if (m_spIOMultiplex)
        m_spIOMultiplex->registerReadEvent(fd, dispatcher);
}

void EventLoop::registerWriteEvent(SOCKET fd, EventDispatcher* dispatcher)
{
    if (m_spIOMultiplex)
        m_spIOMultiplex->registerWriteEvent(fd, dispatcher);
}

void EventLoop::unregisterReadEvent(SOCKET fd, EventDispatcher* dispatcher)
{
    if (m_spIOMultiplex)
        m_spIOMultiplex->unRegisterReadEvent(fd, dispatcher);
}

void EventLoop::unregisterWriteEvent(SOCKET fd, EventDispatcher* dispatcher)
{
    if (m_spIOMultiplex)
        m_spIOMultiplex->unRegisterWriteEvent(fd, dispatcher);
}