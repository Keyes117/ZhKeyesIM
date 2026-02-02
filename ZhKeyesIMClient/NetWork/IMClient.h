#ifndef ZHKEYESIMCLIENT_IMCLIENT_H_
#define ZHKEYESIMCLIENT_IMCLIENT_H_

#include <cstdint>

#include <string>

#include "NetWork/HttpManager.h"
#include "NetWork/TcpManager.h"
#include "util/ConfigManager.h"
#include "Base/global.h"

class IMClient
{
public:
    using SuccessCallback = std::function<void()>;
    using ErrorCallback = std::function<void(const std::string&)>;

public:
    IMClient();
    ~IMClient();

    bool init(const ZhKeyes::Util::ConfigManager& config);

    bool connect(const std::string& ip, uint16_t port, 
        SuccessCallback onSuccess = nullptr, ErrorCallback onError = nullptr);

    void requestVerificationCode(SuccessCallback onSuccess,
        ErrorCallback onError,
        const std::string& email);

    void requestRegister(const std::string& jsonStr,
        ZhKeyesIM::Net::Http::HttpClient::ResponseCallback onResponse,
        ZhKeyesIM::Net::Http::HttpClient::ErrorCallback onError);
 
    void requestResetPassword(const std::string& jsonString,
        ZhKeyesIM::Net::Http::HttpClient::ResponseCallback onResponse,
        ZhKeyesIM::Net::Http::HttpClient::ErrorCallback onError
   );

    void requestUserLogin(SuccessCallback onSuccess,
        ErrorCallback onError,
        const std::string& username,
        const std::string password
    );

    

private:
    void networkThreadFunc();

private:

    std::atomic<bool>                       m_eventLoopRunning = false;

    std::shared_ptr<EventLoop>              m_spMainEventLoop;
    std::unique_ptr<std::thread>            m_networkThread;
    std::unique_ptr<TcpManager>             m_spTcpManager;
    std::unique_ptr<HttpManager>            m_spHttpManager;

    std::string m_httpBaseUrl;
};


#endif
