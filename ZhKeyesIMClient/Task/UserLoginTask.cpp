// LoginTask.cpp
#include "UserLoginTask.h"
#include <QMetaObject>
#include "Logger.h"

#include "Base/UserSession.h"
#include "Task/TaskBuilder.h"
#include "Task/TaskHandler.h"
#include "Task/HttpResponseTask.h"

UserLoginTask::UserLoginTask(
    std::shared_ptr<IMClient> client,
    Task::TaskId id,
    std::string email,
    std::string password)
    : Task(id, Task::TaskType::TASK_TYPE_LOGIN),
    m_client(std::move(client)),
    m_email(std::move(email)),
    m_password(std::move(password))
{
}

void UserLoginTask::doTask()
{
    LOG_INFO("LoginTask: Executing login for email: %s", m_email.c_str());

    nlohmann::json requestJson;
    requestJson["email"] = m_email;
    requestJson["password"] = m_password;

    auto selfTask = std::static_pointer_cast<UserLoginTask>(shared_from_this());

    m_client->requestUserLogin(
        requestJson.dump(),
        std::bind(&UserLoginTask::onHttpResponse, this, std::placeholders::_1),
        std::bind(&UserLoginTask::onTaskError, this, std::placeholders::_1)
    );
}

void UserLoginTask::onHttpResponse(const ZhKeyesIM::Net::Http::HttpResponse& response)
{
    std::string responseBody = response.getBody();

    auto responseFunc = [this](const std::string& responseBody) mutable
        {
            auto requestJsonOpt = ZhKeyes::Util::JsonUtil::parseSafe(responseBody);
            if (!requestJsonOpt)
            {
                onTaskError("登录功能返回信息错误");
                LOG_WARN("IMClient:onResponseResetPassword:接收返回值格式错误：不是正常的Json格式");
                return;
            }

            nlohmann::json requestJson = *requestJsonOpt;
            auto successOpt = ZhKeyes::Util::JsonUtil::getSafe<int>(requestJson, "success");
            auto msgOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(requestJson, "msg");
            auto dataOpt = ZhKeyes::Util::JsonUtil::getSafe<nlohmann::json>(requestJson, "data");
            if (!successOpt || !msgOpt || !dataOpt)
            {
                onTaskError("登录功能返回信息错误");
                LOG_WARN("IMClient:onResponseUserLogin:接收返回值格式错误：不是正常的Json格式");
                return;
            }

            int success = *successOpt;
            std::string msg = *msgOpt;
            nlohmann::json dataJson = *dataOpt;
            if (success == 0)
            {
                onTaskError(msg);
                LOG_WARN("IMClient:onResponseUserLogin:登录失败");
            }
            else if (success == 1)
            {
                auto tokenOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(dataJson, "token");
                auto uidOpt = ZhKeyes::Util::JsonUtil::getSafe<int64_t>(dataJson, "uid");
                auto usernameOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(dataJson, "username");
                auto emailOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(dataJson, "email");
                auto hostOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(dataJson, "host");
                auto portOpt = ZhKeyes::Util::JsonUtil::getSafe<int>(dataJson, "port");


                if (!tokenOpt || !uidOpt || !usernameOpt || !emailOpt || !hostOpt || !portOpt)
                {
                    LOG_WARN("IMClient:onResponseUserLogin:服务端返回信息异常");
                    onTaskError("服务端返回信息异常");
                    return;
                }

                User data;
                data.token = *tokenOpt;
                data.uid = *uidOpt;
                data.username = *usernameOpt;
                data.email = *emailOpt;
                data.chatServerHost = *hostOpt;
                data.chatSevrerPort = static_cast<uint16_t>(*portOpt);

                onHttpSuccess(data);
            }
            return;
        };


    auto responseTask = TaskBuilder::getInstance().buildHttpResponseTask(
        std::move(responseBody),      // 移动局部变量
        std::move(responseFunc));      // 移动 lambda

    TaskHandler::getInstance().registerUITask(std::move(responseTask));
}

void UserLoginTask::onHttpSuccess(const User& data)
{
    LOG_INFO("LoginTask: Login succeeded, uid=%d", data.uid);

    UserSession::getInstance().setUser(data);

    auto tcpConnectTask = TaskBuilder::getInstance().buildTcpConnectTask(
        data.chatServerHost, data.chatSevrerPort
    );

    connect(tcpConnectTask.get(), &Task::taskSuccess, this, [this]() {
        this->onTaskSuccess();
        });
    connect(tcpConnectTask.get(), &Task::taskFailed, this, [this](const QString& errorMsg) {
        this->onTaskError(errorMsg.toStdString());
        });

    TaskHandler::getInstance().registerNetTask(std::move(tcpConnectTask));
}

