#ifndef TASK_RECVUSERLOGINTASK_H_
#define TASK_RECVUSERLOGINTASK_H_

#include <memory>
#include <string>

#include "IMClient.h"
#include "Task.h"

class RecvUserLoginTask : public Task
{
public:
    RecvUserLoginTask(bool success, const std::string& message, int errorCode = 0);

    virtual ~RecvUserLoginTask() = default;

    virtual void doTask() override;

private:

    bool        m_success;
    std::string m_message;
    int         m_errorCode;
};

#endif // !TASK_RECVUSERLOGINTASK_H_