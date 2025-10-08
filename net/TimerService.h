#ifndef NET_TIMERSERVICE_H_
#define NET_TIMERSERVICE_H_

#include <functional>
#include <cstdint>

enum class TimerMode
{
    TimerMode_FixedInterval = 0,
    TimerMode_FixedRate 
};

using TimerTask = std::function<void(int64_t timerID)>;

class TimerService
{
public:
    virtual ~TimerService() = default;

    virtual int64_t addTimer(int32_t intervalMs, int64_t repeatCount,TimerTask task, TimerMode mode ) = 0;
    virtual void removeTimer(int64_t timerID) = 0;
};

#endif
