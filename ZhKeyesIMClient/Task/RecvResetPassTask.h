#ifndef TASK_RECVRESETPASSTASK_H_
#define TASK_RECVRESETPASSTASK_H_

#include <memory>
#include <string>

#include "IMClient.h"
#include "Task.h"

class RecvResetPassTask : public Task
{
public:
    RecvResetPassTask(bool success, const std::string& message, int errorCode = 0);

    virtual ~RecvResetPassTask() = default;

    virtual void doTask() override;

private:

    bool        m_success;
    std::string m_message;
    int         m_errorCode;
};


#endif  //!TASK_RECVRESETPASSTASK_H_