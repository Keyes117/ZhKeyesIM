#include "GateServer.h"

using namespace ZhKeyesIMHttp;
using namespace nlohmann;
//using namespace message;

GateServer::GateServer():
    m_spGrpcVerifyClient(std::make_unique<VerifyGrpcClient>()),
    m_spRedisManager(std::make_unique<RedisManager>()),
    m_spHttpServer( std::make_unique<ZhKeyesIMHttp::HttpServer>())
{

}

GateServer::~GateServer()
{
}

bool GateServer::init(ConfigManager& config)
{
    try
    {
        // 使用 getSafe + value_or 提供默认值
        auto threadNumOpt = config.getSafe<std::string>({"GateServer", "threadNum"});
        auto ipOpt = config.getSafe<std::string>({ "GateServer", "ip" });
        auto portOpt = config.getSafe<std::string>({ "GateServer", "port" });
        auto typeOpt = config.getSafe<std::string>({ "GateServer", "IOType" });

        if (!threadNumOpt || !ipOpt || !portOpt || !typeOpt)
        {
            LOG_ERROR("GateServer: 获取GateServer 相关配置失败");
            return false;
        }

        int threadNum = std::stoi(*threadNumOpt);
        std::string ip = *ipOpt;
        uint16_t port = static_cast<uint16_t>(std::stoi(*portOpt));

        IOMultiplexType type = static_cast<IOMultiplexType>(std::stoi(*typeOpt));

        if (!m_spHttpServer->init(threadNum, ip, port, type))
        {
            LOG_ERROR("GateServer: Http 服务 初始化失败");
            return false;
        }


        if (!m_spGrpcVerifyClient->init(config))
        {
            LOG_ERROR("GateServer: gprc 客户端 初始化失败");
            return false;
        }
        if (!m_spRedisManager->init(config))
        {
            LOG_ERROR("GateServer: redis 客户端 初始化失败");
            return false;
        }

        m_spHttpServer->setRequestCallBack(std::bind(&GateServer::onHttpRequest, this, std::placeholders::_1, std::placeholders::_2));
        registerRoutes();
        return true;
    }
    catch (std::exception& e)
    {
        return false;
    }


    
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

    if (!m_router.dispatch(request, response))
    {
        setErrorRequest(response,
            ZhKeyesIMHttp::HttpStatusCode::NotFound,
            ServerStatus::ErrorCodes::InternalError,
            "Route not found");
    }
}

void GateServer::handleGetRoot(const ZhKeyesIMHttp::HttpRequest& request, ZhKeyesIMHttp::HttpResponse& response, const std::map<std::string, std::string>& params)
{
    response.setStatusCode(ZhKeyesIMHttp::HttpStatusCode::OK);
    response.setBody("Welcome to ZhKeyesIM Gateway");
}

