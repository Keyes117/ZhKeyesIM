#ifndef TASK_SENDREGISTERTASK_H_
#define TASK_SENDREGISTERTASK_H_

#include <string>

#include "Task.h"
#include "IMClient.h"

class SendRegisterTask : public Task
{
public:
    SendRegisterTask(std::shared_ptr<IMClient> spClient,
        const std::string& username,
        const std::string& email,
        const std::string& password,
        const std::string& code
        );

    virtual ~SendRegisterTask() = default;

    virtual void doTask() override;
private:
    std::shared_ptr<IMClient> m_spClient;

    std::string m_username;
    std::string m_email;
    std::string m_password;
    std::string m_code;

};



#endif