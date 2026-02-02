
//#include "GateServer.h"
#include "TaskHandler.h"

#include <functional>

TaskHandler& TaskHandler::getInstance()
{
    static TaskHandler handler;
    return handler;
}

bool TaskHandler::init()
{
    m_running = true;

    m_spNetTaskThread = std::make_unique<std::thread>(std::bind(&TaskHandler::sendThreadProc, this));
    m_spUITaskThread = std::make_unique<std::thread>(std::bind(&TaskHandler::recvThreadProc, this));

    return true;
}


void TaskHandler::close()
{
    m_running = false;

    m_netCV.notify_one();
    m_UICV.notify_one();

    if(m_spNetTaskThread->joinable())
        m_spNetTaskThread->join();

    if(m_spUITaskThread->joinable())
        m_spUITaskThread->join();


    LOG_INFO("TaskHandler::Close successfully ....");
}

void TaskHandler::registerNetTask(std::shared_ptr<Task>&& task)
{
    std::lock_guard<std::mutex> lock(m_netMutex);
    m_netTasks.emplace_back(std::move(task));
    m_netCV.notify_one();
}


void TaskHandler::registerUITask(std::shared_ptr<Task>&& task)
{
    std::lock_guard<std::mutex> lock(m_UIMutex);
    m_UITasks.emplace_back(std::move(task));
    m_UICV.notify_one();
}

void TaskHandler::sendThreadProc()
{
    while (m_running)
    {
        std::unique_lock<std::mutex> guard(m_netMutex);

        while (m_netTasks.empty())
        {
            if (!m_running)
                return;

            //如果获得了互斥锁， 但是条件不合适的话， pthread_cond_wait会释放锁不往下执行
           //当发生变化后，条件合适，pthread_cond_wait将直接获得锁
            m_netCV.wait(guard);
        }

        auto spTask = m_netTasks.front();

        if (spTask == nullptr)
            continue;

        if (!isTaskRunning(spTask->getTaskId()))
        {
            connect(spTask.get(), Task::taskFinished, this, TaskHandler::onTaskFinished);
            spTask->doTask();
            addRunningTask(std::move(spTask));
        }
        m_netTasks.pop_front();
        
    }
}

void TaskHandler::recvThreadProc()
{
    while (m_running)
    {
        std::unique_lock<std::mutex> guard(m_UIMutex);

        while (m_UITasks.empty())
        {
            if (!m_running)
                return;


            m_UICV.wait(guard);
        }

        auto spTask = m_netTasks.front();

        if (spTask == nullptr)
            continue;

        if (!isTaskRunning(spTask->getTaskId()))
        {
            connect(spTask.get(), Task::taskFinished, this, TaskHandler::onTaskFinished);
            spTask->doTask();
            addRunningTask(std::move(spTask));
        }
        m_netTasks.pop_front();
    }
}

void TaskHandler::addRunningTask(std::shared_ptr<Task>&& task)
{
    std::lock_guard<std::mutex> lock(m_runningTasksMutex);
    auto taskId = task->getTaskId();

    m_runningTask[taskId] = std::move(task);
}

void TaskHandler::removeRunningTask(Task::TaskId taskId)
{
    std::lock_guard<std::mutex> lock(m_runningTasksMutex);
    auto iter = m_runningTask.find(taskId);
    if (iter != m_runningTask.end())
    {
        m_runningTask.erase(iter);
    }

}

bool TaskHandler::isTaskRunning(Task::TaskId taskId)
{
    return m_runningTask.find(taskId) != m_runningTask.end();
}


void TaskHandler::onTaskFinished(Task::TaskId taskId)
{
    removeRunningTask(taskId);
}