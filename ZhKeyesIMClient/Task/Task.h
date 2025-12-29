/**
 * @desc:   任务基类，Task.h
 * @author: ZhKeyes
 * @date:   2025/8/4
 */
#ifndef ZHKEYESIMCLIENT_TASK_TASK_H_
#define ZHKEYESIMCLIENT_TASK_TASK_H_

#include <memory>

class Task : public std::enable_shared_from_this<Task>
{
public:
    Task() = default;
    virtual ~Task() = default;

    virtual void doTask() = 0;

private:
    Task(const Task& rhs) = delete;
    Task& operator=(const Task& rhs) = delete;
    Task(Task&& rhs) = delete;
    Task& operator=(Task&& rhs) = delete;
};


 #endif