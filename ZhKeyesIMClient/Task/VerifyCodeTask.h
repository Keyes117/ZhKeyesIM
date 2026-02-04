/**
 * @desc:   获取验证码任务，VerifyCodeTask.h
 * @author: ZhKeyes
 * @date:   2025/12/28
 */

#ifndef ZHKEYESIMCLIENT_GETVERIFYCODETASK_H_
#define ZHKEYESIMCLIENT_GETVERIFYCODETASK_H_

#include <functional>
#include <memory>
#include <string>

#include <QObject>

#include "NetWork/IMClient.h"
#include "Task/Task.h"
#include <Http/HttpResponse.h>


class VerifyCodeTask : public Task
{
    Q_OBJECT
public:
    VerifyCodeTask(
        Task::ConstructorKey key,
        std::shared_ptr<IMClient> spClient,
        Task::TaskId id,
        std::string email
     );

    ~VerifyCodeTask() override = default;

    void doTask() override;

private:
    void onHttpResponse(const ZhKeyesIM::Net::Http::HttpResponse& response);

private:
    std::shared_ptr<IMClient> m_spClient;
    std::string m_email;
    QObject* m_uiReceiver;
    std::function<void()> m_onSuccess;
    std::function<void(const std::string&)> m_onError;


};

#endif