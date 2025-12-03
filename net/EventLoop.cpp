#include "EventLoop.h"

#include "common.h"

#include "Logger.h"

EventLoop::EventLoop(bool isBaseLoop):
    m_isBaseLoop(isBaseLoop)
{
}

bool EventLoop::init(IOMultiplexType type)
{
    if (m_running)
        return true;

    if (type == IOMultiplexType::Select)
        m_spIOMultiplex = std::make_unique<Select>();
    else if (type == IOMultiplexType::Poll)
        m_spIOMultiplex = std::make_unique<Poll>();
    else if (type == IOMultiplexType::Epoll)
        m_spIOMultiplex = std::make_unique<Epoll>();



    if (!createWakeUpSocket())
        return false;

    registerReadEvent(m_wakeUpSocket, m_spWakeUpEventDispatcher.get());

    m_running = true;
    return true;
}

void EventLoop::run()
{
    m_running = true;
    m_threadID = std::this_thread::get_id();

    LOG_DEBUG("thread Id : %d", m_threadID);


    while (m_running)
    {

        checkAndDoTimers();

        std::vector<EventDispatcher*> activeDispatchers;
        m_spIOMultiplex->poll(1000000, activeDispatchers);

        for (auto dispatcher : activeDispatchers) 
        {
            if (dispatcher)
            {
                if(dispatcher->isReadEnabed())
                    dispatcher->onRead();
                if(dispatcher->isWriteEnabled())
                    dispatcher->onWrite();
            }
        }

        doOtherTasks();
    }

    m_running = false;
}
void EventLoop::stop()
{
    m_running = false;
}

void EventLoop::registerReadEvent(SOCKET fd, EventDispatcher* dispatcher)
{
    if (isCallableInOwnerThread())
    {
        if (m_spIOMultiplex)
            m_spIOMultiplex->registerReadEvent(fd, dispatcher);
    }
    else
    {
        registerCustomTask([this, fd, dispatcher]() {
            if (m_spIOMultiplex)
                m_spIOMultiplex->registerReadEvent(fd, dispatcher);

            m_spWakeUpEventDispatcher->WakeUp();
            });
    }

}

void EventLoop::registerWriteEvent(SOCKET fd, EventDispatcher* dispatcher)
{
    if (isCallableInOwnerThread())
    {
        if (m_spIOMultiplex)
            m_spIOMultiplex->registerWriteEvent(fd, dispatcher);
    }
    else
    {
        registerCustomTask([this, fd, dispatcher]() {
            if (m_spIOMultiplex)
                m_spIOMultiplex->registerWriteEvent(fd, dispatcher);

            m_spWakeUpEventDispatcher->WakeUp();
            });
    }
}

void EventLoop::unregisterReadEvent(SOCKET fd, EventDispatcher* dispatcher)
{
    if (isCallableInOwnerThread())
    {
        if (m_spIOMultiplex)
            m_spIOMultiplex->unRegisterReadEvent(fd, dispatcher);
    }
    else
    {
        registerCustomTask([this, fd, dispatcher]() {
            if (m_spIOMultiplex)
                m_spIOMultiplex->unRegisterReadEvent(fd, dispatcher);

            m_spWakeUpEventDispatcher->WakeUp();
            });
    }
}

void EventLoop::unregisterWriteEvent(SOCKET fd, EventDispatcher* dispatcher)
{
    if (isCallableInOwnerThread())
    {
        if (m_spIOMultiplex)
            m_spIOMultiplex->unRegisterWriteEvent(fd, dispatcher);
    }
    else
    {
        registerCustomTask([this, fd, dispatcher]() {
            if (m_spIOMultiplex)
                m_spIOMultiplex->unRegisterWriteEvent(fd, dispatcher);

            m_spWakeUpEventDispatcher->WakeUp();
            });
    }
}

void EventLoop::registerCustomTask(CustomTask&& task)
{
    {
        std::lock_guard<std::mutex> scopedLock(m_taskMutex);
        m_customTasks.push_back(std::move(task));
    }

    
    m_spWakeUpEventDispatcher->WakeUp();
}

int64_t EventLoop::addTimer(int32_t intervalMs, 
    int64_t repeatCount, TimerTask task, TimerMode mode)
{
    auto spTimer = std::make_shared<Timer>(intervalMs,
        repeatCount, task, mode);

    addTimerInternal(spTimer);
    
    return spTimer->getId();
}

void EventLoop::removeTimer(int64_t timerId)
{
    removeTimerInternal(timerId);
}

