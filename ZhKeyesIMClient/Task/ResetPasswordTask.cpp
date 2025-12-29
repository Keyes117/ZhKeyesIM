// ResetPasswordTask.cpp
#include "ResetPasswordTask.h"
#include <QMetaObject>
#include "Logger.h"

ResetPasswordTask::ResetPasswordTask(
    std::shared_ptr<IMClient> client,
    std::string email,
    std::string newPassword,
    std::string code,
    QObject* uiReceiver,
    std::function<void()> onSuccess,
    std::function<void(const std::string&)> onError)
    : m_client(std::move(client)),
    m_email(std::move(email)),
    m_newPassword(std::move(newPassword)),
    m_code(std::move(code)),
    m_uiReceiver(uiReceiver),
    m_onSuccess(std::move(onSuccess)),
    m_onError(std::move(onError))
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

    if (m_onSuccess && m_uiReceiver) {
        QMetaObject::invokeMethod(m_uiReceiver,
            [callback = m_onSuccess]() {
                callback();
            },
            Qt::QueuedConnection
        );
    }
}

void ResetPasswordTask::onError(const std::string& error) {
    LOG_ERROR("ResetPasswordTask: Failed: %s", error.c_str());

    if (m_onError && m_uiReceiver) {
        QMetaObject::invokeMethod(m_uiReceiver,
            [callback = m_onError, error]() {
                callback(error);
            },
            Qt::QueuedConnection
        );
    }
}