void GateServer::handleGetVerifyCode(const ZhKeyesIMHttp::HttpRequest& request, ZhKeyesIMHttp::HttpResponse& response, const std::map<std::string, std::string>& params)
{

    try
    {
        auto jsonOpt = ZhKeyes::Util::JsonUtil::parseSafe(request.getBody());
        if (!jsonOpt)
        {
            setErrorRequest(response,
                ZhKeyesIMHttp::HttpStatusCode::BadRequest,
                ServerStatus::ErrorCodes::InternalError,
                "Invalid reqeust JSON format"
            );
            return;
        }

        auto emailOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(*jsonOpt, "email");
        if (!emailOpt || emailOpt->empty())
        {
            setErrorRequest(response,
                ZhKeyesIMHttp::HttpStatusCode::BadRequest,
                ServerStatus::ErrorCodes::InternalError,
                "Email field is required and cannot be empty");
            return;
        }

        std::string email = *emailOpt;

        auto promise = std::make_shared<std::promise<message::GetVerifyResponse>>();
        auto future = promise->get_future();

        m_spGrpcVerifyClient->GetVerifyCodeAsync(email,
            [promise](const message::GetVerifyResponse& grpcResponse) {
                // 这个回调在 gRPC CompletionQueue 线程中执行
                // 将结果传递给 future
                try {
                    promise->set_value(grpcResponse);
                }
                catch (const std::exception& e) {
                    // 防止重复 set_value 导致异常
                    LOG_ERROR("Failed to set promise value: %s", e.what());
                }
            });

        // 4. 等待结果（带超时保护）
         //    虽然这里会阻塞当前线程，但 GRPC 调用是在独立线程中执行的
         //    EventLoop 可以继续处理其他事件（定时器、其他连接等）
        message::GetVerifyResponse grpcResponse;
        auto status = future.wait_for(std::chrono::seconds(5));  // 5秒超时

        if (status == std::future_status::ready) {
            // 成功获取结果
            grpcResponse = future.get();
        }
        else if (status == std::future_status::timeout) {
            // 超时处理
            LOG_ERROR("GRPC call timeout for email: %s", email.c_str());
            grpcResponse.set_error(static_cast<uint32_t>(ServerStatus::GrpcErrors::Timeout));
        }
        else {
            // 其他错误
            LOG_ERROR("GRPC call deferred for email: %s", email.c_str());
            grpcResponse.set_error(static_cast<uint32_t>(ServerStatus::GrpcErrors::Exception));
        }
        //// 5. 记录日志
        LOG_DEBUG("Email: %s, Error code: %d", email.c_str(), grpcResponse.error());

        //// 6. 相应
        ServerStatus::GrpcErrors grpcStatus =
            static_cast<ServerStatus::GrpcErrors>(grpcResponse.error());

        if (grpcStatus == ServerStatus::GrpcErrors::Success)
        {
            setSuccessReqeust(response,
                ZhKeyesIMHttp::HttpStatusCode::OK,
                ServerStatus::ErrorCodes::Success,
                "Verification Code already sent");
        }
        else
        {
            setErrorRequest(response,
                ZhKeyesIMHttp::HttpStatusCode::InternalServerError,
                ServerStatus::ErrorCodes::RPCFailed,
                "Verification Code Server Error");
        }
    }
    catch (const nlohmann::json::exception& e) {
        // JSON 解析错误
        LOG_ERROR("JSON parse error: %s", e.what());
        setErrorRequest(response,
            ZhKeyesIMHttp::HttpStatusCode::BadRequest,
            ServerStatus::ErrorCodes::InternalError,
            "Invalid JSON format");
    }
    catch (const std::exception& e) {
        // 其他异常
        LOG_ERROR("Exception in handleGetVerifyCode: %s", e.what());
        setErrorRequest(response,
            ZhKeyesIMHttp::HttpStatusCode::InternalServerError,
            ServerStatus::ErrorCodes::InternalError,
            e.what());
    }
}

void GateServer::handleUserLogin(const ZhKeyesIMHttp::HttpRequest& request, ZhKeyesIMHttp::HttpResponse& response, const std::map<std::string, std::string>& params)
{
    try {
        setSuccessReqeust(response,
            ZhKeyesIMHttp::HttpStatusCode::OK,
            ServerStatus::ErrorCodes::Success,
            "User registered successfully");
    }
    catch (const std::exception& e) {
        setErrorRequest(response,
            ZhKeyesIMHttp::HttpStatusCode::BadRequest,
            ServerStatus::ErrorCodes::InternalError,
            e.what());
    }

}

void GateServer::handleUserRegister(const ZhKeyesIMHttp::HttpRequest& request, ZhKeyesIMHttp::HttpResponse& response, const std::map<std::string, std::string>& params)
{
    try {

        auto jsonOpt = ZhKeyes::Util::JsonUtil::parseSafe(request.getBody());
        if (!jsonOpt)
        {
            setErrorRequest(response,
                ZhKeyesIMHttp::HttpStatusCode::BadRequest,
                ServerStatus::ErrorCodes::InternalError,
                "Invalid reqeust JSON format"
            );
            return;
        }

        auto usernameOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(*jsonOpt, "username");
        auto passwordOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(*jsonOpt, "password");
        auto emailOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(*jsonOpt, "email");
        auto codeOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(*jsonOpt, "code");
        if (!emailOpt || !usernameOpt || !emailOpt || !codeOpt)
        {
            setErrorRequest(response,
                ZhKeyesIMHttp::HttpStatusCode::BadRequest,
                ServerStatus::ErrorCodes::InternalError,
                "error request JSON format");
            return;
        }

        std::string username = *usernameOpt;
        std::string password = *passwordOpt;
        std::string email = *emailOpt;
        std::string code = *codeOpt;

        std::string verifyCode;
        bool isVerified = m_spRedisManager->get(email, verifyCode);
        if (!isVerified)
        {
            setErrorRequest(response,
                ZhKeyesIMHttp::HttpStatusCode::InternalServerError,
                ServerStatus::ErrorCodes::VarifyExpired,
                "error request JSON format");
            return;
        }

        if (code != verifyCode)
        {
            setErrorRequest(response,
                ZhKeyesIMHttp::HttpStatusCode::InternalServerError,
                ServerStatus::ErrorCodes::VarifyCodeErr,
                "error request JSON format");
            return;
        }

        bool isUserInRedis = m_spRedisManager->existsKey(username);

        //TODO验证MySQL中是否存在用户

        setSuccessReqeust(response,
            ZhKeyesIMHttp::HttpStatusCode::OK,
            ServerStatus::ErrorCodes::Success,
            "User registered successfully");
    }
    catch (const std::exception& e) {
        setErrorRequest(response,
            ZhKeyesIMHttp::HttpStatusCode::BadRequest,
            ServerStatus::ErrorCodes::InternalError,
            e.what());
    }
}

