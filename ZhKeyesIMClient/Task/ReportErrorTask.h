#ifndef TASK_REPORTERRORTASK_H_
#define TASK_REPORTERRORTASK_H_

#include <string>

#include "Task.h"
#include "TaskHandler.h"


class ReportErrorTask : public Task
{
public:
    ReportErrorTask( const std::string& errorMsg);

    virtual ~ReportErrorTask() = default;

    virtual void doTask() override;
private:

    std::string m_errorMsg;

};


#endif //!TASK_REPORTERRORTASK_H_