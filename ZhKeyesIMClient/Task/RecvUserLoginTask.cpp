#include "RecvUserLoginTask.h"

#include "TaskHandler.h"

RecvUserLoginTask::RecvUserLoginTask(bool success, const std::string& message, 
    int errorCode) : 
    m_success(success), m_message(message), m_errorCode(errorCode)
{
}

void RecvUserLoginTask::doTask()
{
    emit TaskHandler::getInstance().userLoginRecevied(m_success, 
        QString::fromStdString(m_message),m_errorCode);
}
