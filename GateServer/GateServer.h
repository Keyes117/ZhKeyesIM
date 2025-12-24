#ifndef GATESERVER_GATESERVER_H_
#define GATESERVER_GATESERVER_H_

#include <future>

#include "const.h"
#include "ConfigManager.h"
#include "Http/HttpServer.h"
#include "Http/HttpRequest.h"
#include "Http/HttpResponse.h"
#include "Http/Router.h"
#include "WorkThreadPool.h"

#include "infrastructure/RedisManager.h"
#include "infrastructure/MySqlManager.h"
#include "infrastructure/VerifyGrpcClient.h"

#include "service/VerifyService.h"
#include "Service/UserService.h"

#include "controller/VerifyController.h"
#include "Controller/UserController.h"

#include "repository/UserRepository.h"
#include "repository/RedisRepository.h"

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
*   "success":""    //功能是否成功：1-成功，0-失败
*   "code":""       //错误码   详情请见ErrorCodes、
*   "msg":""        //错误信息，一般由业务自己决定,
*   "timestamp":""  //时间戳
* }
*
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

    virtual void onHttpRequestAsync(const ZhKeyesIMHttp::HttpRequest& request, ZhKeyesIMHttp::HttpServer::AsyncDone done);



    // ... 其他路由处理函数保持不变，但添加异步版本
    virtual void handleGetRootAsync(const ZhKeyesIMHttp::HttpRequest& request, 
        ZhKeyesIMHttp::HttpServer::AsyncDone done,
        const std::map<std::string, std::string>& params);

    virtual void handleGetRoot(const ZhKeyesIMHttp::HttpRequest& request, ZhKeyesIMHttp::HttpResponse& response,
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


    std::shared_ptr<VerifyGrpcClient>  m_spGrpcVerifyClient;
    std::shared_ptr<RedisManager>      m_spRedisManager;
    std::shared_ptr<MySqlManager>      m_spMySqlManager;
    std::shared_ptr< WorkThreadPool>    m_spWorkThreadPool;

    std::unique_ptr<ZhKeyesIMHttp::HttpServer> m_spHttpServer;

    // =========== Controller ==================
    std::unique_ptr<VerifyController>   m_spVerifyController;
    std::unique_ptr<UserController>     m_spUserController;

    // =========== Service ================
    std::shared_ptr<AuthService>        m_spAuthService;
    std::shared_ptr<VerifyService>      m_spVerifyService;
    std::shared_ptr<UserService>        m_spUserService;


    // =========== Repository ==================
    std::shared_ptr<RedisRepository>    m_spRedisRepository;
    std::shared_ptr<UserRepository>     m_spUserRepository;


private:
    GateServer(const GateServer&) = delete;
    GateServer(GateServer&&) noexcept = delete;

    GateServer& operator=(const GateServer&) = delete;
    GateServer& operator=(GateServer&&) = delete;
};

#endif
