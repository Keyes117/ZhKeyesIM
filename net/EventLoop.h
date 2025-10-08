#ifndef NET_EVENTLOOP_H_
#define NET_EVENTLOOP_H_

#include <memory>
#include <mutex>
#include <thread>

#include "common.h"
#include "Epoll.h"
#include "net_export.h"
#include "Poll.h"
#include "Select.h"
#include "Timer.h"
#include "TimerService.h"
#include "WakeUpEventDispatcher.h"

using CustomTask = std::function<void()>;

enum class IOMultiplexType
{
    Select = 0,
    Poll,
    Epoll
};

class NET_API EventLoop : public TimerService, public std::enable_shared_from_this<EventLoop>
{
public:
    EventLoop(bool isBaseLoop = false);
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

    void registerCustomTask(CustomTask&& task);

    virtual int64_t addTimer(int32_t intervalMs, int64_t repeatCount, TimerTask task,
                TimerMode mode = TimerMode::TimerMode_FixedInterval) override;

    virtual void removeTimer(int64_t timerId) override;

private:
    //唤醒事件fd
    bool createWakeUpSocket();

    void doOtherTasks();

    void checkAndDoTimers();

    bool isCallableInOwnerThread() const;

    void addTimerInternal(std::shared_ptr<Timer> spTimer);
    void removeTimerInternal(int64_t timerId);

private:
    bool                                    m_isBaseLoop;                       //主事件循环标志
    bool                                    m_running = false;                  //主事件循环运行标志
    bool                                    m_isCheckingTimer;

    SOCKET                                  m_wakeUpSocket;                     //唤醒Socket
#ifdef _WIN32
    sockaddr_in                             m_wakeUpAddr;
#endif
    std::unique_ptr<IOMultiplex>            m_spIOMultiplex;                    //当前IO复用模型
    std::thread::id                         m_threadID;                         //主线程Id

    std::unique_ptr<WakeUpEventDispatcher>  m_spWakeUpEventDispatcher;          //唤醒事件处理

    std::vector<CustomTask>                 m_customTasks;                      //自定义任务列表
    std::mutex                              m_taskMutex;                        

    std::vector<std::shared_ptr<Timer>>     m_timers;                           //定时器任务列表
    std::vector<std::shared_ptr<Timer>>     m_pendingAddTimer;                  //待添加的定时器任务列表   
    std::vector<int64_t>                    m_pendingRemoveTimer;               //待删除的定时器任务列表
    std::mutex                              m_timerMutex;

private:
    EventLoop(const EventLoop&) = delete;
    EventLoop& operator=(const EventLoop&) = delete;
    EventLoop(EventLoop&&) = delete;
    EventLoop& operator=(EventLoop&&) = delete;
};

#endif

