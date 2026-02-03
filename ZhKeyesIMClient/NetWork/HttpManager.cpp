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

void HttpManager::requestVerificationCode(const std::string& jsonString,
    ZhKeyesIM::Net::Http::HttpClient::ResponseCallback onResponse,
    ZhKeyesIM::Net::Http::HttpClient::ErrorCallback onError)
{

    std::string url = fmt::format("http://{}{}", m_httpBaseUrl.c_str(), ApiRoutes::API_VERIFY_GETCODE);
    m_spHttpClient->postJson(url,
        jsonString,
        onResponse, onError);
}

void HttpManager::requestRegister(const std::string& jsonString,
    ZhKeyesIM::Net::Http::HttpClient::ResponseCallback onResponse, 
    ZhKeyesIM::Net::Http::HttpClient::ErrorCallback onError
    )
{  
    std::string url = fmt::format("http://{}{}", m_httpBaseUrl.c_str(), ApiRoutes::API_USER_REGISTER);
    m_spHttpClient->postJson(url,
        jsonString,
        onResponse, onError
    );
}


void HttpManager::requestResetPassword(const std::string& jsonString,
    ZhKeyesIM::Net::Http::HttpClient::ResponseCallback onResponse,
    ZhKeyesIM::Net::Http::HttpClient::ErrorCallback onError)
{

    std::string url = fmt::format("http://{}{}", m_httpBaseUrl.c_str(), ApiRoutes::API_USER_RESETPASS);
    m_spHttpClient->postJson(url,
        jsonString, onResponse, onError
    );
}

void HttpManager::requestUserLogin(const std::string& jsonString,
    ZhKeyesIM::Net::Http::HttpClient::ResponseCallback onResponse,
    ZhKeyesIM::Net::Http::HttpClient::ErrorCallback onError)
{
    std::string url = fmt::format("http://{}{}", m_httpBaseUrl.c_str(), ApiRoutes::API_USER_LOGIN);
    m_spHttpClient->postJson(url,
        jsonString,
        onResponse,onError
    );
}

void HttpManager::onResponseVerificationCode(SuccessCallback onSuccess, ErrorCallback onError, const ZhKeyesIM::Net::Http::HttpResponse& response)
{
   
}

void HttpManager::onResponseUserLogin(DataCallback<User> onSuccess, ErrorCallback onError, const ZhKeyesIM::Net::Http::HttpResponse& response)
{   
}

void HttpManager::onResponseResetPassword(SuccessCallback onSuccess, ErrorCallback onError, const ZhKeyesIM::Net::Http::HttpResponse& response)
{

    
}

