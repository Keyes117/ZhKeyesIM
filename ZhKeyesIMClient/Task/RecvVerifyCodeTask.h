#ifndef TASK_RECVVERIFYCODETASK_H_
#define TASK_RECVVERIFYCODETASK_H_

#include "Task.h"
#include <string>

class RecvVerifyCodeTask : public Task
{
public:
    RecvVerifyCodeTask(bool success, const std::string& message, int errorCode = 0);

    virtual ~RecvVerifyCodeTask() = default;

    virtual void doTask() override;
private:

    bool        m_success;
    std::string m_message;
    int         m_errorCode;

};


#endif