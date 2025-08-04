/**
 * @desc:   任务处理类，TaskHandler.h
 * @author: ZhKeyes
 * @date:   2025/8/4
 */

 #ifndef ZHKEYESIMCLIENT_TASK_TASKHANDLER_H_
 #define ZHKEYESIMCLIENT_TASK_TASKHANDLER_H_

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <list>
#include <memory>
#include <mutex>

#include "Logger.h"

#include "Task.h"

class TaskHandler final
{
public:
    static TaskHandler& getInstance();              

    bool init();
    void close();

    //SendTask : ClientUI -> ClientNetWork
    //RecvTask : ClientNetWork -> ClientUI
    void registerSendTask(std::shared_ptr<Task>&& task);
    void registerRecvTask(std::shared_ptr<Task>&& task);

private:
    TaskHandler();
    ~TaskHandler() = default;

    void sendThreadProc();
    void recvThreadProc();


private:
    std::mutex                          m_sendMutex;
    std::mutex                          m_recvMutex;
    
    std::condition_variable             m_sendCV;   
    std::condition_variable             m_recvCV;

    std::list<std::shared_ptr<Task>>    m_sendTasks;
    std::list<std::shared_ptr<Task>>    m_recvTasks;

    std::unique_ptr<std::thread>        m_spSendThread;
    std::unique_ptr<std::thread>        m_spRecvThread;

    std::atomic<bool>                   m_running;

};


 #endif