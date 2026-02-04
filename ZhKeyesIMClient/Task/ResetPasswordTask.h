// ResetPasswordTask.h
#ifndef ZHKEYESIMCLIENT_TASK_RESETPASSWORDTASK_H_
#define ZHKEYESIMCLIENT_TASK_RESETPASSWORDTASK_H_

#include <functional>
#include <memory>
#include <string>

#include <QObject>

#include "net/Http/HttpResponse.h"

#include "Task/Task.h"
#include "NetWork/IMClient.h"

/**
 * 重置密码任务
 */
class ResetPasswordTask : public Task 
{
    Q_OBJECT

public:
    ResetPasswordTask(
        Task::ConstructorKey key,
        std::shared_ptr<IMClient> client,
        Task::TaskId id,
        std::string email,
        std::string newPassword,
        std::string code
  );

    ~ResetPasswordTask() override = default;

    void doTask() override;

private:
    void onHttpResponse(const ZhKeyesIM::Net::Http::HttpResponse& response);

private:
    std::shared_ptr<IMClient> m_client;
    std::string m_email;
    std::string m_newPassword;
    std::string m_code;
};

#endif