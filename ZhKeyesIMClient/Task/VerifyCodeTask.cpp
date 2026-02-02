// GetVerifyCodeTask.cpp
#include "VerifyCodeTask.h"
#include <QMetaObject>
#include "Logger.h"

VerifyCodeTask::VerifyCodeTask(
    std::shared_ptr<IMClient> client,
    Task::TaskId id,
    std::string email)
    :Task(id,Task::TaskType::TASK_TYPE_VERIFYCODE),
    m_spClient(std::move(client)),
    m_email(std::move(email))
{
}

void VerifyCodeTask::doTask() {
    LOG_INFO("GetVerifyCodeTask: Sending verify code to: %s", m_email.c_str());

    auto selfTask = std::static_pointer_cast<VerifyCodeTask>(shared_from_this());

    m_spClient->requestVerificationCode(std::bind(&VerifyCodeTask::onSuccess, selfTask),
        std::bind(&VerifyCodeTask::onError, selfTask, std::placeholders::_1),
        m_email
    );
}

void VerifyCodeTask::onSuccess() {
    LOG_INFO("GetVerifyCodeTask: Verify code sent successfully");
}

void VerifyCodeTask::onError(const std::string& error) {
    LOG_ERROR("GetVerifyCodeTask: Failed: %s", error.c_str());

}