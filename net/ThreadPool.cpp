#include "ThreadPool.h"

#include <functional>

static constexpr int32_t MAX_THREAD_NUM = 20;
static constexpr int32_t DEFAULT_THREAD_NUM = 1;

void ThreadPool::start(int32_t threadNum, IOMultiplexType type)
{
    if (threadNum <= 0 || threadNum > MAX_THREAD_NUM)
        threadNum = DEFAULT_THREAD_NUM;

    m_running = true;

    for (int32_t i = 0; i < threadNum; i++)
    {
        auto spEventLoop = std::make_shared<EventLoop>();
        spEventLoop->init(type);
        m_eventLoops.push_back(std::move(spEventLoop));
    }
    for (int32_t i = 0; i < threadNum; i++)
    {
        std::shared_ptr<std::thread> spThread = std::make_shared<std::thread>(std::bind(&ThreadPool::threadFunc, this, i));
        m_eventLoops[i]->setThreadID(spThread->get_id());
        m_threads.emplace_back(std::move(spThread));
    }


}

void ThreadPool::stop()
{
    m_running = false;

    int32_t threadNum = m_threads.size();
    for (int32_t i = 0; i < threadNum; i++)
    {
        m_eventLoops[i]->stop();
        m_threads[i]->join();
    }

}

std::shared_ptr<EventLoop> ThreadPool::getNextEventLoop()
{
    size_t index = m_lastEventLoopNo.fetch_add(1) % m_eventLoops.size();
    auto spEventLoop = m_eventLoops[index];

    return spEventLoop;
}

void ThreadPool::threadFunc(size_t index)
{
    while (m_running)
    {
        m_eventLoops[index]->run();
    }
}