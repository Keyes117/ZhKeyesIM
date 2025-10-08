#ifndef NET_TIMER_H_
#define NET_TIMER_H_

#include "TimerService.h"
#include <cstdint>

class Timer
{
public:
    Timer(int32_t intervalMs, int64_t repeatCount,
        TimerTask task, TimerMode mode = TimerMode::TimerMode_FixedInterval);
    virtual ~Timer() = default;

    int64_t getId();

    static int64_t generateTimerId();

    int64_t nextTriggeredTimerMs();

    void doTimer(int64_t timerId, int64_t nowMs);

private:
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

    Timer(Timer&&) = delete;
    Timer& operator=(Timer&&) noexcept = delete;


private:
    int64_t     m_id;

    int32_t     m_intervalMs;

    /**
     * @brief -1 标识一直重复, 0 标识不重复, >0 则是重复次数
     */
    int64_t     m_repeatedCount;

    TimerTask   m_task;
    TimerMode   m_mode;

    int64_t     m_nextTriggeredTimeMs;
};

#endif
