#ifndef ZHKEYESIMCLIENT_TASK_HTTPRESPONSETASK_H_
#define ZHKEYESIMCLIENT_TASK_HTTPRESPONSETASK_H_

#include <functional>
#include <string>
#include <Task/Task.h>

class HttpResponseTask : public Task
{

public:
    using ResponseFunc = std::function<void(const std::string& responseBody)>;
     
    ~HttpResponseTask() = default;

    void doTask() override;
protected:
    HttpResponseTask(Task::TaskId id,
        std::string responseBody, ResponseFunc responseFunc);

private:
    std::string m_responseBody;
    ResponseFunc m_responseFunc;
};


#endif