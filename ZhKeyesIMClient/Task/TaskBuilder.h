#ifndef ZHKEYESIMCLIENT_TASK_TASKBUILDER_H_
#define ZHKEYESIMCLIENT_TASK_TASKBUILDER_H_

#include <atomic>
#include <cstdint>
#include <memory>
#include <functional>
#include <string>

#include "Task/Task.h"
#include "Task/HttpResponseTask.h"
#include "NetWork/IMClient.h"


/**
 * @brief 任务统一构建类
 */
class TaskBuilder
{
public:
    ~TaskBuilder() = default;

    static TaskBuilder& getInstance();

    bool init(std::shared_ptr<IMClient> client);

    // ==================== 任务构建方法 ====================

    /**
     * 注册任务
     */
    std::shared_ptr<Task> buildRegisterTask(
        const std::string& username,
        const std::string& email,
        const std::string& password,
        const std::string& code
    );

    /**
     * 登录任务
     */
    std::shared_ptr<Task> buildLoginTask(
        const std::string& email,
        const std::string& password
     );

    /**
     * 验证码任务
     */
    std::shared_ptr<Task> buildVerifyCodeTask(
        const std::string& email
    );

    /**
     * 重置密码
     */
    std::shared_ptr<Task> buildResetPasswordTask(
        const std::string& email,
        const std::string& newPassword,
        const std::string& code
   );
    
    std::shared_ptr<Task> buildHttpResponseTask(
        std::string responseBody,
        HttpResponseTask::ResponseFunc responseFunc
    );

    std::shared_ptr<Task> buildTcpConnectTask(
        std::string ip,
        uint16_t port
    );

private:
    TaskBuilder() = default;
    Task::TaskId generateTaskId();

    std::shared_ptr<IMClient> m_client;
};

#endif