// LoginTask.cpp
#include "UserLoginTask.h"
#include <QMetaObject>
#include "Logger.h"

#include "Base/UserSession.h"

UserLoginTask::UserLoginTask(
    std::shared_ptr<IMClient> client,
    std::string email,
    std::string password,
    QObject* uiReceiver,
    std::function<void()> onSuccess,
    std::function<void(const std::string&)> onError)
    : m_client(std::move(client)),
    m_email(std::move(email)),
    m_password(std::move(password)),
    m_uiReceiver(uiReceiver),
    m_onSuccess(std::move(onSuccess)),
    m_onError(std::move(onError))
{
}

void UserLoginTask::doTask() {
    LOG_INFO("LoginTask: Executing login for email: %s", m_email.c_str());

    auto selfTask = std::static_pointer_cast<UserLoginTask>(shared_from_this());

    m_client->requestUserLogin(
        std::bind(&UserLoginTask::onHttpConnectSuccess, selfTask, std::placeholders::_1),
        std::bind(&UserLoginTask::onLoginError, selfTask, std::placeholders::_1),
        m_email, m_password
    );
}

void UserLoginTask::onHttpConnectSuccess(const User& data) {
    LOG_INFO("LoginTask: Login succeeded, uid=%d", data.uid);

    UserSession::getInstance().setUser(data);

    auto selfTask = std::static_pointer_cast<UserLoginTask>(shared_from_this());

    if (!m_client->connect(data.chatServerHost, data.chatSevrerPort,
        std::bind(&UserLoginTask::onTcpConnectSuccess, selfTask),
        std::bind(&UserLoginTask::onLoginError, selfTask,std::placeholders::_1)))
    {
        onLoginError("连接服务器失败");
    }
}

void UserLoginTask::onLoginError(const std::string& error) {
    LOG_ERROR("LoginTask: Login failed: %s", error.c_str());

    if (m_onError && m_uiReceiver) {
        QMetaObject::invokeMethod(m_uiReceiver,
            [callback = m_onError, error]() {
                callback(error);
            },
            Qt::QueuedConnection
        );
    }
}

void UserLoginTask::onTcpConnectSuccess()
{
    if (m_onSuccess && m_uiReceiver) {
        QMetaObject::invokeMethod(m_uiReceiver,
            [callback = m_onSuccess]() {
                callback();
            },
            Qt::QueuedConnection
        );
    }
}
