#include "HttpResponseTask.h"

HttpResponseTask::HttpResponseTask(std::string responseBody, ResponseFunc responseFunc)
    :m_responseBody(responseBody), m_responseFunc(responseFunc)
{
}

void HttpResponseTask::doTask()
{
    if (m_responseFunc)
    {
        m_responseFunc(m_responseBody);
    }
}
