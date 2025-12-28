// RegisterTask.h
#ifndef ZHKEYESIMCLIENT_TASK_REGISTERTASK_H_
#define ZHKEYESIMCLIENT_TASK_REGISTERTASK_H_

#include <functional>
#include <memory>
#include <string>
#include <QObject>


#include "Task/Task.h"
#include "NetWork/IMClient.h"

/**
 * 用户注册任务
 * 负责处理用户注册的网络请求
 */
class RegisterTask : public Task {
public:
    /**
     * 构造函数
     * @param client 网络客户端
     * @param username 用户名
     * @param email 邮箱
     * @param password 密码
     * @param code 验证码
     * @param uiReceiver UI接收者（用于线程切换）
     * @param onSuccess 成功回调（在UI线程执行）
     * @param onError 失败回调（在UI线程执行）
     */
    RegisterTask(std::shared_ptr<IMClient> client,
        std::string username,
        std::string email,
        std::string password,
        std::string code,
        QObject* uiReceiver,
        std::function<void(int)> onSuccess,
        std::function<void(const std::string&)> onError);

    ~RegisterTask() override = default;

    void doTask() override;

private:
    void onSuccess(int uid);
    void onError(const std::string& error);

private:
    std::shared_ptr<IMClient> m_client;
    std::string m_username;
    std::string m_email;
    std::string m_password;
    std::string m_code;
    QObject* m_uiReceiver;
    std::function<void(int)> m_onSuccess;
    std::function<void(const std::string&)> m_onError;
};

#endif