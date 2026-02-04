// RegisterTask.h
#ifndef ZHKEYESIMCLIENT_TASK_REGISTERTASK_H_
#define ZHKEYESIMCLIENT_TASK_REGISTERTASK_H_

#include <functional>
#include <memory>
#include <string>

#include "net/Http/HttpResponse.h"

#include "Task/Task.h"
#include "NetWork/IMClient.h"

/**
 * 用户注册任务
 * 负责处理用户注册的网络请求
 */

class RegisterTask : public Task
{
    Q_OBJECT  
public:
    /**
    * 构造函数
    * @param client 网络客户端
    * @param username 用户名
    * @param email 邮箱
    * @param password 密码
    * @param code 验证码
    */
    RegisterTask(
        Task::ConstructorKey key,
        std::shared_ptr<IMClient> client,
        Task::TaskId taskId,
        std::string username,
        std::string email,
        std::string password,
        std::string code
    );

    ~RegisterTask() override = default;

    void doTask() override;

protected:


private:
    void onHttpResponse(const ZhKeyesIM::Net::Http::HttpResponse& response);
    void onHttpSuccess();

private:
    std::shared_ptr<IMClient> m_client;
    std::string m_username;
    std::string m_email;
    std::string m_password;
    std::string m_code;
};

#endif