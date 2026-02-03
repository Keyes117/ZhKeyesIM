// LoginTask.h
#ifndef ZHKEYESIMCLIENT_TASK_USERLOGINTASK_H_
#define ZHKEYESIMCLIENT_TASK_USERLOGINTASK_H_

#include <functional>
#include <memory>
#include <string>


#include "NetWork/IMClient.h"
#include "Task/Task.h"
#include "Base/global.h"
#include <Http/HttpResponse.h>

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

    virtual  ~UserLoginTask() override = default;

    virtual void doTask() override;

private:
    void onHttpResponse(const ZhKeyesIM::Net::Http::HttpResponse& response);
    void onHttpSuccess(const User& data);

private:
    std::shared_ptr<IMClient> m_client;
    std::string m_email;
    std::string m_password;
};

#endif