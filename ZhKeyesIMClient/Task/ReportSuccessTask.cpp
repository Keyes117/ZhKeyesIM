#include "ReportSuccessTask.h"

ReportSuccessTask::ReportSuccessTask(const std::string& msg):
    m_msg(msg)
{
}

void ReportSuccessTask::doTask()
{
    emit TaskHandler::getInstance().reportErrorMsg(QString::fromStdString(m_msg));
}
