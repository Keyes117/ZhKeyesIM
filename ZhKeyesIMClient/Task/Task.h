/**
 * @desc:   任务基类，Task.h
 * @author: ZhKeyes
 * @date:   2025/8/4
 */
#ifndef ZHKEYESIMCLIENT_TASK_TASK_H_
#define ZHKEYESIMCLIENT_TASK_TASK_H_

class Task
{
public:
    Task();
    virtual ~Task() = 0;

    virtual void doTask() = 0;
};


 #endif