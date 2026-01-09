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

#include <QObject>

#include "log/Logger.h"
#include "Task/Task.h"

class TaskHandler final : public QObject
{
    Q_OBJECT
public:
    static TaskHandler& getInstance();

    bool init();
    void close();

    //SendTask : ClientUI -> ClientNetWork
    //RecvTask : ClientNetWork -> ClientUI
    void registerNetTask(std::shared_ptr<Task>&& task);
    void registerUITask(std::shared_ptr<Task>&& task);

signals:
    void verifyCodeRecevied(bool isSuccess,const QString& message, int errorCode);
    void registerRecevied(bool isSuccess, const QString& message, int errorCode);
    void resetPasswordRecevied(bool isSuccess, const QString& message, int errorCode);
    void userLoginRecevied(bool isSuccess, const QString& message, int errorCode);
    
    void reportErrorMsg(QString errorMsg);
    void reportSuccessMsg(QString successMsg);
    
private:
    TaskHandler() = default;
    ~TaskHandler() = default;

    void sendThreadProc();
    void recvThreadProc();


private:
    std::mutex                          m_netMutex;
    std::mutex                          m_UIMutex;

    std::condition_variable             m_netCV;
    std::condition_variable             m_UICV;

    std::list<std::shared_ptr<Task>>    m_netTasks;
    std::list<std::shared_ptr<Task>>    m_UITasks;

    std::unique_ptr<std::thread>        m_spNetTaskThread;
    std::unique_ptr<std::thread>        m_spUITaskThread;

    std::atomic<bool>                   m_running;

};


#endif