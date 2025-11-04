#include "GateServer.h"

GateServer::GateServer()
{
}

GateServer::~GateServer()
{
}

bool GateServer::init(uint32_t threadNum, const std::string& ip = "")
{
    m_spHttpServer = std::make_unique<ZhKeyesIMHttp::HttpServer>();
    if (!m_spHttpServer->init(threadNum, ip))
        return false;

    m_spHttpServer->setRequestCallBack(std::bind(&GateServer::onHttpRequest, this, std::placeholders::_1, std::placeholders::_2));

    return true;
}

void GateServer::start()
{
    m_spHttpServer->start();
}

void GateServer::shutdown()
{

}

void GateServer::onHttpRequest(const ZhKeyesIMHttp::HttpRequest& request, ZhKeyesIMHttp::HttpResponse& response)
{
    LOG_INFO("Received: %s request: %s ", request.toString(), request.getPath());

    // 1. CORS 处理 （如果需要）
    response.setHeader("Access-Control-Allow-Origin", "*");
    response.setHeader("Access-Control-Allow-Methods","GET,POST,PUT,DELETE,OPTIONS");
    response.setHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");

    if (request.getMethod() == ZhKeyesIMHttp::HttpMethod::GET && request.getPath() == "/")
    {
        response.setStatusCode(ZhKeyesIMHttp::HttpStatusCode::NoContent);
        return;
    }

    LOG_INFO("Request info: Method = %s, Path = %s, ContentType = %s",
        request.getMethodString(),
        request.getPath(),
        request.getContentType()
    );


    const std::string& path = request.getPath();

    // 2. 身份验证（对于需要鉴权的接口）
    // 3. 路由分发
}

void GateServer::sendJsonResponse(ZhKeyesIMHttp::HttpResponse& response, 
        const nlohmann::json& json,
        ZhKeyesIMHttp::HttpStatusCode code)
{
    response.setStatusCode(code);
    response.setJsonResponse(json.dump());
}

void GateServer::sendErrorRequest(ZhKeyesIMHttp::HttpResponse& response, ZhKeyesIMHttp::HttpStatusCode code, const std::string& message)
{
    nlohmann::json errorData = {
        {"code", static_cast<int>(code)},
        {"error", message},
        {"timestamp", std::time(nullptr)}
    };

    response.setStatusCode(code);
    response.setJsonResponse(errorData);
}

void GateServer::sendSuccessReqeust(ZhKeyesIMHttp::HttpResponse& response, ZhKeyesIMHttp::HttpStatusCode code, const std::string& message)
{
    nlohmann::json successData = {
    {"code", static_cast<int>(code)},
    {"success", message},
    {"timestamp", std::time(nullptr)}
    };

    response.setStatusCode(code);
    response.setJsonResponse(successData);
}
