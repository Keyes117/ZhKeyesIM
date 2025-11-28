#include "GetVerifyCodeTask.h"

GetVerifyCodeTask::GetVerifyCodeTask(std::shared_ptr<IMClient> spClient, 
    const std::string& email):
    m_spClient(spClient),
    m_strEmail(email)
{

}

void GetVerifyCodeTask::doTask()
{

}
