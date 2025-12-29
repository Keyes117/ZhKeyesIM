// GetVerifyCodeTask.cpp
#include "VerifyCodeTask.h"
#include <QMetaObject>
#include "Logger.h"

GetVerifyCodeTask::GetVerifyCodeTask(
    std::shared_ptr<IMClient> client,
    std::string email,
    QObject* uiReceiver,
    std::function<void()> onSuccess,
    std::function<void(const std::string&)> onError)
    : m_spClient(std::move(client)),
    m_email(std::move(email)),
    m_uiReceiver(uiReceiver),
    m_onSuccess(std::move(onSuccess)),
    m_onError(std::move(onError))
{
}

void GetVerifyCodeTask::doTask() {
    LOG_INFO("GetVerifyCodeTask: Sending verify code to: %s", m_email.c_str());

    auto selfTask = std::static_pointer_cast<GetVerifyCodeTask>(shared_from_this());

    m_spClient->requestVerificationCode(std::bind(&GetVerifyCodeTask::onSuccess, selfTask),
        std::bind(&GetVerifyCodeTask::onError, selfTask, std::placeholders::_1),
        m_email
    );
}

void GetVerifyCodeTask::onSuccess() {
    LOG_INFO("GetVerifyCodeTask: Verify code sent successfully");

    if (m_onSuccess && m_uiReceiver) {
        QMetaObject::invokeMethod(m_uiReceiver,
            [callback = m_onSuccess]() {
                callback();
            },
            Qt::QueuedConnection
        );
    }
}

void GetVerifyCodeTask::onError(const std::string& error) {
    LOG_ERROR("GetVerifyCodeTask: Failed: %s", error.c_str());

    if (m_onError && m_uiReceiver) {
        QMetaObject::invokeMethod(m_uiReceiver,
            [callback = m_onError, error]() {
                callback(error);
            },
            Qt::QueuedConnection
        );
    }
}