#ifndef TASK_SENDUSERLOGINTASK_H_
#define TASK_SENDUSERLOGINTASK_H_

#include <memory>
#include <string>

#include "IMClient.h"
#include "Task.h"

class SendUserLoginTask : public Task
{
public:
    SendUserLoginTask(std::shared_ptr<IMClient> spClient, const std::string& user,
            const std::string& newPassword);

    ~SendUserLoginTask() = default;

    virtual void doTask() override;

private:
    std::shared_ptr<IMClient> m_spClient;
    std::string m_user;
    std::string m_email;
    std::string m_code;
    std::string m_password;
};

#endif // !TASK_SENDUSERLOGINTASK_H_