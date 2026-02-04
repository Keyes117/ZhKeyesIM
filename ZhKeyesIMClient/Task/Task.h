/**
 * @desc:   任务基类，Task.h
 * @author: ZhKeyes
 * @date:   2025/8/4
 */
#ifndef ZHKEYESIMCLIENT_TASK_TASK_H_
#define ZHKEYESIMCLIENT_TASK_TASK_H_

#include <atomic>
#include <cstdint>
#include <memory>

#include <QObject>

class Task : public QObject, public std::enable_shared_from_this<Task>
{ 
    Q_OBJECT
public:
    class ConstructorKey {
    private:
        ConstructorKey() = default;
        friend class TaskBuilder;  // 只有 TaskBuilder 能构造这个 key
    };

    enum class TaskType
    {
        TASK_TYPE_UNKNOWN = 0,
        TASK_TYPE_REGISTER ,
        TASK_TYPE_VERIFYCODE,
        TASK_TYPE_LOGIN,
        TASK_TYPE_RESETPASS,
        TASK_TYPE_HTTPRESPONSE,
        TASK_TYPE_TCPCONNECT
    };

    using TaskId = uint64_t;

    virtual ~Task() = default;

    virtual void doTask() = 0;

    TaskId getTaskId() const { return m_taskId; }
    TaskType getTaskType() const { return m_taskType; }

signals:
    void taskFinished(Task::TaskId taskId);
    void taskFailed(const QString& errorMsg);
    void taskSuccess();

protected:
    Task(ConstructorKey, TaskId taskId,TaskType type)
        : QObject(), m_taskId(taskId),
        m_taskType(type)
    {}

    virtual void onTaskSuccess()
    {
        emit taskSuccess();
        emit taskFinished(m_taskId);
    }
    virtual void onTaskError(const std::string& errorMsg)
    {
        emit taskFailed(QString::fromStdString(errorMsg));
        emit taskFinished(m_taskId);
    }

private:
    TaskId    m_taskId;
    TaskType    m_taskType;
private:
    Task(const Task& rhs) = delete;
    Task& operator=(const Task& rhs) = delete;
    Task(Task&& rhs) = delete;
    Task& operator=(Task&& rhs) = delete;
};


 #endif