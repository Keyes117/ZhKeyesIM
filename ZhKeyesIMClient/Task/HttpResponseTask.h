#ifndef ZHKEYESIMCLIENT_TASK_HTTPRESPONSETASK_H_
#define ZHKEYESIMCLIENT_TASK_HTTPRESPONSETASK_H_

#include <functional>
#include <string>
#include <Task/Task.h>

class TaskBuilder;

class HttpResponseTask : public Task
{
    friend class TaskBuilder;
    Q_OBJECT
public:
    using ResponseFunc = std::function<void(const std::string& responseBody)>;
     
    HttpResponseTask(
        Task::ConstructorKey key,
        Task::TaskId id,
        std::string responseBody, ResponseFunc responseFunc);
    ~HttpResponseTask() = default;

    void doTask() override;
protected:


private:
    std::string m_responseBody;
    ResponseFunc m_responseFunc;
};


#endif