// ResetPasswordTask.h
#ifndef ZHKEYESIMCLIENT_TASK_RESETPASSWORDTASK_H_
#define ZHKEYESIMCLIENT_TASK_RESETPASSWORDTASK_H_

#include <functional>
#include <memory>
#include <string>
#include <QObject>
#include "Task.h"
#include "IMClient.h"

/**
 * ÷ÿ÷√√‹¬Î»ŒŒÒ
 */
class ResetPasswordTask : public Task {
public:
    ResetPasswordTask(std::shared_ptr<IMClient> client,
        std::string email,
        std::string newPassword,
        std::string code,
        QObject* uiReceiver,
        std::function<void()> onSuccess,
        std::function<void(const std::string&)> onError);

    ~ResetPasswordTask() override = default;

    void doTask() override;

private:
    void onSuccess();
    void onError(const std::string& error);

private:
    std::shared_ptr<IMClient> m_client;
    std::string m_email;
    std::string m_newPassword;
    std::string m_code;
    QObject* m_uiReceiver;
    std::function<void()> m_onSuccess;
    std::function<void(const std::string&)> m_onError;
};

#endif