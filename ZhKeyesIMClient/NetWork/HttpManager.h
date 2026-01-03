#ifndef ZHKEYESIMCLIENT_NETWORK_HTTPMANAGER_H_
#define ZHKEYESIMCLIENT_NETWORK_HTTPMANAGER_H_

#include <memory>

#include "ConfigManager.h"
#include "EventLoop.h"
#include "global.h"
#include "Http/HttpClient.h"

class HttpManager 
{
public:
    using SuccessCallback = std::function<void()>;
    using ErrorCallback = std::function<void(const std::string&)>;

    template<typename T>
    using DataCallback = std::function<void(const T&)>;

    HttpManager(std::shared_ptr<EventLoop> eventLoop);
    ~HttpManager() = default;


    bool init(const ConfigManager& config);


    void requestVerificationCode(SuccessCallback onSuccess,
        ErrorCallback onError,
        const std::string& email);

    void requestRegister(DataCallback<int> onSuccess,
        ErrorCallback onError,
        const std::string& username,
        const std::string& email,
        const std::string& password,
        const std::string& verificationCode
        );

    void requestResetPassword(SuccessCallback onSuccess,
        ErrorCallback onError,
        const std::string& email,
        const std::string& password,
        const std::string& verificationCode
    );

    void requestUserLogin(DataCallback<User> onSuccess,
        ErrorCallback onError,
        const std::string& username,
        const std::string password
    );

private:
    void onResponseVerificationCode(SuccessCallback onSuccess,
        ErrorCallback onError,
        const ZhKeyesIM::Net::Http::HttpResponse& response);

    void onResponseRegister(DataCallback<int> onSuccess,
        ErrorCallback onError,
        const ZhKeyesIM::Net::Http::HttpResponse& response);

    void onResponseUserLogin(DataCallback<User> onSuccess,
        ErrorCallback onError,
        const ZhKeyesIM::Net::Http::HttpResponse& response);

    void onResponseResetPassword(SuccessCallback onSuccess,
        ErrorCallback onError,
        const ZhKeyesIM::Net::Http::HttpResponse& response);

private:
    std::shared_ptr<EventLoop>              m_spEventLoop;
    std::unique_ptr<ZhKeyesIM::Net::Http::HttpClient>   m_spHttpClient;

    std::string m_httpBaseUrl;

};

#endif