#ifndef ZHKEYESIMCLIENT_IMCLIENT_H_
#define ZHKEYESIMCLIENT_IMCLIENT_H_

#include <string>

#include "Http/HttpClient.h"
#include "TCPClient.h"
#include "ConfigManager.h"
#include "global.h"

class IMClient
{
public:
    using SuccessCallback = std::function<void()>;
    using ErrorCallback = std::function<void(const std::string&)>;

    template<typename T>
    using DataCallback = std::function<void(const T&)>;

public:
    IMClient();
    ~IMClient();

    bool init(const ConfigManager& config);
    bool connect();

    void requestVerificationCode(SuccessCallback onSuccess,
        ErrorCallback onError,
        const std::string& email);

    void requestRegister(DataCallback<int> onSuccess,
        ErrorCallback onError,
        const std::string& username,
        const std::string& email,
        const std::string& password,
        const std::string& verificationCode);

    void requestResetPassword(SuccessCallback onSuccess,
        ErrorCallback onError,
        const std::string& email,
        const std::string& password,
        const std::string& verificationCode
   );

    void requestUserLogin(DataCallback<UserData> onSuccess,
        ErrorCallback onError,
        const std::string& username,
        const std::string password
 );

private:
    void networkThreadFunc();
    void reportErrorMsg(const std::string& msg);
    void reportSuccessMsg(const std::string& msg);

//onResponse
private:
    void onResponseVerificationCode(SuccessCallback onSuccess,
        ErrorCallback onError,
        const ZhKeyesIM::Net::Http::HttpResponse& response);

    void onResponseRegister(DataCallback<int> onSuccess,
        ErrorCallback onError,
        const ZhKeyesIM::Net::Http::HttpResponse& response);

    void onResponseUserLogin(DataCallback<UserData> onSuccess,
        ErrorCallback onError,
        const ZhKeyesIM::Net::Http::HttpResponse& response);

    void onResponseResetPassword(SuccessCallback onSuccess,
        ErrorCallback onError,
        const ZhKeyesIM::Net::Http::HttpResponse& response);


//onError
private:
    void onErrorVerificationCode(ErrorCallback onError,
        const std::string& errorMsg);

    void onErrorRegister(ErrorCallback onError,
        const std::string& errorMsg);

    void onErrorResetPassword(ErrorCallback onError,
        const std::string& errorMsg);

    void onErrorUserLogin(ErrorCallback onError,
        const std::string& errorMsg);

private:

    std::atomic<bool>                       m_eventLoopRunning = false;

    std::shared_ptr<EventLoop>              m_spMainEventLoop;
    std::unique_ptr<std::thread>            m_networkThread;
    std::unique_ptr<TCPClient>              m_spTcpClient;
    std::unique_ptr<ZhKeyesIM::Net::Http::HttpClient>   m_spHttpClient;

    std::string m_httpBaseUrl;
};


#endif
