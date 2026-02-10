// GetVerifyCodeTask.cpp
#include "VerifyCodeTask.h"
#include <QMetaObject>
#include "Logger.h"

#include "Task/TaskBuilder.h"
#include "Task/TaskHandler.h"

VerifyCodeTask::VerifyCodeTask(
    Task::ConstructorKey key,
    Task::TaskId id,
    std::shared_ptr<IMClient> client,
    std::string email)
    :Task(key, id,Task::TaskType::TASK_TYPE_VERIFYCODE),
    m_spClient(std::move(client)),
    m_email(std::move(email))
{
}

void VerifyCodeTask::doTask()
{
    LOG_INFO("GetVerifyCodeTask: Sending verify code to: %s", m_email.c_str());

    nlohmann::json requestJson;
    requestJson["email"] = m_email;

   
    m_spClient->requestVerificationCode(requestJson.dump(),
        std::bind(&VerifyCodeTask::onHttpResponse, this, std::placeholders::_1),
        std::bind(&VerifyCodeTask::onTaskError,this, std::placeholders::_1)
    );
}

void VerifyCodeTask::onHttpResponse(const ZhKeyesIM::Net::Http::HttpResponse& response)
{
    std::string responseBody = response.getBody();

    auto responseFunc = [this](const std::string& responseBody) mutable
        {
            auto requestJsonOpt = ZhKeyes::Util::JsonUtil::parseSafe(responseBody);
            if (!requestJsonOpt)
            {
                onTaskError("验证码接收错误");
                LOG_ERROR("IMClient:onResponseVerificationCode:接收返回值格式错误：不是正常的Json格式");
                return;
            }

            nlohmann::json requestJson = *requestJsonOpt;
            auto successOpt = ZhKeyes::Util::JsonUtil::getSafe<int>(requestJson, "success");
            if (!successOpt)
            {
                onTaskError("验证码接收错误");
                LOG_ERROR("IMClient:onResponseVerificationCode:接收返回值格式错误：不是正常的Json格式");
                return;
            }

            int success = *successOpt;
            if (success == 0)
            {
                onTaskError("验证码服务出错");
                LOG_ERROR("IMClient:onResponseVerificationCode:未成功发送验证吗");
                return;
            }

            onTaskSuccess();
        };


    auto responseTask = TaskBuilder::getInstance().buildHttpResponseTask(
        std::move(responseBody),      // 移动局部变量
        std::move(responseFunc));      // 移动 lambda
    TaskHandler::getInstance().registerUITask(std::move(responseTask));
}

