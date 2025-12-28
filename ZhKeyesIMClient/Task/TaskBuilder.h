// TaskBuilder.h
#ifndef ZHKEYESIMCLIENT_TASK_TASKBUILDER_H_
#define ZHKEYESIMCLIENT_TASK_TASKBUILDER_H_

#include <memory>
#include <functional>
#include <string>
#include <QObject>
#include "Task.h"
#include "IMClient.h"


/**
 * 任务构建器
 * 使用Builder模式创建各种任务
 *
 * 职责：
 * 1. 统一创建各种任务
 * 2. 简化任务创建流程
 * 3. 集中管理任务配置
 */
class TaskBuilder {
public:
    TaskBuilder() = default;
    ~TaskBuilder() = default;

    /**
     * 设置网络客户端
     * @param client IMClient实例
     * @return this（支持链式调用）
     */
    TaskBuilder& setClient(std::shared_ptr<IMClient> client) {
        m_client = client;
        return *this;
    }

    /**
     * 设置UI接收者（用于线程切换）
     * @param receiver UI对象（通常是QDialog或QWidget）
     * @return this（支持链式调用）
     */
    TaskBuilder& setUIReceiver(QObject* receiver) {
        m_uiReceiver = receiver;
        return *this;
    }

    // ==================== 任务构建方法 ====================

    /**
     * 构建注册任务
     */
    std::shared_ptr<Task> buildRegisterTask(
        const std::string& username,
        const std::string& email,
        const std::string& password,
        const std::string& code,
        std::function<void(int)> onSuccess,
        std::function<void(const std::string&)> onError);

    /**
     * 构建登录任务
     */
    std::shared_ptr<Task> buildLoginTask(
        const std::string& email,
        const std::string& password,
        std::function<void(const UserData&)> onSuccess,
        std::function<void(const std::string&)> onError);

    /**
     * 构建获取验证码任务
     */
    std::shared_ptr<Task> buildVerifyCodeTask(
        const std::string& email,
        std::function<void()> onSuccess,
        std::function<void(const std::string&)> onError);

    /**
     * 构建重置密码任务
     */
    std::shared_ptr<Task> buildResetPasswordTask(
        const std::string& email,
        const std::string& newPassword,
        const std::string& code,
        std::function<void()> onSuccess,
        std::function<void(const std::string&)> onError);

private:
    std::shared_ptr<IMClient> m_client;
    QObject* m_uiReceiver = nullptr;
};

#endif