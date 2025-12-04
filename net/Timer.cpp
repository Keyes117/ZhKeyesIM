#include "Timer.h"

#include <atomic>
#include <chrono>

Timer::Timer(int32_t intervalMs, int64_t repeatCount, 
    TimerTask task, TimerMode mode):
    m_intervalMs(intervalMs),
    m_repeatedCount(repeatCount),
    m_task(task),
    m_mode(mode)
{
    m_id = Timer::generateTimerId();

    int64_t nowMs = std::chrono::duration_cast<std::chrono::milliseconds>
        (std::chrono::system_clock::now().time_since_epoch()).count();

    m_nextTriggeredTimeMs = nowMs + m_intervalMs;
}

int64_t Timer::getId()
{
    return m_id;
}

int64_t Timer::generateTimerId()
{
    static std::atomic<int64_t> baseID{ 0 };
    baseID++;

    return baseID;
}

int64_t Timer::nextTriggeredTimerMs()
{
    return m_nextTriggeredTimeMs;
}

void Timer::doTimer(int64_t timerId, int64_t nowMs)
{

    if (m_repeatedCount > 0)
        m_repeatedCount--;
    else if (m_repeatedCount == 0)
        return;

    if (m_repeatedCount > 0 || m_repeatedCount == -1)
    {
        if (m_mode == TimerMode::TimerMode_FixedInterval)
        {
            m_nextTriggeredTimeMs += m_intervalMs;
        }
        else if (m_mode == TimerMode::TimerMode_FixedRate)
        {
            m_nextTriggeredTimeMs += nowMs + m_intervalMs;
        }
    }

    m_task(timerId);
}
