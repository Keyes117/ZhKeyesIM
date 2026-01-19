#include "HttpManager.h"


#include "NetWork/ApiRoutes.h"
#include "Task/HttpResponseTask.h"
#include "Task/TaskHandler.h"
#include "Base/global.h"

#include "fmt/format.h"

using namespace ZhKeyes::Util;

HttpManager::HttpManager(std::shared_ptr<EventLoop> eventLoop):
    m_spEventLoop(eventLoop)
{
}

bool HttpManager::init(const ConfigManager& config)
{
    auto baseUrlOpt = config.getSafe<std::string>({ "HttpServer", "baseUrl" });
    auto typeOpt = config.getSafe<std::string>({ "IOType", "type" });

    if (!baseUrlOpt || !typeOpt)
    {
        LOG_ERROR("HttpManager: 获取HttpManager 相关配置失败");
        return false;
    }

    m_httpBaseUrl = *baseUrlOpt;
    m_spHttpClient = std::make_unique<ZhKeyesIM::Net::Http::HttpClient>(m_spEventLoop);
    m_spHttpClient->setConnectionTimeout(5000);
    m_spHttpClient->setRequestTimeout(5000);

    return true;
}

void HttpManager::requestVerificationCode(SuccessCallback onSuccess, 
    ErrorCallback onError, 
    const std::string& email)
{
    nlohmann::json requestJson;
    requestJson["email"] = email;


    std::string url = fmt::format("http://{}{}", m_httpBaseUrl.c_str(), ApiRoutes::API_VERIFY_GETCODE);
    m_spHttpClient->postJson(url,
        requestJson.dump(),
        std::bind(&HttpManager::onResponseVerificationCode, this, std::move(onSuccess), std::move(onError), std::placeholders::_1)
    );
}

void HttpManager::requestRegister(DataCallback<int> onSuccess, 
    ErrorCallback onError, 
    const std::string& username, 
    const std::string& email, 
    const std::string& password,
    const std::string& verificationCode)
{
    nlohmann::json requestJson;
    requestJson["username"] = username;
    requestJson["password"] = password;
    requestJson["email"] = email;
    requestJson["code"] = verificationCode;

    std::string url = fmt::format("http://{}{}", m_httpBaseUrl.c_str(), ApiRoutes::API_USER_REGISTER);
    m_spHttpClient->postJson(url,
        requestJson.dump(),
        std::bind(&HttpManager::onResponseRegister, this, std::move(onSuccess), std::move(onError), std::placeholders::_1)
    );
}


void HttpManager::requestResetPassword(SuccessCallback onSuccess,
    ErrorCallback onError, 
    const std::string& email,
    const std::string& password,
    const std::string& verificationCode)
{
    nlohmann::json requestJson;
    requestJson["password"] = password;
    requestJson["email"] = email;
    requestJson["code"] = verificationCode;

    std::string url = fmt::format("http://{}{}", m_httpBaseUrl.c_str(), ApiRoutes::API_USER_RESETPASS);
    m_spHttpClient->postJson(url,
        requestJson.dump(),
        std::bind(&HttpManager::onResponseResetPassword, this, std::move(onSuccess), std::move(onError), std::placeholders::_1)
    );
}

void HttpManager::requestUserLogin(DataCallback<User> onSuccess, ErrorCallback onError, const std::string& username, const std::string password)
{
    nlohmann::json requestJson;
    requestJson["email"] = username;
    requestJson["password"] = password;

    std::string url = fmt::format("http://{}{}", m_httpBaseUrl.c_str(), ApiRoutes::API_USER_LOGIN);
    m_spHttpClient->postJson(url,
        requestJson.dump(),
        std::bind(&HttpManager::onResponseUserLogin, this, std::move(onSuccess), std::move(onError), std::placeholders::_1)
    );
}

void HttpManager::onResponseVerificationCode(SuccessCallback onSuccess, ErrorCallback onError, const ZhKeyesIM::Net::Http::HttpResponse& response)
{
    std::string responseBody = response.getBody();

    auto responseFunc = [onSuccess = std::move(onSuccess), onError = std::move(onError)](const std::string& responseBody) mutable
        {
            auto requestJsonOpt = ZhKeyes::Util::JsonUtil::parseSafe(responseBody);
            if (!requestJsonOpt)
            {
                onError("验证码接收错误");
                LOG_ERROR("IMClient:onResponseVerificationCode:接收返回值格式错误：不是正常的Json格式");
                return;
            }

            nlohmann::json requestJson = *requestJsonOpt;
            auto successOpt = ZhKeyes::Util::JsonUtil::getSafe<int>(requestJson, "success");
            if (!successOpt)
            {
                onError("验证码接收错误");
                LOG_ERROR("IMClient:onResponseVerificationCode:接收返回值格式错误：不是正常的Json格式");
                return;
            }

            int success = *successOpt;
            if (success == 0)
            {
                onError("验证码服务出错");
                LOG_ERROR("IMClient:onResponseVerificationCode:未成功发送验证吗");
                return;
            }

            onSuccess();
        };  


    auto responseTask = std::make_shared<HttpResponseTask>(
        std::move(responseBody),      // 移动局部变量
        std::move(responseFunc));      // 移动 lambda
    TaskHandler::getInstance().registerUITask(std::move(responseTask));
}

