
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

        auto pTask = m_netTasks.front();

        if (pTask == nullptr)
            continue;

        pTask->doTask();
        m_netTasks.pop_front();
        
        //pTask.reset();
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

        auto pTask = m_UITasks.front();

        if (pTask == nullptr)
            continue;

        pTask->doTask();
        m_UITasks.pop_front();
        //pTask.reset();
    }
}