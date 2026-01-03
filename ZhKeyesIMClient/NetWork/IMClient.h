#ifndef ZHKEYESIMCLIENT_IMCLIENT_H_
#define ZHKEYESIMCLIENT_IMCLIENT_H_

#include <string>

#include "HttpManager.h"
#include "TcpManager.h"
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
