#include "GetVerifyCodeTask.h"

GetVerifyCodeTask::GetVerifyCodeTask(std::shared_ptr<IMClient> spClient, 
    const std::string& email):
    m_spClient(spClient),
    m_email(email)
{

}

void GetVerifyCodeTask::doTask()
{
    m_spClient->requestVerificationCode(m_email);
}
