#ifndef UTIL_WORKTHREADPOOL_H_
#define UTIL_WORKTHREADPOOL_H_

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

#include "util_export.h"

/**
 * @brief 通用工作线程池
 * 
 * 用于执行阻塞任务（如数据库操作、文件I/O等）
 * 不会阻塞EventLoop线程
 */
class Util_API WorkThreadPool final
{
public:
    /**
     * @brief 构造函数
     * @param threadNum 线程池中的线程数量，默认为4
     * @param maxQueueSize 最大任务队列大小，0表示无限制，默认为1000
     */
    explicit WorkThreadPool(size_t threadNum = 4, size_t maxQueueSize = 1000);
    
    /**
     * @brief 析构函数，会等待所有任务完成
     */
    ~WorkThreadPool();

    /**
     * @brief 启动线程池
     */
    bool start();

    /**
     * @brief 停止线程池（等待所有任务完成）
     */
    void stop();

    /**
     * @brief 立即停止线程池（不等待任务完成）
     */
    void stopNow();

    /**
     * @brief 提交一个任务到线程池
     * @param task 要执行的任务
     * @return 如果成功提交返回true，如果队列已满返回false
     */
    bool enqueue(std::function<void()>&& task);

    /**
     * @brief 提交一个任务并获取future
     * @tparam F 函数类型
     * @tparam Args 参数类型
     * @param f 要执行的函数
     * @param args 函数参数
     * @return future对象，可用于获取返回值或等待完成
     */
    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args) 
        -> std::future<typename std::invoke_result<F, Args...>::type>;

    /**
     * @brief 获取线程池中的线程数量
     */
    size_t getThreadCount() const { return m_threadNum; }

    /**
     * @brief 获取当前队列中等待的任务数量
     */
    size_t getQueueSize() const;

    /**
     * @brief 获取正在执行的任务数量
     */
    size_t getActiveTaskCount() const { return m_activeTasks.load(); }

    /**
     * @brief 获取已完成的任务总数
     */
    size_t getCompletedTaskCount() const { return m_completedTasks.load(); }

    /**
     * @brief 检查线程池是否正在运行
     */
    bool isRunning() const { return m_running.load(); }

private:
    void workerThread();

private:
    size_t                              m_threadNum;        // 线程数量
    size_t                              m_maxQueueSize;     // 最大队列大小
    std::vector<std::thread>            m_workers;          // 工作线程
    std::queue<std::function<void()>>   m_tasks;            // 任务队列
    
    mutable std::mutex                  m_queueMutex;       // 队列互斥锁
    std::condition_variable             m_condition;        // 条件变量
    
    std::atomic<bool>                   m_running;          // 运行状态
    std::atomic<bool>                   m_stop;             // 停止标志
    std::atomic<bool>                   m_stopNow;          // 立即停止标志
    std::atomic<size_t>                 m_activeTasks;      // 活跃任务数
    std::atomic<size_t>                 m_completedTasks;   // 已完成任务数

private:
    WorkThreadPool(const WorkThreadPool&) = delete;
    WorkThreadPool& operator=(const WorkThreadPool&) = delete;
    WorkThreadPool(WorkThreadPool&&) = delete;
    WorkThreadPool& operator=(WorkThreadPool&&) = delete;
};

// 模板函数实现
template<typename F, typename... Args>
auto WorkThreadPool::submit(F&& f, Args&&... args) 
    -> std::future<typename std::invoke_result<F, Args...>::type>
{
    using ReturnType = typename std::invoke_result<F, Args...>::type;
    
    auto task = std::make_shared<std::packaged_task<ReturnType()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    
    std::future<ReturnType> result = task->get_future();
    
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        
        if (m_stop.load() || m_stopNow.load()) {
            throw std::runtime_error("WorkThreadPool is stopped, cannot submit task");
        }
        
        if (m_maxQueueSize > 0 && m_tasks.size() >= m_maxQueueSize) {
            throw std::runtime_error("WorkThreadPool task queue is full");
        }
        
        m_tasks.emplace([task]() { (*task)(); });
    }
    
    m_condition.notify_one();
    return result;
}

#endif // NET_WORKTHREADPOOL_H_