#ifndef GATESERVER_GATESERVER_H_
#define GATESERVER_GATESERVER_H_

#include <future>

#include "const.h"
#include "ConfigManager.h"
#include "Http/HttpServer.h"
#include "Http/HttpRequest.h"
#include "Http/HttpResponse.h"
#include "Http/Router.h"
#include "RedisManager.h"
#include "VerifyGrpcClient.h"

#include "JsonUtil.h"

namespace ZhKeyesIMHttp = ZhKeyesIM::Net::Http;


/*
* Grpc服务返回Json
* {
*   "error":"",     //错误,如果为1 则是有错误，0无错误
*   "email":"",     //邮件接收方的邮箱地址， 一般用于二次校验
*   "code":"",      //错误码： 10000-成功，10001-redis服务出错，10002-email服务出错，10003-grpc服务内部出错
* }
*
* 返回客户端Json
* {
*   "success":""    //验证码是否发送成功：1-成功，0-失败
*   "code":""       //错误码   详情请见ErrorCodes、
*   "msg":""        //错误信息，一般由业务自己决定,
*   "timestamp":""  //时间戳
* }
*
*/

class GateServer
{
public:
    GateServer();
    ~GateServer();

    bool init(ConfigManager& config);
    void start();
    void shutdown();

protected:
    virtual void onHttpRequest(const ZhKeyesIMHttp::HttpRequest& request, ZhKeyesIMHttp::HttpResponse& response);

    virtual void handleGetRoot(const ZhKeyesIMHttp::HttpRequest& request, ZhKeyesIMHttp::HttpResponse& response,
        const std::map<std::string, std::string>& params);

    virtual void handleGetVerifyCode(const ZhKeyesIMHttp::HttpRequest& request, ZhKeyesIMHttp::HttpResponse& response,
        const std::map<std::string, std::string>& params);

    virtual void handleUserLogin(const ZhKeyesIMHttp::HttpRequest& request, ZhKeyesIMHttp::HttpResponse& response,
        const std::map<std::string, std::string>& params);

    virtual void handleUserRegister(const ZhKeyesIMHttp::HttpRequest& request, ZhKeyesIMHttp::HttpResponse& response,
        const std::map<std::string, std::string>& params);
private:
    //void setJsonResponse(ZhKeyesIMHttp::HttpResponse& response,
    //    const nlohmann::json& json,
    //    ServerStatus::ErrorCodes errorCode,
    //    ZhKeyesIMHttp::HttpStatusCode code);

    void setErrorRequest(ZhKeyesIMHttp::HttpResponse& response,
        ZhKeyesIMHttp::HttpStatusCode httpCode,
        ServerStatus::ErrorCodes errorCode,
        const std::string& message =  " ");

    void setSuccessReqeust(ZhKeyesIMHttp::HttpResponse& response,
        ZhKeyesIMHttp::HttpStatusCode code,
        ServerStatus::ErrorCodes errorCode,
        const std::string& message = " ");

    void registerRoutes();
private:
    ZhKeyesIMHttp::Router m_router;


    std::unique_ptr<VerifyGrpcClient>  m_spGrpcVerifyClient;
    std::unique_ptr<RedisManager>       m_spRedisManager;
    std::unique_ptr<ZhKeyesIMHttp::HttpServer> m_spHttpServer;

private:
    GateServer(const GateServer&) = delete;
    GateServer(GateServer&&) noexcept = delete;

    GateServer& operator=(const GateServer&) = delete;
    GateServer& operator=(GateServer&&) = delete;
};

#endif
