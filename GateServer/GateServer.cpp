#include "GateServer.h"

using namespace ZhKeyesIMHttp;
using namespace nlohmann;
//using namespace message;

#include  "model/User.h"


GateServer::GateServer():
    m_spGrpcStatusClient(std::make_shared<StatusGrpcClient>()),
    m_spGrpcVerifyClient(std::make_shared<VerifyGrpcClient>()),
    m_spRedisManager(std::make_shared<RedisManager>()),
    m_spMySqlManager(std::make_shared<MySqlManager>()),
    m_spHttpServer(std::make_unique<ZhKeyesIMHttp::HttpServer>())
{

}

GateServer::~GateServer()
{
}

bool GateServer::init(ZhKeyes::Util::ConfigManager& config)
{
    try
    {
        // 使用 getSafe + value_or 提供默认值
        auto threadNumOpt = config.getSafe<std::string>({"GateServer", "threadNum"});
        auto ipOpt = config.getSafe<std::string>({ "GateServer", "ip" });
        auto portOpt = config.getSafe<std::string>({ "GateServer", "port" });
        auto typeOpt = config.getSafe<std::string>({ "GateServer", "IOType" });

        auto workThreadNumOpt = config.getSafe<std::string>({ "WorkThreadPool", "threadNum" });
        auto maxQueueSizeOpt = config.getSafe<std::string>({ "WorkThreadPool", "maxQueueSize" });

        if (!threadNumOpt || !ipOpt || !portOpt || !typeOpt || !workThreadNumOpt || !maxQueueSizeOpt)
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

        // ================== WorkThreadPool ==================
        int workThreadNum = std::stoi(*workThreadNumOpt);
        int maxQueueSize = std::stoi(*maxQueueSizeOpt);

        m_spWorkThreadPool = std::make_shared<WorkThreadPool>(workThreadNum, maxQueueSize);
        m_spWorkThreadPool->start();

        if (!m_spGrpcStatusClient->init(config))
        {
            LOG_ERROR("GateServer: StatusServer gprc 客户端 初始化失败");
            return false;
        }

        if (!m_spGrpcVerifyClient->init(config))
        {
            LOG_ERROR("GateServer: VerifyServer gprc 客户端 初始化失败");
            return false;
        }
        if (!m_spRedisManager->init(config))
        {
            LOG_ERROR("GateServer: redis 客户端 初始化失败");
            return false;
        }
        if (!m_spMySqlManager->init(config))
        {
            LOG_ERROR("GateServer: MySql 客户端 初始化失败");
            return false;
        }
     

        // ================== Repository ==================
        m_spRedisRepository = std::make_shared<RedisRepository>(m_spRedisManager);
        m_spUserRepository = std::make_shared<UserRepository>(m_spMySqlManager);

        // ================== Service ==================
        m_spAuthService = std::make_shared<AuthService>();
        m_spVerifyService = std::make_shared<VerifyService>(m_spGrpcVerifyClient, m_spRedisRepository);
        m_spUserService = std::make_shared<UserService>(m_spUserRepository,
        m_spRedisRepository, m_spAuthService,m_spGrpcStatusClient, m_spWorkThreadPool);


        // ================== Controller ==================
        m_spVerifyController = std::make_unique<VerifyController>(m_spVerifyService);
        m_spUserController = std::make_unique<UserController>(m_spUserService);

        m_spHttpServer->setAsyncRequestCallBack(
            std::bind(&GateServer::onHttpRequestAsync, this,
                std::placeholders::_1, std::placeholders::_2
            ));

        //m_spHttpServer->setRequestCallBack(std::bind(&GateServer::onHttpRequest, this, std::placeholders::_1, std::placeholders::_2));
        registerRoutes();
        return true;
    }
    catch (std::exception& e)
    {
        LOG_ERROR("GateServer: 初始化时出错,%s",e.what());
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

void GateServer::onHttpRequestAsync(const ZhKeyesIMHttp::HttpRequest& request, ZhKeyesIMHttp::HttpServer::AsyncDone done)
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

    if (!m_router.dispatchAsync(request, done))
    {
        HttpResponse response;
        setErrorRequest(response,
            ZhKeyesIMHttp::HttpStatusCode::NotFound,
            ServerStatus::ErrorCodes::InternalError,
            "Route not found");

        done(std::move(response));
    }

}

void GateServer::handleGetRootAsync(const ZhKeyesIMHttp::HttpRequest& request, ZhKeyesIMHttp::HttpServer::AsyncDone done, const std::map<std::string, std::string>& params)
{
    HttpResponse response;
    response.setStatusCode(HttpStatusCode::OK);
    response.setBody("Welcome to ZhKeyesIM Gateway");
    done(std::move(response));
}


void GateServer::handleGetRoot(const ZhKeyesIMHttp::HttpRequest& request, ZhKeyesIMHttp::HttpResponse& response, const std::map<std::string, std::string>& params)
{
    response.setStatusCode(ZhKeyesIMHttp::HttpStatusCode::OK);
    response.setBody("Welcome to ZhKeyesIM Gateway");
}

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
            response.setJsonResponse("{\"code\":" + std::to_string(static_cast<int>(code)) +
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
    m_router.addAsyncRoute(HttpMethod::GET, "/",
        std::bind(&GateServer::handleGetRootAsync, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    m_router.addAsyncRoute(HttpMethod::POST, "api/verify/getCode",
        std::bind(&VerifyController::handleGetVerifyCode, m_spVerifyController.get(),
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    m_router.addAsyncRoute(HttpMethod::POST, "api/user/register",
        std::bind(&UserController::handleRegisterUser, m_spUserController.get(),
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    m_router.addAsyncRoute(HttpMethod::POST, "api/user/resetPass",
        std::bind(&UserController::handleResetPassword, m_spUserController.get(),
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    m_router.addAsyncRoute(HttpMethod::POST, "api/user/login",
        std::bind(&UserController::handleLogin, m_spUserController.get(),
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

}
