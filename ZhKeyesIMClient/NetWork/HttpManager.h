#ifndef ZHKEYESIMCLIENT_NETWORK_HTTPMANAGER_H_
#define ZHKEYESIMCLIENT_NETWORK_HTTPMANAGER_H_

#include <memory>

#include <QObject>

#include "EventLoop.h"
#include "Http/HttpClient.h"
#include "Base/global.h"
#include "util/ConfigManager.h"


class HttpManager : public QObject
{
    Q_OBJECT
public:
    HttpManager(std::shared_ptr<EventLoop> eventLoop);
    ~HttpManager() = default;

    bool init(const ZhKeyes::Util::ConfigManager& config);

    void requestVerificationCode(SuccessCallback onSuccess,
        ErrorCallback onError,
        const std::string& email);

    void requestRegister(const std::string& jsonString,
        ZhKeyesIM::Net::Http::HttpClient::ResponseCallback onResponse,
        ZhKeyesIM::Net::Http::HttpClient::ErrorCallback onError);

    void requestResetPassword(const std::string& jsonString,
        ZhKeyesIM::Net::Http::HttpClient::ResponseCallback onResponse,
        ZhKeyesIM::Net::Http::HttpClient::ErrorCallback onError
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