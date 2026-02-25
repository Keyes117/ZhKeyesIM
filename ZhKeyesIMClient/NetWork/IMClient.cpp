 #include "IMClient.h"
#include <iostream>


#include <QMessageBox>

#include "ApiRoutes.h"
#include "JsonUtil.h"
#include "fmt/format.h"
#include "IMProtocol/BinaryReader.h"

IMClient::IMClient()
{

}
IMClient::~IMClient()
{

    m_spMainEventLoop->stop();
    if (m_networkThread && m_networkThread->joinable())
        m_networkThread->join();
    m_eventLoopRunning.store(false);

}

bool IMClient::init(const ZhKeyes::Util::ConfigManager& config)
{
    auto typeOpt = config.getSafe<std::string>({ "IOType", "type" });

    if (!typeOpt)
    {
        LOG_ERROR("IMClient: 获取IMClient 相关配置失败");
        return false;
    }

    IOMultiplexType type = static_cast<IOMultiplexType>(std::stoi(*typeOpt));

    m_spMainEventLoop = std::make_shared<EventLoop>();
    if (!m_spMainEventLoop->init(type))
    {
        LOG_ERROR("IMClient: 初始化 EventLoop 失败");
        return false;
    }
   
    m_spHttpManager = std::make_shared<HttpManager>(m_spMainEventLoop);
    if (!m_spHttpManager->init(config))
    {
        LOG_ERROR("IMClient: 初始化 HttpManager 失败");
    }

    m_networkThread = std::make_unique<std::thread>(std::bind(&IMClient::networkThreadFunc, this));
    while (!m_eventLoopRunning.load())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    LOG_INFO("网络线程已启动: %d", m_networkThread->get_id());

    registerMessageHandlers();

    return true;
}

bool IMClient::tcpConnect(const std::string& ip, uint16_t port,
    SuccessCallback onSuccess /*= nullptr */, ErrorCallback onError /*= nullptr */)
{
    if(!m_spTcpManager)
        m_spTcpManager = std::make_shared<TcpManager>(m_spMainEventLoop);

    m_spTcpManager->setConnectCallback(std::move(onSuccess));
    m_spTcpManager->setConnectFailedCallback(std::move(onError));

    return m_spTcpManager->connect(ip, port);
}

void IMClient::tcpDisconnect()
{
    m_spMainEventLoop->registerCustomTask([this]()
        {
            m_spTcpManager->disconnect();
        });
}

void IMClient::requestVerificationCode(const std::string& jsonString,
    ZhKeyesIM::Net::Http::HttpClient::ResponseCallback onResponse,
    ZhKeyesIM::Net::Http::HttpClient::ErrorCallback onError)
{
    m_spHttpManager->requestVerificationCode(jsonString, onResponse, onError);
}

void IMClient::requestRegister(const std::string& jsonString,
    ZhKeyesIM::Net::Http::HttpClient::ResponseCallback onResponse,
    ZhKeyesIM::Net::Http::HttpClient::ErrorCallback onError)
{
    m_spHttpManager->requestRegister(jsonString, onResponse, onError);
}

void IMClient::requestResetPassword(const std::string& jsonString,
    ZhKeyesIM::Net::Http::HttpClient::ResponseCallback onResponse,
    ZhKeyesIM::Net::Http::HttpClient::ErrorCallback onError)
{
    m_spHttpManager->requestResetPassword(jsonString, onResponse, onError);
}

void IMClient::requestUserLogin(const std::string& jsonString,
    ZhKeyesIM::Net::Http::HttpClient::ResponseCallback onResponse,
    ZhKeyesIM::Net::Http::HttpClient::ErrorCallback onError)
{
    m_spHttpManager->requestUserLogin(jsonString, onResponse, onError);
}

bool IMClient::auth(uint32_t uid, const std::string& token,
    TcpManager::TcpResponseHandler onResponse, ErrorCallback onError/* = nullptr*/)
{
    return m_spTcpManager->authenticate(uid, token, std::move(onResponse),std::move(onError));
}

bool IMClient::applyFriend(uint32_t uid, TcpManager::TcpResponseHandler onResponse,
    ErrorCallback onError)
{
    return m_spTcpManager->applyFriend(uid, std::move(onResponse), std::move(onError));
}

bool IMClient::searchUser(uint32_t uid, TcpManager::TcpResponseHandler onResponse, ErrorCallback onError)
{
    return m_spTcpManager->searchUser(uid, onResponse, onError);
}

bool IMClient::fetchFriendApplyList(uint32_t uid, TcpManager::TcpResponseHandler onResponse,
    ErrorCallback onError)
{
    return m_spTcpManager->fetchFriendApplyList(uid, std::move(onResponse), std::move(onError));
}

bool IMClient::authenFriendApply(uint32_t uid, uint32_t toUid, uint8_t decision,
            const std::string backName, TcpManager::TcpResponseHandler onResponse, ErrorCallback onError)
{
    return m_spTcpManager->authenFriendApply(uid, toUid, decision, backName, std::move(onResponse), std::move(onError));
}



void IMClient::networkThreadFunc()
{
    m_eventLoopRunning.store(true);
    m_spMainEventLoop->run();
    m_eventLoopRunning.store(false);
}

void IMClient::registerMessageHandlers()
{
    if (!m_spTcpManager)
        return;

    m_spTcpManager->registerHandler(
        ZhKeyesIM::Protocol::MessageType::NOTIFY_FRIEND_APPLY,
        std::bind(&IMClient::onNotifyApplyFriend, this,
            std::placeholders::_1, std::placeholders::_2)
    );
}

void IMClient::onNotifyApplyFriend(std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg, 
    std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender)
{
    if (!msg || !msg->hasBody()) return;

    ZhKeyesIM::Protocol::BinaryReader reader(msg->getBody());

    uint32_t fromUid = 0;
    std::string name, nick, desc, icon;
    uint32_t sex = 0;

    if (!reader.readUInt32(fromUid) ||
        !reader.readString(name) ||
        !reader.readString(nick) ||
        !reader.readString(desc) ||
        !reader.readUInt32(sex) ||
        !reader.readString(icon))
    {
        LOG_WARN("IMClient::onNotifyApplyFriend: 解析失败");
        return;
    }

    LOG_INFO("收到好友申请通知, fromUid=%u, name=%s", fromUid, name.c_str());

    auto friendApply = std::make_shared<AddFriendApply>(
        fromUid, 
        QString::fromStdString(name),
        QString::fromStdString(desc),
        QString::fromStdString(icon),
        QString::fromStdString(nick),
        sex
    );
    emit friendApplyReceived( friendApply);
}
