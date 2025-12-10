#include "RecvRegisterTask.h"

#include "TaskHandler.h"

RecvRegisterTask::RecvRegisterTask(bool success, const std::string& message,
    int errorCode) :
    m_success(success), m_message(message), m_errorCode(errorCode)
{
}

void RecvRegisterTask::doTask()
{
    QString message = QString::fromStdString(m_message);
    emit TaskHandler::getInstance().registerRecevied(m_success, message, m_errorCode);
}
