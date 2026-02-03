// RegisterTask.cpp
#include "RegisterTask.h"
#include <QMetaObject>
#include "Logger.h"

#include "Task/TaskHandler.h"
#include "Task/TaskBuilder.h"


RegisterTask::RegisterTask(
    std::shared_ptr<IMClient> client,
    uint64_t taskId,
    std::string username,
    std::string email,
    std::string password,
    std::string code)
    :  Task(taskId,Task::TaskType::TASK_TYPE_REGISTER),
    m_client(client),
    m_username(std::move(username)),
    m_email(std::move(email)),
    m_password(std::move(password)),
    m_code(std::move(code))
{
}

void RegisterTask::onHttpResponse(const ZhKeyesIM::Net::Http::HttpResponse& response)
{
    std::string responseBody = response.getBody();

    auto responseFunc = [this](const std::string& responseBody)
        {
            auto requestJsonOpt = ZhKeyes::Util::JsonUtil::parseSafe(responseBody);
            if (!requestJsonOpt)
            {
                onTaskError("注册功能返回信息错误");
                LOG_WARN("IMClient:onResponseVerificationCode:接收返回值格式错误：不是正常的Json格式");
                return;
            }

            nlohmann::json requestJson = *requestJsonOpt;
            auto successOpt = ZhKeyes::Util::JsonUtil::getSafe<int>(requestJson, "success");
            auto msgOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(requestJson, "msg");
            if (!successOpt || !msgOpt)
            {
                onTaskError("注册功能返回信息错误");
                LOG_WARN("IMClient:onResponseVerificationCode:接收返回值格式错误：不是正常的Json格式");
                return;
            }

            int success = *successOpt;
            std::string msg = *msgOpt;
            if (success == 0)
            {
                onTaskError(msg);
                LOG_WARN("IMClient:onResponseVerificationCode:未成功注册用户");
                return;
            }
            else if (success == 1)
            {
                onTaskSuccess();
            }
        };


    auto responseTask = TaskBuilder::getInstance().buildHttpResponseTask(
        std::move(responseBody),
        std::move(responseFunc)
    );
       
    TaskHandler::getInstance().registerUITask(std::move(responseTask));

}

void RegisterTask::doTask() 
{
    LOG_INFO("RegisterTask: Executing register for user: %s", m_username.c_str());

    nlohmann::json requestJson;
    requestJson["username"] = m_username;
    requestJson["password"] = m_password;
    requestJson["email"] =  m_email;
    requestJson["code"] = m_code;

    auto selfTask = std::static_pointer_cast<RegisterTask>(shared_from_this());

    // 在网络线程调用IMClient
    m_client->requestRegister(requestJson.dump(),
        std::bind(&RegisterTask::onHttpResponse, this, std::placeholders::_1),
        std::bind(&RegisterTask::onTaskError, this, std::placeholders::_1)
    );
}



