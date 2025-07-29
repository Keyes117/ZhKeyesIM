#include "EventLoop.h"
#include "ThreadPool.h"

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
    while (m_running)
    {



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
}


void EventLoop::stop()
{
    if (m_running)
    {
        m_running = false;
    }
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