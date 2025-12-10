#ifndef TASK_RECVREGISTERTASK_H_
#define TASK_RECVREGISTERTASK_H_

#include <memory>
#include <string>

#include "IMClient.h"
#include "Task.h"

class RecvRegisterTask : public Task
{
public:
    RecvRegisterTask(bool success, const std::string& message, int errorCode = 0);

    virtual ~RecvRegisterTask() = default;

    virtual void doTask() override;

private:

    bool        m_success;
    std::string m_message;
    int         m_errorCode;
};


#endif