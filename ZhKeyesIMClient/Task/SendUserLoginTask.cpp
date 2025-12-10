#include "SendUserLoginTask.h"

SendUserLoginTask::SendUserLoginTask(std::shared_ptr<IMClient> spClient, 
    const std::string& user, const std::string& newPassword) :
    m_spClient(spClient), m_user(user), m_password(newPassword)
{
}

void SendUserLoginTask::doTask()
{
    m_spClient->requestUserLogin(m_user, m_password);
}
