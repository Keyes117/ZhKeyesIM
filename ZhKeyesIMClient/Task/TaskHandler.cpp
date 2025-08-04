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

    m_spSendThread = std::make_unique<std::thread>(std::bind(&TaskHandler::sendThreadProc, this));
    m_spRecvThread = std::make_unique<std::thread>(std::bind(&TaskHandler::recvThreadProc, this));

    return true;
}

void TaskHandler::close()
{
    m_running = false;

    m_sendCV.notify_one();
    m_recvCV.notify_one();

    m_spRecvThread->join();
    m_spSendThread->join();

    LOG_INFO("TaskHandler::Close successfully ....");
}

void TaskHandler::registerSendTask(std::shared_ptr<Task>&& task)
{
    std::lock_guard<std::mutex> lock(m_sendMutex);
    m_sendTasks.emplace_back(std::move(task));
    m_sendCV.notify_one();
}


void TaskHandler::registerRecvTask(std::shared_ptr<Task>&& task)
{
    std::lock_guard<std::mutex> lock(m_recvMutex);
    m_recvTasks.emplace_back(std::move(task));
    m_recvCV.notify_one();
}

void TaskHandler::sendThreadProc()
{
    while(m_running)
    {
        std::unique_lock<std::mutex> guard(m_sendMutex);

        while(m_sendTasks.empty())
        {
            if(!m_running)
                return;
            
             //如果获得了互斥锁， 但是条件不合适的话， pthread_cond_wait会释放锁不往下执行
            //当发生变化后，条件合适，pthread_cond_wait将直接获得锁
            m_sendCV.wait(guard);
        }

        auto pTask = m_sendTasks.front();

        if(pTask == nullptr)
            continue;

        pTask->doTask();
        m_sendTasks.pop_front();
        pTask.reset();
    }
}

void TaskHandler::recvThreadProc()
{
    while(m_running)
    {
        std::unique_lock<std::mutex> guard(m_recvMutex);

        while(m_recvTasks.empty())
        {
            if(!m_running)
                return;
            
        
            m_recvCV.wait(guard);
        }

        auto pTask = m_recvTasks.front();

        if(pTask == nullptr)
            continue;

        pTask->doTask();
        m_recvTasks.pop_front();
        pTask.reset();
    }
}