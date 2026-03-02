#ifndef ZHKEYESIMCLIENT_IMCLIENT_H_
#define ZHKEYESIMCLIENT_IMCLIENT_H_

#include <QObject>

#include <cstdint>

#include <string>

#include "NetWork/HttpManager.h"
#include "NetWork/TcpManager.h"
#include "util/ConfigManager.h"
#include "Base/global.h"
#include "Base/UserData.h"


class IMClient : public QObject
{
    Q_OBJECT
public:
    using SuccessCallback = std::function<void()>;
    using ErrorCallback = std::function<void(const std::string&)>;

public:
    IMClient();
    ~IMClient();

    bool init(const ZhKeyes::Util::ConfigManager& config);

    void requestVerificationCode(const std::string& jsonString,
        ZhKeyesIM::Net::Http::HttpClient::ResponseCallback onResponse,
        ZhKeyesIM::Net::Http::HttpClient::ErrorCallback onError);

    void requestRegister(const std::string& jsonStr,
        ZhKeyesIM::Net::Http::HttpClient::ResponseCallback onResponse,
        ZhKeyesIM::Net::Http::HttpClient::ErrorCallback onError);
 
    void requestResetPassword(const std::string& jsonString,
        ZhKeyesIM::Net::Http::HttpClient::ResponseCallback onResponse,
        ZhKeyesIM::Net::Http::HttpClient::ErrorCallback onError
   );

    void requestUserLogin(const std::string& jsonString,
        ZhKeyesIM::Net::Http::HttpClient::ResponseCallback onResponse,
        ZhKeyesIM::Net::Http::HttpClient::ErrorCallback onError
    );

    // ==================== Tcp =============================

    bool tcpConnect(const std::string& ip, uint16_t port,
        SuccessCallback onSuccess = nullptr, ErrorCallback onError = nullptr);

    void tcpDisconnect();

    bool auth(uint32_t uid, const std::string& token, 
        TcpManager::TcpResponseHandler onResponse = nullptr, ErrorCallback onError = nullptr);

    bool applyFriend(uint32_t uid,TcpManager::TcpResponseHandler onResponse, ErrorCallback onError =nullptr);

    bool searchUser(uint32_t uid, TcpManager::TcpResponseHandler onResponse, ErrorCallback onError = nullptr);

    bool fetchFriendApplyList(uint32_t uid, TcpManager::TcpResponseHandler onResponse, ErrorCallback onError = nullptr);

    bool authenFriendApply(uint32_t uid, uint32_t toUid, uint8_t decision, const std::string backName,
        TcpManager::TcpResponseHandler onResponse, ErrorCallback onError = nullptr);

private:
    void networkThreadFunc();

    void registerMessageHandlers();

signals:
    void friendApplyReceived(std::shared_ptr< AddFriendApply> applyInfo);
    void authFriendApplyReceived(std::shared_ptr< AuthenApplyNotification> info);

    void authFriendApplyResponse(std::shared_ptr< AuthInfo> info);
private:
    void onNotifyApplyFriend(std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg,
        std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender);

    void onNotifyAuthenFriendApply(std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg,
        std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender);

private:

    std::atomic<bool>                       m_eventLoopRunning = false;

    std::shared_ptr<EventLoop>              m_spMainEventLoop;
    std::unique_ptr<std::thread>            m_networkThread;
    std::shared_ptr<TcpManager>             m_spTcpManager;
    std::shared_ptr<HttpManager>            m_spHttpManager;

    std::string m_httpBaseUrl;
};


#endif
