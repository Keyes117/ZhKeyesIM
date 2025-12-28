// RegisterTask.cpp
#include "RegisterTask.h"
#include <QMetaObject>
#include "Logger.h"

RegisterTask::RegisterTask(
    std::shared_ptr<IMClient> client,
    std::string username,
    std::string email,
    std::string password,
    std::string code,
    QObject* uiReceiver,
    std::function<void(int)> onSuccess,
    std::function<void(const std::string&)> onError)
    : m_client(std::move(client)),
    m_username(std::move(username)),
    m_email(std::move(email)),
    m_password(std::move(password)),
    m_code(std::move(code)),
    m_uiReceiver(uiReceiver),
    m_onSuccess(std::move(onSuccess)),
    m_onError(std::move(onError))
{
}

void RegisterTask::doTask() {
    LOG_INFO("RegisterTask: Executing register for user: %s", m_username.c_str());

    // 在网络线程调用IMClient
    m_client->requestRegister(
        std::bind(&RegisterTask::onSuccess, this, std::placeholders::_1),
        std::bind(&RegisterTask::onError, this, std::placeholders::_1),
        m_username, m_email, m_password, m_code     
    );
}

void RegisterTask::onSuccess(int uid) {
    LOG_INFO("RegisterTask: Register succeeded, uid=%d", uid);

    // 切换到UI线程执行回调
    if (m_onSuccess && m_uiReceiver) {
        QMetaObject::invokeMethod(m_uiReceiver,
            [callback = m_onSuccess, uid]() {
                callback(uid);
            },
            Qt::QueuedConnection
        );
    }
}

void RegisterTask::onError(const std::string& error) {
    LOG_ERROR("RegisterTask: Register failed: %s", error.c_str());

    // 切换到UI线程执行回调
    if (m_onError && m_uiReceiver) {
        QMetaObject::invokeMethod(m_uiReceiver,
            [callback = m_onError, error]() {
                callback(error);
            },
            Qt::QueuedConnection
        );
    }
}