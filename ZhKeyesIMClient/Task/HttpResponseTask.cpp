#include "HttpResponseTask.h"

HttpResponseTask::HttpResponseTask(
    Task::ConstructorKey key,
    Task::TaskId id,
    std::string responseBody, ResponseFunc responseFunc)
    :Task(key,id,TaskType::TASK_TYPE_HTTPRESPONSE),
    m_responseBody(std::move(responseBody)), 
    m_responseFunc(std::move(responseFunc))
{
}

void HttpResponseTask::doTask()
{
    if (m_responseFunc)
    {
        m_responseFunc(m_responseBody);
    }

    onTaskSuccess();
}
