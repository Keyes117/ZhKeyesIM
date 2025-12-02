#include "RecvVerifyCodeTask.h"

#include "TaskHandler.h"

RecvVerifyCodeTask::RecvVerifyCodeTask(bool success, 
    const std::string& message, int errorCode):
    m_success(success),
    m_message(message),
    m_errorCode(errorCode)
{
}

void RecvVerifyCodeTask::doTask()
{
    emit TaskHandler::getInstance().verifyCodeRecevied(
        m_success, QString::fromStdString(m_message), m_errorCode
    );
}
