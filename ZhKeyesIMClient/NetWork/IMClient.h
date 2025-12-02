#ifndef ZHKEYESIMCLIENT_IMCLIENT_H_
#define ZHKEYESIMCLIENT_IMCLIENT_H_

#include "Http/HttpClient.h"
#include "TCPClient.h"
#include "ConfigManager.h"

class IMClient
{
public:
    IMClient();
    ~IMClient();

    bool init(const ConfigManager& config);
    bool connect();

    void requestVerificationCode(const std::string& email);

private:
    void networkThreadFunc();

    void onResponseVerificationCode(const ZhKeyesIM::Net::Http::HttpResponse& response);
    void onErrorVerificationCode(const std::string& errorMsg);
private:

    std::atomic<bool>                       m_eventLoopRunning = false;

    std::shared_ptr<EventLoop>              m_spMainEventLoop;
    std::unique_ptr<std::thread>            m_networkThread;
    std::unique_ptr<TCPClient>              m_spTcpClient;
    std::unique_ptr<ZhKeyesIM::Net::Http::HttpClient>   m_spHttpClient;

    std::string m_httpBaseUrl;
};


#endif
