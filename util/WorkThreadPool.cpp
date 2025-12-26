#include "WorkThreadPool.h"

#include "Logger.h"

WorkThreadPool::WorkThreadPool(size_t threadNum, size_t maxQueueSize)
    : m_threadNum(threadNum)
    , m_maxQueueSize(maxQueueSize)
    , m_running(false)
    , m_stop(false)
    , m_stopNow(false)
    , m_activeTasks(0)
    , m_completedTasks(0)
{
    if (threadNum == 0) {
        throw std::invalid_argument("WorkThreadPool: threadNum must be greater than 0");
    }
    
    if (threadNum > 256) {
        LOG_WARN("WorkThreadPool: threadNum=%zu is very large, consider using a smaller value", 
                 threadNum);
    }
}

WorkThreadPool::~WorkThreadPool()
{
    stop();
}

bool WorkThreadPool::start()
{
    if (m_running.load()) {
        LOG_WARN("WorkThreadPool: already running");
        return false;
    }
    
    m_running.store(true);
    m_stop.store(false);
    m_stopNow.store(false);
    
    m_workers.reserve(m_threadNum);
    
    try {
        for (size_t i = 0; i < m_threadNum; ++i) {
            m_workers.emplace_back(&WorkThreadPool::workerThread, this);
        }
        
        return true;
        LOG_INFO("WorkThreadPool started with %zu threads", m_threadNum);
    }
    catch (const std::exception& e) {
        LOG_ERROR("WorkThreadPool: failed to start threads: %s", e.what());
        m_running.store(false);
        return false;
   
    }
}

void WorkThreadPool::stop()
{
    if (!m_running.load()) {
        return;
    }
    
    LOG_INFO("WorkThreadPool: stopping...");
    
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_stop.store(true);
    }
    
    m_condition.notify_all();
    
    for (std::thread& worker : m_workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    
    m_workers.clear();
    m_running.store(false);
    
    LOG_INFO("WorkThreadPool: stopped. Completed %zu tasks", 
             m_completedTasks.load());
}

void WorkThreadPool::stopNow()
{
    if (!m_running.load()) {
        return;
    }
    
    LOG_WARN("WorkThreadPool: stopping immediately, %zu tasks may be discarded", 
             getQueueSize());
    
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_stopNow.store(true);
        m_stop.store(true);
        
        // 清空任务队列
        std::queue<std::function<void()>> empty;
        std::swap(m_tasks, empty);
    }
    
    m_condition.notify_all();
    
    for (std::thread& worker : m_workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    
    m_workers.clear();
    m_running.store(false);
    
    LOG_INFO("WorkThreadPool: stopped immediately");
}

bool WorkThreadPool::enqueue(std::function<void()>&& task)
{
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        
        if (m_stop.load() || m_stopNow.load()) {
            LOG_WARN("WorkThreadPool: cannot enqueue task, pool is stopping");
            return false;
        }
        
        // 检查队列是否已满
        if (m_maxQueueSize > 0 && m_tasks.size() >= m_maxQueueSize) {
            LOG_WARN("WorkThreadPool: task queue is full (size=%zu)", m_tasks.size());
            return false;
        }        
        m_tasks.emplace(std::move(task));
    }
    
    m_condition.notify_one();
    return true;
}

size_t WorkThreadPool::getQueueSize() const
{
    std::unique_lock<std::mutex> lock(m_queueMutex);
    return m_tasks.size();
}

void WorkThreadPool::workerThread()
{
    LOG_DEBUG("WorkThreadPool: worker thread %llu started", 
              std::this_thread::get_id());
    
    while (true) {
        std::function<void()> task;
        
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            
            // 等待任务或停止信号
            m_condition.wait(lock, [this] {
                return m_stop.load() || !m_tasks.empty();
            });
            
            // 检查是否需要停止
            if (m_stopNow.load()) {
                break;
            }
            
            if (m_stop.load() && m_tasks.empty()) {
                break;
            }
            
            // 取出任务
            if (!m_tasks.empty()) {
                task = std::move(m_tasks.front());
                m_tasks.pop();
            }
        }
        
        // 执行任务
        if (task) {
            m_activeTasks.fetch_add(1);
            
            try {
                task();
            }
            catch (const std::exception& e) {
                LOG_ERROR("WorkThreadPool: task threw exception: %s", e.what());
            }
            catch (...) {
                LOG_ERROR("WorkThreadPool: task threw unknown exception");
            }
            
            m_activeTasks.fetch_sub(1);
            m_completedTasks.fetch_add(1);
        }
    }
    
    LOG_DEBUG("WorkThreadPool: worker thread %llu stopped", 
              std::this_thread::get_id());
}