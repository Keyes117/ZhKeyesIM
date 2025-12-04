#ifndef TASK_REPORTSUCCESSTASK_H_
#define TASK_REPORTSUCCESSTASK_H_

#include <string>

#include "Task.h"
#include "TaskHandler.h"


class ReportSuccessTask : public Task
{
public:
    ReportSuccessTask(const std::string& errorMsg);

    virtual ~ReportSuccessTask() = default;

    virtual void doTask() override;
private:

    std::string m_msg;

};


#endif //!TASK_REPORTSUCCESSTASK_H_