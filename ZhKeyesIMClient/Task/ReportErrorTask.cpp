#include "ReportErrorTask.h"

#include "TaskHandler.h"

ReportErrorTask::ReportErrorTask(const std::string& errorMsg) :
    m_errorMsg(errorMsg)
{
}

void ReportErrorTask::doTask()
{
    emit TaskHandler::getInstance().reportErrorMsg(QString::fromStdString(m_errorMsg));
}