bool EventLoop::createWakeUpSocket()
{
#ifdef _WIN32
    m_wakeUpSocket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (m_wakeUpSocket == INVALID_SOCKET)
    {
        LOG_ERROR("Failed to create Wakeup Sokcet, error : %d", GetSocketError());
        return false;
    }

    sockaddr_in bindAddr = { 0 };
    bindAddr.sin_family = AF_INET;
    bindAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    bindAddr.sin_port = 0;
       
    if (::bind(m_wakeUpSocket, (sockaddr*)&bindAddr, sizeof(bindAddr)) != 0)
    {
        LOG_ERROR("Failed to bind Wakeup Sokcet, error: %d", GetSocketError());
        closesocket(m_wakeUpSocket);
        m_wakeUpSocket = INVALID_SOCKET;
        return false;
    }

    int addrLen = sizeof(m_wakeUpAddr);
    if (getsockname(m_wakeUpSocket, (sockaddr*)&m_wakeUpAddr, &addrLen) != 0)
    {
        LOG_ERROR("Failed to get socket name, error: %d", GetSocketError());
        closesocket(m_wakeUpSocket);
        m_wakeUpSocket = INVALID_SOCKET;
        return false;
    }

    //非阻塞模式
    u_long mode = 1;
    if (ioctlsocket(m_wakeUpSocket, FIONBIO, &mode) != 0)
    {
        LOG_ERROR("Failed to set non-blocking mode, error: %d", GetSocketError());
        closesocket(m_wakeUpSocket);
        m_wakeUpSocket = INVALID_SOCKET;
        return false;
    }

    m_spWakeUpEventDispatcher = std::make_unique<WakeUpEventDispatcher>(m_wakeUpSocket);
    m_spWakeUpEventDispatcher->setWakeUpSocketAddr(m_wakeUpAddr);
#else

    m_wakeUpSocket = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    if (m_wakeUpSocket == INVALID_SOCKET) {
        LOG_ERROR("Failed to create eventfd, errno: %d", errno);
        return false;
    }
    m_spWakeUpEventDispatcher = std::make_unique<WakeUpEventDispatcher>(m_wakeUpSocket);
#endif

    LOG_INFO("create wakeupSocket(fd) successfully, socket(fd): %d", m_wakeUpSocket);

    return true;
}

void EventLoop::doOtherTasks()
{
    std::vector<CustomTask> tasks;
    {
        std::lock_guard<std::mutex> scopedLock(m_taskMutex);
        tasks.swap(m_customTasks);
    }

    for (auto& task : tasks)
    {
        task();
    }
}

void EventLoop::checkAndDoTimers()
{

    //获取当前系统时间
    int64_t nowMs = std::chrono::duration_cast<std::chrono::milliseconds>
        (std::chrono::system_clock::now().time_since_epoch()).count();

    //如果一个事件在他的事件逻辑内 调用一次添加定时器或者 移除定时器的逻辑，
    //就会导致 自己把自己 删除和其他的死锁问题
    m_isCheckingTimer = true;
    {
        for (auto iter = m_timers.begin(); iter != m_timers.end(); iter++)
        {
            while (nowMs >= (*iter)->nextTriggeredTimerMs())
            {
                
                (*iter)->doTimer((*iter)->getId(), nowMs);
            }
        }
    }
    m_isCheckingTimer = false;

    if (!m_pendingRemoveTimer.empty())
    {
        for (auto& timerId : m_pendingRemoveTimer)
            removeTimerInternal(timerId);
    }

    if (!m_pendingAddTimer.empty())
    {
        for (auto& spTimer : m_pendingAddTimer)
            addTimerInternal(spTimer);
    }
}

bool EventLoop::isCallableInOwnerThread() const
{
    return std::this_thread::get_id() == m_threadID;
}

void EventLoop::addTimerInternal(std::shared_ptr<Timer> spTimer)
{
    if (isCallableInOwnerThread())
    {
        if (m_isCheckingTimer)
        {
            m_pendingAddTimer.push_back(spTimer);
        }
        else
        {
            LOG_DEBUG("addTimerInternal, timerID:%d , threadID: %d", spTimer->getId(), std::this_thread::get_id());
            m_timers.push_back(spTimer);
        }
    }
    else
    {
        registerCustomTask(std::bind(&EventLoop::addTimerInternal, this, spTimer));
    }

}

void EventLoop::removeTimerInternal(int64_t timerId)
{
    if (isCallableInOwnerThread())
    {
        if(m_isCheckingTimer)
        {
            m_pendingRemoveTimer.push_back(timerId);
        }
        else
        {
            for (auto iter = m_timers.begin(); iter != m_timers.end(); iter++)
            {
                if ((*iter)->getId() == timerId)
                {
                    LOG_DEBUG("removeTimerInternal, timerId: %d, thread: %d", (*iter)->getId(), std::this_thread::get_id());
                    m_timers.erase(iter);
                    return;
                }
            }
        }
    }
    else
    {
        registerCustomTask(std::bind(&EventLoop::removeTimerInternal, this, timerId));
    }
}
