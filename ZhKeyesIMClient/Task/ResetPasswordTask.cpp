// ResetPasswordTask.cpp
#include "ResetPasswordTask.h"

#include "Logger.h"

#include "Task/TaskBuilder.h"
#include "Task/TaskHandler.h"

ResetPasswordTask::ResetPasswordTask(
    Task::ConstructorKey key,
    Task::TaskId id,
    std::shared_ptr<IMClient> client,
    std::string email,
    std::string newPassword,
    std::string code)
    :Task(key, id, Task::TaskType::TASK_TYPE_RESETPASS),
    m_client(std::move(client)),
    m_email(std::move(email)),
    m_newPassword(std::move(newPassword)),
    m_code(std::move(code))
{
}

void ResetPasswordTask::doTask() {
    LOG_INFO("ResetPasswordTask: Resetting password for: %s", m_email.c_str());

    nlohmann::json requestJson;
    requestJson["password"] = m_newPassword;
    requestJson["email"] = m_email;
    requestJson["code"] = m_code;

    m_client->requestResetPassword(requestJson.dump(),
        std::bind(&ResetPasswordTask::onHttpResponse, this, std::placeholders::_1),
        std::bind(&ResetPasswordTask::onTaskError, this, std::placeholders::_1)       
    );
}

void ResetPasswordTask::onHttpResponse(const ZhKeyesIM::Net::Http::HttpResponse& response)
{
    std::string responseBody = response.getBody();

    auto responseFunc = [this](const std::string& responseBody)
        {
            auto requestJsonOpt = ZhKeyes::Util::JsonUtil::parseSafe(responseBody);
            if (!requestJsonOpt)
            {
                onTaskError("重置密码功能返回信息错误");
                LOG_ERROR("IMClient:onResponseResetPassword:接收返回值格式错误：不是正常的Json格式");
                return;
            }

            nlohmann::json requestJson = *requestJsonOpt;
            auto successOpt = ZhKeyes::Util::JsonUtil::getSafe<int>(requestJson, "success");
            auto msgOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(requestJson, "msg");
            if (!successOpt || !msgOpt)
            {
                onTaskError("重置密码功能返回信息错误");
                LOG_ERROR("IMClient:onResponseResetPassword:接收返回值格式错误：不是正常的Json格式");
                return;
            }

            int success = *successOpt;
            std::string msg = *msgOpt;
            if (success == 0)
            {
                onTaskError(msg);
                LOG_ERROR("IMClient:onResponseResetPassword:未成功重置密码");
            }
            else if (success == 1)
            {
                onTaskSuccess();
            }
            return;

        };


    auto responseTask = TaskBuilder::getInstance().buildHttpResponseTask(
        std::move(responseBody),      
        std::move(responseFunc));     

    TaskHandler::getInstance().registerUITask(std::move(responseTask));
}
