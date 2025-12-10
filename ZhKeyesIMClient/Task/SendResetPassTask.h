#ifndef TASK_SENDRESETPASSTASK_H_
#define TASK_SENDRESETPASSTASK_H_

#include <memory>
#include <string>

#include "IMClient.h"
#include "Task.h"

class SendResetPassTask : public Task
{
public:
    SendResetPassTask(std::shared_ptr<IMClient> spClient, const std::string& user,
        const std::string& email, const std::string& code, const std::string& newPassword);

    ~SendResetPassTask() = default;

    virtual void doTask() override;

private:
    std::shared_ptr<IMClient> m_spClient;
    std::string m_user;
    std::string m_email;
    std::string m_code;
    std::string m_password;
};

#endif // !TASK_SENDRESETPASSTASK_H_