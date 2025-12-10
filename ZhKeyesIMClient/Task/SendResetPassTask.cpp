#include "SendResetPassTask.h"

#include "TaskHandler.h"

SendResetPassTask::SendResetPassTask(std::shared_ptr<IMClient> spClient, 
    const std::string& user, const std::string& email, 
    const std::string& code, const std::string& newPassword) :
    m_spClient(spClient), m_user(user), m_email(email),
    m_code(code), m_password(newPassword)
{
}

void SendResetPassTask::doTask()
{
    m_spClient->requestResetPassword(m_user, m_email, m_password, m_code);
}
