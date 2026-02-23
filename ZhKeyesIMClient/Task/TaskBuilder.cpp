// TaskBuilder.cpp
#include "TaskBuilder.h"
#include "Task/RegisterTask.h"
#include "Task/UserLoginTask.h"
#include "Task/VerifyCodeTask.h"
#include "Task/ResetPasswordTask.h"
#include "Task/TcpConnectTask.h"
#include "Task/SearchUserTask.h"

TaskFactory& TaskFactory::getInstance()
{
    static TaskFactory builder;
    return builder;
}

bool TaskFactory::init(std::shared_ptr<IMClient> client)
{
    m_client = client;
    return true;
}

Task::TaskId TaskFactory::generateTaskId()
{
    static std::atomic<Task::TaskId> counter{ 1 };
    return counter.fetch_add(1);
}
