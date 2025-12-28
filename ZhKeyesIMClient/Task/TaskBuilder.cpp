// TaskBuilder.cpp
#include "TaskBuilder.h"
#include "RegisterTask.h"
#include "Task/UserLoginTask.h"
#include "Task/VerifyCodeTask.h"
#include "ResetPasswordTask.h"

std::shared_ptr<Task> TaskBuilder::buildRegisterTask(
    const std::string& username,
    const std::string& email,
    const std::string& password,
    const std::string& code,
    std::function<void(int)> onSuccess,
    std::function<void(const std::string&)> onError)
{
    return std::make_shared<RegisterTask>(
        m_client,
        username, email, password, code,
        m_uiReceiver,
        std::move(onSuccess),
        std::move(onError)
    );
}

std::shared_ptr<Task> TaskBuilder::buildLoginTask(
    const std::string& email,
    const std::string& password,
    std::function<void(const UserData&)> onSuccess,
    std::function<void(const std::string&)> onError)
{
    return std::make_shared<UserLoginTask>(
        m_client,
        email, password,
        m_uiReceiver,
        std::move(onSuccess),
        std::move(onError)
    );
}

std::shared_ptr<Task> TaskBuilder::buildVerifyCodeTask(
    const std::string& email,
    std::function<void()> onSuccess,
    std::function<void(const std::string&)> onError)
{
    return std::make_shared<GetVerifyCodeTask>(
        m_client,
        email,
        m_uiReceiver,
        std::move(onSuccess),
        std::move(onError)
    );
}

std::shared_ptr<Task> TaskBuilder::buildResetPasswordTask(
    const std::string& email,
    const std::string& newPassword,
    const std::string& code,
    std::function<void()> onSuccess,
    std::function<void(const std::string&)> onError)
{
    return std::make_shared<ResetPasswordTask>(
        m_client,
        email, newPassword, code,
        m_uiReceiver,
        std::move(onSuccess),
        std::move(onError)
    );
}