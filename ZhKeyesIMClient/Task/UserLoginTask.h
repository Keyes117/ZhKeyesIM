// LoginTask.h
#ifndef ZHKEYESIMCLIENT_TASK_USERLOGINTASK_H_
#define ZHKEYESIMCLIENT_TASK_USERLOGINTASK_H_

#include <functional>
#include <memory>
#include <string>
#include <QObject>


#include "NetWork/IMClient.h"
#include "Task/Task.h"
#include "Base/global.h"

/**
 * 用户登录任务
 */
class UserLoginTask : public Task
{
public:
    UserLoginTask(std::shared_ptr<IMClient> client,
        Task::TaskId id,
        std::string email,
        std::string password);

    ~UserLoginTask() override = default;

    void doTask() override;


signals:
    void LoginSuccess();
    void LoginFailed(const QString& errorMsg);

private:
    void onHttpConnectSuccess(const User& data);
    void onTcpConnectSuccess();

    void onLoginError(const std::string& error);
private:
    std::shared_ptr<IMClient> m_client;
    std::string m_email;
    std::string m_password;
};

#endif