//void GateServer::setJsonResponse(ZhKeyesIMHttp::HttpResponse& response,
//    const nlohmann::json& json,
//    ServerStatus::ErrorCodes errorCode,
//    ZhKeyesIMHttp::HttpStatusCode code)
//{
//    response.setStatusCode(code);
//    auto jsonStrOpt = ZhKeyes::Util::JsonUtil::dumpSafe(json);
//    if (jsonStrOpt)
//    {
//        response.setJsonResponse(*jsonStrOpt);
//    }
//    else
//    {
//        LOG_ERROR("Failed to dump JSON in setJsonResponse");
//        setErrorRequest(response,
//            ZhKeyesIMHttp::HttpStatusCode::InternalServerError,
//            ServerStatus::ErrorCodes::InternalError,
//            "Internal server error");
//    }
//}

void GateServer::setErrorRequest(ZhKeyesIMHttp::HttpResponse& response,
    ZhKeyesIMHttp::HttpStatusCode code,
    ServerStatus::ErrorCodes errorCode,
    const std::string& message)
{
    try
    {
        nlohmann::json errorData = {
            {"success",0},
            {"code", static_cast<int>(code)},
            {"msg", message},
            {"timestamp", std::time(nullptr)}
        };

        response.setStatusCode(code);
        auto jsonStrOpt = ZhKeyes::Util::JsonUtil::dumpSafe(errorData);
        if (jsonStrOpt)
        {
            response.setJsonResponse(*jsonStrOpt);
        }
        else
        {
            response.setBody("{\"code\":" + std::to_string(static_cast<int>(code)) +
                ",\"success\":\"" + message + "\"}");
        }
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Exception in setErrorRequest: %s", e.what());
        response.setStatusCode(code);
        response.setBody("{\"code\":" + std::to_string(static_cast<int>(code)) +
            ",\"error\":\"Internal error\"}");
    }
}

void GateServer::setSuccessReqeust(ZhKeyesIMHttp::HttpResponse& response, 
    ZhKeyesIMHttp::HttpStatusCode code,
    ServerStatus::ErrorCodes errorCode,
    const std::string& message)
{
    try
    {
        nlohmann::json successData = {
            {"success",1},
            {"code", static_cast<int>(code)},
            {"msg", message},
            {"timestamp", std::time(nullptr)}
        };

        response.setStatusCode(code);
        auto jsonStrOpt = ZhKeyes::Util::JsonUtil::dumpSafe(successData);
        if (jsonStrOpt)
        {
            response.setJsonResponse(*jsonStrOpt);
        }
        else
        {
            response.setBody("{\"code\":" + std::to_string(static_cast<int>(code)) +
                ",\"success\":\"" + message + "\"}");
        }
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Exception in setSuccessReqeust: %s", e.what());
        response.setStatusCode(code);
        response.setBody("{\"code\":" + std::to_string(static_cast<int>(code)) +
            ",\"success\":\"Internal error\"}");
    }
}

void GateServer::registerRoutes()
{
    m_router.addRoute(ZhKeyesIMHttp::HttpMethod::GET, "/",
        std::bind(&GateServer::handleGetRoot, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    m_router.addRoute(HttpMethod::POST, "api/verify/getCode",
        std::bind(&GateServer::handleGetVerifyCode, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    m_router.addRoute(HttpMethod::POST, "api/user/register",
        std::bind(&GateServer::handleUserRegister, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}
