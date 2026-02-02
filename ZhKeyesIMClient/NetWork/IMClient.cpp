 #include "IMClient.h"
#include <iostream>


#include <QMessageBox>

#include "ApiRoutes.h"
#include "JsonUtil.h"
#include "fmt/format.h"

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
   
    m_spHttpManager = std::make_unique<HttpManager>(m_spMainEventLoop);
    if (!m_spHttpManager->init(config));
    {
        LOG_ERROR("IMClient: 初始化 HttpManager 失败");
    }

    m_networkThread = std::make_unique<std::thread>(std::bind(&IMClient::networkThreadFunc, this));
    while (!m_eventLoopRunning.load())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    LOG_INFO("网络线程已启动: %d", m_networkThread->get_id());

    return true;
}

bool IMClient::connect(const std::string& ip, uint16_t port,
    SuccessCallback onSuccess /*= nullptr */, ErrorCallback onError /*= nullptr */)
{
    m_spTcpManager = std::make_unique<TcpManager>(m_spMainEventLoop);

    m_spTcpManager->setConnectCallback(std::move(onSuccess));
    m_spTcpManager->setConnectFailedCallback(std::move(onError));

    return m_spTcpManager->connect(ip, port);
}

void IMClient::requestVerificationCode(SuccessCallback onSuccess,
    ErrorCallback onError,
    const std::string& email)
{
    m_spHttpManager->requestVerificationCode(onSuccess, onError, email);
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

void IMClient::requestUserLogin(DataCallback<User> onSuccess,
    ErrorCallback onError,
    const std::string& username, 
    const std::string password)
{
    m_spHttpManager->requestUserLogin(std::move(onSuccess),
        std::move(onError), username, password);
}

void IMClient::networkThreadFunc()
{
    m_eventLoopRunning.store(true);
    m_spMainEventLoop->run();
    m_eventLoopRunning.store(false);
}
