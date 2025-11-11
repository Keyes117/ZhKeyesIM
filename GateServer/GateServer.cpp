#include "GateServer.h"

GateServer::GateServer()
{
}

GateServer::~GateServer()
{
}

bool GateServer::init(uint32_t threadNum, const std::string& ip, uint16_t port)
{
    m_spHttpServer = std::make_unique<ZhKeyesIMHttp::HttpServer>();
    if (!m_spHttpServer->init(threadNum, ip, port, IOMultiplexType::Select))
        return false;

    m_spHttpServer->setRequestCallBack(std::bind(&GateServer::onHttpRequest, this, std::placeholders::_1, std::placeholders::_2));
    m_spRouter = std::make_unique<ZhKeyesIMHttp::Router>();
    registerRoutes();
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
    LOG_INFO("Received: %s request: %s ", request.toString().c_str(), request.getPath().c_str());

    // 1. CORS 处理 （如果需要）
    //response.setHeader("Access-Control-Allow-Origin", "*");
    //response.setHeader("Access-Control-Allow-Methods", "GET,POST,PUT,DELETE,OPTIONS");
    //response.setHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");

    //LOG_INFO("Request info: Method = %s, Path = %s, ContentType = %s",
    //    request.getMethodString(),
    //    request.getPath(),
    //    request.getContentType()
    //);

    const std::string& path = request.getPath();

    // 2. 身份验证（对于需要鉴权的接口）
    // 3. 路由分发

    if (!m_spRouter->dispatch(request, response))
    {
        setErrorRequest(response,
            ZhKeyesIMHttp::HttpStatusCode::NotFound,
            "Route not found");
    }
}

void GateServer::handleGetRoot(const ZhKeyesIMHttp::HttpRequest& request, ZhKeyesIMHttp::HttpResponse& response, const std::map<std::string, std::string>& params)
{
    response.setStatusCode(ZhKeyesIMHttp::HttpStatusCode::OK);
    response.setBody("Welcome to ZhKeyesIM Gateway");
}

void GateServer::handleUserLogin(const ZhKeyesIMHttp::HttpRequest& request, ZhKeyesIMHttp::HttpResponse& response, const std::map<std::string, std::string>& params)
{
    try {
        auto json = nlohmann::json::parse(request.getBody());

        std::string username = json["username"];
        std::string password = json["password"];

        // TODO: 实际的登录逻辑

        nlohmann::json responseData = {
            {"code", 200},
            {"message", "Login successful"},
            {"data", {
                {"token", "your_token_here"},
                {"userId", 12345}
            }}
        };

        setJsonResponse(response, responseData,
            ZhKeyesIMHttp::HttpStatusCode::OK);
    }
    catch (const std::exception& e) {
        setErrorRequest(response,
            ZhKeyesIMHttp::HttpStatusCode::BadRequest,
            e.what());
    }

}

void GateServer::handleUserRegister(const ZhKeyesIMHttp::HttpRequest& request, ZhKeyesIMHttp::HttpResponse& response, const std::map<std::string, std::string>& params)
{
    try {
        // 解析JSON请求体
        auto json = nlohmann::json::parse(request.getBody());

        std::string username = json["username"];
        std::string password = json["password"];

        // TODO: 实际的注册逻辑

        setSuccessReqeust(response,
            ZhKeyesIMHttp::HttpStatusCode::OK,
            "User registered successfully");
    }
    catch (const std::exception& e) {
        setErrorRequest(response,
            ZhKeyesIMHttp::HttpStatusCode::BadRequest,
            e.what());
    }
}

void GateServer::setJsonResponse(ZhKeyesIMHttp::HttpResponse& response,
    const nlohmann::json& json,
    ZhKeyesIMHttp::HttpStatusCode code)
{
    response.setStatusCode(code);
    response.setJsonResponse(json.dump());
}

void GateServer::setErrorRequest(ZhKeyesIMHttp::HttpResponse& response, ZhKeyesIMHttp::HttpStatusCode code, const std::string& message)
{
    nlohmann::json errorData = {
        {"code", static_cast<int>(code)},
        {"error", message},
        {"timestamp", std::time(nullptr)}
    };

    response.setStatusCode(code);
    response.setJsonResponse(errorData);
}

void GateServer::setSuccessReqeust(ZhKeyesIMHttp::HttpResponse& response, ZhKeyesIMHttp::HttpStatusCode code, const std::string& message)
{
    nlohmann::json successData = {
    {"code", static_cast<int>(code)},
    {"success", message},
    {"timestamp", std::time(nullptr)}
    };

    response.setStatusCode(code);
    response.setJsonResponse(successData);
}

void GateServer::registerRoutes()
{
    m_spRouter->addRoute(ZhKeyesIMHttp::HttpMethod::GET, "/",
        std::bind(&GateServer::handleGetRoot, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    m_spRouter->addRoute(ZhKeyesIMHttp::HttpMethod::POST, "api/user/register",
        std::bind(&GateServer::handleUserRegister, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    m_spRouter->addRoute(ZhKeyesIMHttp::HttpMethod::POST, "api/user/login",
        std::bind(&GateServer::handleUserLogin, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}