void HttpManager::onResponseRegister(DataCallback<int> onSuccess, ErrorCallback onError, const ZhKeyesIM::Net::Http::HttpResponse& response)
{

    std::string responseBody = response.getBody();

    auto responseFunc = [onSuccess = std::move(onSuccess), onError = std::move(onError)](const std::string& responseBody)
        {
            auto requestJsonOpt = ZhKeyes::Util::JsonUtil::parseSafe(responseBody);
            if (!requestJsonOpt)
            {
                onError("注册功能返回信息错误");
                LOG_WARN("IMClient:onResponseVerificationCode:接收返回值格式错误：不是正常的Json格式");
                return;
            }

            nlohmann::json requestJson = *requestJsonOpt;
            auto successOpt = ZhKeyes::Util::JsonUtil::getSafe<int>(requestJson, "success");
            auto msgOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(requestJson, "msg");
            if (!successOpt || !msgOpt)
            {
                onError("注册功能返回信息错误");
                LOG_WARN("IMClient:onResponseVerificationCode:接收返回值格式错误：不是正常的Json格式");
                return;
            }

            int success = *successOpt;
            std::string msg = *msgOpt;
            if (success == 0)
            {
                onError(msg);
                LOG_WARN("IMClient:onResponseVerificationCode:未成功注册用户");
                return;
            }
            else if (success == 1)
            {
                onSuccess(1);
            }
        };


    auto responseTask = std::make_shared<HttpResponseTask>(
        std::move(responseBody),      // 移动局部变量
        std::move(responseFunc));      // 移动 lambda
    TaskHandler::getInstance().registerUITask(std::move(responseTask));


}

void HttpManager::onResponseUserLogin(DataCallback<User> onSuccess, ErrorCallback onError, const ZhKeyesIM::Net::Http::HttpResponse& response)
{

    std::string responseBody = response.getBody();

    auto responseFunc = [onSuccess = std::move(onSuccess), onError = std::move(onError)](const std::string& responseBody) mutable
        {
            auto requestJsonOpt = ZhKeyes::Util::JsonUtil::parseSafe(responseBody);
            if (!requestJsonOpt)
            {
                onError("登录功能返回信息错误");
                LOG_WARN("IMClient:onResponseResetPassword:接收返回值格式错误：不是正常的Json格式");
                return;
            }

            nlohmann::json requestJson = *requestJsonOpt;
            auto successOpt = ZhKeyes::Util::JsonUtil::getSafe<int>(requestJson, "success");
            auto msgOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(requestJson, "msg");
            auto dataOpt = ZhKeyes::Util::JsonUtil::getSafe<nlohmann::json>(requestJson, "data");
            if (!successOpt || !msgOpt || !dataOpt)
            {
                onError("登录功能返回信息错误");
                LOG_WARN("IMClient:onResponseUserLogin:接收返回值格式错误：不是正常的Json格式");
                return;
            }

            int success = *successOpt;
            std::string msg = *msgOpt;
            nlohmann::json dataJson = *dataOpt;
            if (success == 0)
            {
                onError(msg);
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
                    onError("服务端返回信息异常");
                }

                User data;
                data.token = *tokenOpt;
                data.uid = *uidOpt;
                data.username = *usernameOpt;
                data.email = *emailOpt;
                data.chatServerHost = *hostOpt;
                data.chatSevrerPort = static_cast<uint16_t>(*portOpt);

                onSuccess(data);
            }
            return;
        };


    auto responseTask = std::make_shared<HttpResponseTask>(
        std::move(responseBody),      // 移动局部变量
        std::move(responseFunc));      // 移动 lambda
    TaskHandler::getInstance().registerUITask(std::move(responseTask));


   
}

void HttpManager::onResponseResetPassword(SuccessCallback onSuccess, ErrorCallback onError, const ZhKeyesIM::Net::Http::HttpResponse& response)
{

    std::string responseBody = response.getBody();

    auto responseFunc = [onSuccess = std::move(onSuccess), onError = std::move(onError)](const std::string& responseBody)
        {
            auto requestJsonOpt = ZhKeyes::Util::JsonUtil::parseSafe(responseBody);
            if (!requestJsonOpt)
            {
                onError("重置密码功能返回信息错误");
                LOG_ERROR("IMClient:onResponseResetPassword:接收返回值格式错误：不是正常的Json格式");
                return;
            }

            nlohmann::json requestJson = *requestJsonOpt;
            auto successOpt = ZhKeyes::Util::JsonUtil::getSafe<int>(requestJson, "success");
            auto msgOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(requestJson, "msg");
            if (!successOpt || !msgOpt)
            {
                onError("重置密码功能返回信息错误");
                LOG_ERROR("IMClient:onResponseResetPassword:接收返回值格式错误：不是正常的Json格式");
                return;
            }

            int success = *successOpt;
            std::string msg = *msgOpt;
            if (success == 0)
            {
                onError(msg);
                LOG_ERROR("IMClient:onResponseResetPassword:未成功重置密码");
            }
            else if (success == 1)
            {
                onSuccess();
            }
            return;

        };


    auto responseTask = std::make_shared<HttpResponseTask>(
        std::move(responseBody),      // 移动局部变量
        std::move(responseFunc));      // 移动 lambda
    TaskHandler::getInstance().registerUITask(std::move(responseTask));
}

