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

class TaskBuilder;
class Task : public QObject, public std::enable_shared_from_this<Task>
{ 
    friend class TaskBuilder;

    Q_OBJECT
public:
    enum class TaskType
    {
        TASK_TYPE_UNKNOWN = 0,
        TASK_TYPE_REGISTER ,
        TASK_TYPE_VERIFYCODE,
        TASK_TYPE_LOGIN,
        TASK_TYPE_RESETPASS
    };

    using TaskId = uint64_t;

    virtual ~Task() = default;

    virtual void doTask() = 0;

    TaskId getTaskId() const { return m_taskId; }

signals:
    void taskFinished();

protected:
    Task(TaskId taskId,TaskType type)
        : QObject(), m_taskId(taskId),
        m_taskType(type)
    {}

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