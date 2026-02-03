// TaskBuilder.cpp
#include "TaskBuilder.h"
#include "Task/RegisterTask.h"
#include "Task/UserLoginTask.h"
#include "Task/VerifyCodeTask.h"
#include "Task/ResetPasswordTask.h"
#include "Task/TcpConnectTask.h"

TaskBuilder& TaskBuilder::getInstance()
{
    static TaskBuilder builder;
    return builder;
}

bool TaskBuilder::init(std::shared_ptr<IMClient> client)
{
    m_client = client;
    return true;
}

std::shared_ptr<Task> TaskBuilder::buildRegisterTask(const std::string& username, 
    const std::string& email, const std::string& password, const std::string& code)
{
    if (!m_client)
        return nullptr;

    Task::TaskId taskId = generateTaskId();

    auto registerTask = std::make_shared<RegisterTask>(m_client, taskId, username,
        email, password, code);

    return std::move(registerTask);
}

std::shared_ptr<Task> TaskBuilder::buildLoginTask(const std::string& email, const std::string& password)
{
    if (!m_client)
        return nullptr;

    Task::TaskId taskId = generateTaskId();

    auto loginTask = std::make_shared<UserLoginTask>(m_client, taskId, email, password);

    return std::move(loginTask);
}

std::shared_ptr<Task> TaskBuilder::buildVerifyCodeTask(const std::string& email)
{
    if (!m_client)
        return nullptr;

    Task::TaskId taskId = generateTaskId();

    auto verifyCodeTask = std::make_shared<VerifyCodeTask>(m_client, taskId, email );

    return std::move(verifyCodeTask);
}

std::shared_ptr<Task> TaskBuilder::buildResetPasswordTask(const std::string& email, const std::string& newPassword, const std::string& code)
{
    if (!m_client)
        return nullptr;

    Task::TaskId taskId = generateTaskId();

    auto resetPassWordTask = std::make_shared<ResetPasswordTask>(m_client, taskId, email, newPassword,code);

    return std::move(resetPassWordTask);
}

std::shared_ptr<Task> TaskBuilder::buildHttpResponseTask(std::string responseBody, HttpResponseTask::ResponseFunc responseFunc)
{
    if (!m_client)
        return nullptr;

    Task::TaskId taskId = generateTaskId();

    auto httpResponseTask = std::make_shared<HttpResponseTask>(taskId, 
        std::move(responseBody), std::move(responseFunc));

    return std::move(httpResponseTask);
}

std::shared_ptr<Task> TaskBuilder::buildTcpConnectTask(
    std::string ip, uint16_t port)
{
    if (!m_client)
        return nullptr;

    Task::TaskId taskId = generateTaskId();

    auto tcpConnectTask = std::make_shared<TcpConnectTask>(
        m_client,taskId,ip,port
       );

    return std::move(tcpConnectTask);
}

uint64_t TaskBuilder::generateTaskId()
{
    static std::atomic<Task::TaskId> counter{ 1 };
    return counter.fetch_add(1);
}
