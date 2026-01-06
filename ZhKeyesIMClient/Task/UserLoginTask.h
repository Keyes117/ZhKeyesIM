// LoginTask.h
#ifndef ZHKEYESIMCLIENT_TASK_USERLOGINTASK_H_
#define ZHKEYESIMCLIENT_TASK_USERLOGINTASK_H_

#include <functional>
#include <memory>
#include <string>
#include <QObject>


#include "Task/Task.h"
#include "NetWork/IMClient.h"
#include "UI/global.h"
/**
 * 用户登录任务
 */
class UserLoginTask : public Task
{
public:
    UserLoginTask(std::shared_ptr<IMClient> client,
        std::string email,
        std::string password,
        QObject* uiReceiver,
        std::function<void()> onSuccess,
        std::function<void(const std::string&)> onError);

    ~UserLoginTask() override = default;

    void doTask() override;

private:
    void onHttpConnectSuccess(const User& data);
    void onTcpConnectSuccess();

    void onLoginError(const std::string& error);




private:
    std::shared_ptr<IMClient> m_client;
    std::string m_email;
    std::string m_password;
    QObject* m_uiReceiver;
    std::function<void()> m_onSuccess;
    std::function<void(const std::string&)> m_onError;
};

#endif