#include "HttpResponseTask.h"

HttpResponseTask::HttpResponseTask(std::string responseBody, ResponseFunc responseFunc)
    :m_responseBody(std::move(responseBody)), 
    m_responseFunc(std::move(responseFunc))
{
}

void HttpResponseTask::doTask()
{
    if (m_responseFunc)
    {
        m_responseFunc(m_responseBody);
    }
}
