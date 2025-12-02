#ifndef TASK_GETVERIFYCODETASK_H_
#define TASK_GETVERIFYCODETASK_H_


#include <string>

#include "Task.h"
#include "IMClient.h"



class GetVerifyCodeTask : public Task
{
public:
    GetVerifyCodeTask(std::shared_ptr<IMClient> spClient, const std::string& email);

    virtual ~GetVerifyCodeTask() = default;

    virtual void doTask() override;
private:

    std::shared_ptr<IMClient> m_spClient;
    std::string m_email;

};

#endif