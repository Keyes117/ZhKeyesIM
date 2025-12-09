 #include "IMClient.h"
#include <iostream>


#include <QMessageBox>

#include "ApiRoutes.h"
#include "JsonUtil.h"
#include "fmt/format.h"

#include "ReportErrorTask.h"
#include "ReportSuccessTask.h"


IMClient::IMClient()
{

}
IMClient::~IMClient()
{
    m_spTcpClient->disconnect();
    m_spMainEventLoop->stop();
    if (m_networkThread && m_networkThread->joinable())
        m_networkThread->join();
    m_eventLoopRunning.store(false);

}

bool IMClient::init(const ConfigManager& config)
{
    //auto tcpIpOpt = config.getSafe<std::string>({"TCPServer", "Ip"});
    //auto tcpPortOpt = config.getSafe<std::string>({ "TCPServer", "Port" });
    auto baseUrlOpt = config.getSafe<std::string>({ "HttpServer", "baseUrl" });
    auto typeOpt = config.getSafe<std::string>({ "IOType", "type" });

    if ( !baseUrlOpt ||  !typeOpt)
    {
        LOG_ERROR("IMClient: 获取IMClient 相关配置失败");
        return false;
    }

    //std::string tcpIp = *tcpIpOpt;
    //uint16_t tcpPort = static_cast<uint16_t>(std::stoi(*tcpPortOpt));
    m_httpBaseUrl = *baseUrlOpt;

    IOMultiplexType type = static_cast<IOMultiplexType>(std::stoi(*typeOpt));

    //if (!m_spTcpClient->init(tcpIp, tcpPort))
    //    return false;


    m_spMainEventLoop = std::make_shared<EventLoop>();
    if (!m_spMainEventLoop->init(type))
    {
        LOG_ERROR("IMClient: 初始化 EventLoop 失败");
        return false;
    }
    m_spTcpClient = std::make_unique<TCPClient>(m_spMainEventLoop);
    m_spHttpClient = std::make_unique<ZhKeyesIM::Net::Http::HttpClient>(m_spMainEventLoop);
    m_spHttpClient->setConnectionTimeout(5000);
    m_spHttpClient->setRequestTimeout(5000);
    m_networkThread = std::make_unique<std::thread>(std::bind(&IMClient::networkThreadFunc, this));
    while (!m_eventLoopRunning.load())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    LOG_INFO("网络线程已启动: %d", m_networkThread->get_id());

    return true;
}

bool IMClient::connect()
{
    return m_spTcpClient->connect();
}

void IMClient::requestVerificationCode(const std::string& email)
{
    nlohmann::json requestJson;
    requestJson["email"] = email;


    std::string url = fmt::format("http://{}{}", m_httpBaseUrl.c_str(), ApiRoutes::API_VERIFY_GETCODE);
    m_spHttpClient->postJson(url,
        requestJson.dump(),
        std::bind(&IMClient::onResponseVerificationCode, this, std::placeholders::_1),
        std::bind(&IMClient::onErrorVerificationCode, this, std::placeholders::_1));
}

void IMClient::requestRegister(const std::string& username, const std::string& email, const std::string& password, const std::string& verificationCode)
{
    nlohmann::json requestJson;
    requestJson["username"] = username;
    requestJson["password"] = password;
    requestJson["email"] = email;
    requestJson["code"] = verificationCode;

    std::string url = fmt::format("http://{}{}", m_httpBaseUrl.c_str(), ApiRoutes::API_USER_REGISTER);
    m_spHttpClient->postJson(url,
        requestJson.dump(),
        std::bind(&IMClient::onResponseRegister, this, std::placeholders::_1),
        std::bind(&IMClient::onErrorRegister, this, std::placeholders::_1));
}

void IMClient::networkThreadFunc()
{
    m_eventLoopRunning.store(true);
    m_spMainEventLoop->run();
    m_eventLoopRunning.store(false);
}

void IMClient::reportErrorMsg(const std::string& msg)
{
    auto reportErrorTask = std::make_shared<ReportErrorTask>(msg);
    TaskHandler::getInstance().registerUITask(std::move(reportErrorTask));
}

void IMClient::reportSuccessMsg(const std::string& msg)
{
    auto reportSuccessTask = std::make_shared<ReportSuccessTask>(msg);
    TaskHandler::getInstance().registerUITask(std::move(reportSuccessTask));
}


void IMClient::onResponseVerificationCode(const ZhKeyesIM::Net::Http::HttpResponse& response)
{
    auto requestJsonOpt = ZhKeyes::Util::JsonUtil::parseSafe(response.getBody());
    if (!requestJsonOpt)
    {
        onErrorVerificationCode("验证码接收错误");
        LOG_ERROR("IMClient:onResponseVerificationCode:接收返回值格式错误：不是正常的Json格式");
        return;
    }

    nlohmann::json requestJson = *requestJsonOpt;
    auto successOpt = ZhKeyes::Util::JsonUtil::getSafe<int>(requestJson,  "success" );
    if (!successOpt)
    {
        onErrorVerificationCode("验证码接收错误");
        LOG_ERROR("IMClient:onResponseVerificationCode:接收返回值格式错误：不是正常的Json格式");
        return;
    }

    int success = *successOpt;
    if (success == 0)
    {
        onErrorVerificationCode("验证码服务出错");
        LOG_ERROR("IMClient:onResponseVerificationCode:未成功发送验证吗");
        return;
    }


}

void IMClient::onResponseRegister(const ZhKeyesIM::Net::Http::HttpResponse& response)
{
    auto requestJsonOpt = ZhKeyes::Util::JsonUtil::parseSafe(response.getBody());
    if (!requestJsonOpt)
    {
        onErrorRegister("注册功能返回信息错误");
        LOG_ERROR("IMClient:onResponseVerificationCode:接收返回值格式错误：不是正常的Json格式");
        return;
    }

    nlohmann::json requestJson = *requestJsonOpt;
    auto successOpt = ZhKeyes::Util::JsonUtil::getSafe<int>(requestJson, "success");
    auto msgOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(requestJson, "msg");
    if (!successOpt || !msgOpt)
    {
        onErrorRegister("注册功能返回信息错误");
        LOG_ERROR("IMClient:onResponseVerificationCode:接收返回值格式错误：不是正常的Json格式");
        return;
    }

    int success = *successOpt;
    std::string msg = *msgOpt;
    if (success == 0)
    {
        onErrorRegister(msg);
        LOG_ERROR("IMClient:onResponseVerificationCode:未成功注册用户");
        return;
    }
    else if (success == 1)
    {
        reportSuccessMsg("注册成功!");
    }
}

void IMClient::onErrorVerificationCode(const std::string& errorMsg)
{
    reportErrorMsg(errorMsg);
}

void IMClient::onErrorRegister(const std::string& errorMsg)
{
    reportErrorMsg(errorMsg);
}
