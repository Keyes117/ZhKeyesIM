
#ifndef NET_THREADPOOL_H_
#define NET_THREADPOOL_H_

#include <atomic>
#include <memory>
#include <stdint.h>
#include <thread>
#include <vector>

#include "EventLoop.h"


class ThreadPool final
{
public:
    ThreadPool() = default;
    ~ThreadPool() = default;

    void start(int32_t threadNum = 1, IOMultiplexType type = IOMultiplexType::Epoll);
    void stop();

    std::shared_ptr<EventLoop> getNextEventLoop();

private:
    void threadFunc(size_t index);

private:
    bool                        m_running{ false };
    std::atomic<size_t>         m_lastEventLoopNo{ 0 };

    std::vector<std::shared_ptr<EventLoop>> m_eventLoops;
    std::vector<std::shared_ptr<std::thread>> m_threads;

private:
    ThreadPool(const ThreadPool& rhs) = delete;
    ThreadPool& operator=(const ThreadPool& rhs) = delete;
    ThreadPool(ThreadPool&& rhs) = delete;
    ThreadPool& operator=(ThreadPool&& rhs) = delete;



};

#endif

