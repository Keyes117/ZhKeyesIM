#ifndef GATESERVER_GATESERVER_H_
#define GATESERVER_GATESERVER_H_

#include <future>

#include "ConfigManager.h"
#include "Http/HttpServer.h"
#include "Http/HttpRequest.h"
#include "Http/HttpResponse.h"
#include "Http/Router.h"
#include "RedisManager.h"
#include "VerifyGrpcClient.h"

#include "JsonUtil.h"

namespace ZhKeyesIMHttp = ZhKeyesIM::Net::Http;


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
    void setJsonResponse(ZhKeyesIMHttp::HttpResponse& response,
        const nlohmann::json& json,
        ZhKeyesIMHttp::HttpStatusCode code);

    void setErrorRequest(ZhKeyesIMHttp::HttpResponse& response,
        ZhKeyesIMHttp::HttpStatusCode code,
        const std::string& message);

    void setSuccessReqeust(ZhKeyesIMHttp::HttpResponse& response,
        ZhKeyesIMHttp::HttpStatusCode code,
        const std::string& message);

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
