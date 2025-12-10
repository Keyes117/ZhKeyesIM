#include "RecvResetPassTask.h"

#include "TaskHandler.h"

RecvResetPassTask::RecvResetPassTask(bool success, const std::string& message, 
    int errorCode):
    m_success(success),m_message(message),m_errorCode(errorCode)
{

}

void RecvResetPassTask::doTask()
{
    QString message = QString::fromStdString(m_message);
    emit TaskHandler::getInstance().resetPasswordRecevied(m_success,message,m_errorCode);
}
