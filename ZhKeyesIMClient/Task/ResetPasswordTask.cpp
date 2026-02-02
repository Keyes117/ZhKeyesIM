// ResetPasswordTask.cpp
#include "ResetPasswordTask.h"
#include <QMetaObject>
#include "Logger.h"

ResetPasswordTask::ResetPasswordTask(
    std::shared_ptr<IMClient> client,
    Task::TaskId id,
    std::string email,
    std::string newPassword,
    std::string code)
    :Task(id, Task::TaskType::TASK_TYPE_RESETPASS),
    m_client(std::move(client)),
    m_email(std::move(email)),
    m_newPassword(std::move(newPassword)),
    m_code(std::move(code))
{
}

void ResetPasswordTask::doTask() {
    LOG_INFO("ResetPasswordTask: Resetting password for: %s", m_email.c_str());

    auto selfTask = std::static_pointer_cast<ResetPasswordTask>(shared_from_this());

    m_client->requestResetPassword(
        std::bind(&ResetPasswordTask::onSuccess, selfTask),
        std::bind(&ResetPasswordTask::onError, selfTask, std::placeholders::_1),
        m_email, m_newPassword, m_code        
    );
}

void ResetPasswordTask::onSuccess() {
    LOG_INFO("ResetPasswordTask: Password reset successfully");

}

void ResetPasswordTask::onError(const std::string& error) {
    LOG_ERROR("ResetPasswordTask: Failed: %s", error.c_str());
}