#include "SendRegisterTask.h"

SendRegisterTask::SendRegisterTask(std::shared_ptr<IMClient> spClient,
    const std::string& username,
    const std::string& email,
    const std::string& password,
    const std::string& code) :
    m_spClient(spClient),
    m_username(username),
    m_email(email),
    m_password(password),
    m_code(code)
{
}

void SendRegisterTask::doTask()
{
    m_spClient->requestRegister(m_username, m_email, m_password, m_code);
}
