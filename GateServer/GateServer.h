#ifndef GATESERVER_GATESERVRE_H_
#define GATESERVRE_GATESERVER_H_

#include <grpc/event_engine/memory_request.h>
#include "absl/strings/string_view.h"

#include "Http/HttpServer.h"
#include "Http/HttpRequest.h"
#include "Http/HttpResponse.h"
#include "Http/Router.h"

#include "nlohmann/json.hpp"

namespace ZhKeyesIMHttp = ZhKeyesIM::Net::Http;

class GateServer
{
public:
    GateServer();
    ~GateServer();


    bool init(uint32_t threadNum, const std::string& ip = "", uint16_t port = 8080);
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
    std::unique_ptr<ZhKeyesIMHttp::HttpServer> m_spHttpServer;
    std::unique_ptr<ZhKeyesIMHttp::Router> m_spRouter;
private:
    GateServer(const GateServer&) = delete;
    GateServer(GateServer&&) noexcept = delete;

    GateServer& operator=(const GateServer&) = delete;
    GateServer& operator=(GateServer&&) = delete;
};

#endif
