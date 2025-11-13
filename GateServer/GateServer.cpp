#include "GateServer.h"

#include "VerifyGrpcClient.h"

using namespace ZhKeyesIMHttp;
using namespace nlohmann;
using namespace message;

GateServer::GateServer()
{
}

GateServer::~GateServer()
{
}

bool GateServer::init(uint32_t threadNum, const std::string& ip, uint16_t port)
{
    m_spHttpServer = std::make_unique<HttpServer>();
    if (!m_spHttpServer->init(threadNum, ip, port, IOMultiplexType::Select))
        return false;

    m_spHttpServer->setRequestCallBack(std::bind(&GateServer::onHttpRequest, this, std::placeholders::_1, std::placeholders::_2));
    m_spRouter = std::make_unique<Router>();
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

void GateServer::onHttpRequest(const HttpRequest& request, HttpResponse& response)
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
            HttpStatusCode::NotFound,
            "Route not found");
    }
}

void GateServer::handleGetRoot(const HttpRequest& request, HttpResponse& response, const std::map<std::string, std::string>& params)
{
    response.setStatusCode(HttpStatusCode::OK);
    response.setBody("Welcome to ZhKeyesIM Gateway");
}

void GateServer::handleGetVerifyCode(const HttpRequest& request, HttpResponse& response, const std::map<std::string, std::string>& params)
{
    try
    {
        json requestJson = json::parse(request.getBody());
        std::string email = requestJson["email"];

        GetVerifyResponse grpcResponse = VerifyGrpcClient::getInstance().GetVerifyCode(email);
        LOG_DEBUG("email is %s", email.c_str());

        json responseJson;
        responseJson["error"] = grpcResponse.error();
        responseJson["email"] = grpcResponse.email();
        responseJson["code"] = grpcResponse.code();

        setJsonResponse(response, responseJson, HttpStatusCode::OK);

    }
    catch (const std::exception& e) {
        setErrorRequest(response,
            ZhKeyesIMHttp::HttpStatusCode::BadRequest,
            e.what());
    }
}

void GateServer::handleUserLogin(const HttpRequest& request, HttpResponse& response, const std::map<std::string, std::string>& params)
{
    try {
        auto json = json::parse(request.getBody());

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
            HttpStatusCode::OK);
    }
    catch (const std::exception& e) {
        setErrorRequest(response,
            HttpStatusCode::BadRequest,
            e.what());
    }

}

void GateServer::handleUserRegister(const HttpRequest& request, HttpResponse& response, const std::map<std::string, std::string>& params)
{
    try {
        // 解析JSON请求体
        auto json = json::parse(request.getBody());

        std::string username = json["username"];
        std::string password = json["password"];

        // TODO: 实际的注册逻辑

        setSuccessReqeust(response,
            HttpStatusCode::OK,
            "User registered successfully");
    }
    catch (const std::exception& e) {
        setErrorRequest(response,
            HttpStatusCode::BadRequest,
            e.what());
    }
}

void GateServer::setJsonResponse(HttpResponse& response,
    const json& json,
    HttpStatusCode code)
{
    response.setStatusCode(code);
    response.setJsonResponse(json.dump());
}

void GateServer::setErrorRequest(HttpResponse& response, HttpStatusCode code, const std::string& message)
{
    json errorData = {
        {"code", static_cast<int>(code)},
        {"error", message},
        {"timestamp", std::time(nullptr)}
    };

    response.setStatusCode(code);
    response.setJsonResponse(errorData);
}

void GateServer::setSuccessReqeust(HttpResponse& response, HttpStatusCode code, const std::string& message)
{
    json successData = {
        {"code", static_cast<int>(code)},
        {"success", message},
        {"timestamp", std::time(nullptr)}
    };

    response.setStatusCode(code);
    response.setJsonResponse(successData);
}

void GateServer::registerRoutes()
{
    m_spRouter->addRoute(HttpMethod::GET, "/",
        std::bind(&GateServer::handleGetRoot, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    m_spRouter->addRoute(HttpMethod::POST, "api/user/register",
        std::bind(&GateServer::handleUserRegister, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    m_spRouter->addRoute(HttpMethod::POST, "api/user/login",
        std::bind(&GateServer::handleUserLogin, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    
    m_spRouter->addRoute(HttpMethod::POST, "api/verify/getCode",
        std::bind(&GateServer::handleGetVerifyCode, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
}
