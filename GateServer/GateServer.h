#ifndef GATESERVER_GATESERVRE_H_
#define GATESERVRE_GATESERVER_H_


#include "Http/HttpServer.h"
#include "Http/HttpRequest.h"
#include "Http/HttpResponse.h"

#include "json.hpp"

namespace ZhKeyesIMHttp = ZhKeyesIM::Net::Http;

class GateServer
{
public:
    GateServer();
    ~GateServer();


    bool init(uint32_t threadNum, const std::string& ip = "");
    void start();
    void shutdown();

protected:
    virtual void onHttpRequest(const ZhKeyesIMHttp::HttpRequest& request, ZhKeyesIMHttp::HttpResponse& response);


private:
    void sendJsonResponse(ZhKeyesIMHttp::HttpResponse& response,
        const nlohmann::json& json,
        ZhKeyesIMHttp::HttpStatusCode code);

    void sendErrorRequest(ZhKeyesIMHttp::HttpResponse& response,
        ZhKeyesIMHttp::HttpStatusCode code,
        const std::string& message);

    void sendSuccessReqeust(ZhKeyesIMHttp::HttpResponse& response,
        ZhKeyesIMHttp::HttpStatusCode code,
        const std::string& message);

private:
    std::unique_ptr<ZhKeyesIMHttp::HttpServer> m_spHttpServer;
private:
    GateServer(const GateServer&) = delete;
    GateServer(GateServer&&) noexcept = delete;

    GateServer& operator=(const GateServer&) = delete;
    GateServer& operator=(GateServer&&) = delete;
};

#endif