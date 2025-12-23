#include "GateServer.h"

using namespace ZhKeyesIMHttp;
using namespace nlohmann;
//using namespace message;

#include  "model/User.h"


GateServer::GateServer():
    m_spGrpcVerifyClient(std::make_shared<VerifyGrpcClient>()),
    m_spRedisManager(std::make_shared<RedisManager>()),
    m_spMySqlManager(std::make_shared<MySqlManager>()),
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


        //if (!m_spGrpcVerifyClient->init(config))
        //{
        //    LOG_ERROR("GateServer: gprc 客户端 初始化失败");
        //    return false;
        //}
        //if (!m_spRedisManager->init(config))
        //{
        //    LOG_ERROR("GateServer: redis 客户端 初始化失败");
        //    return false;
        //}
        //if (!m_spMySqlManager->init(config))
        //{
        //    LOG_ERROR("GateServer: MySql 客户端 初始化失败");
        //    return false;
        //}

        // ================== Repository ==================
        m_spRedisRepository = std::make_shared<RedisRepository>(m_spRedisManager);

        // ================== Service ==================
        m_spVerifyService = std::make_shared<VerifyService>(m_spGrpcVerifyClient, m_spRedisRepository);

        // ================== Controller ==================
        m_spVerifyController = std::make_unique<VerifyController>(m_spVerifyService);


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

void GateServer::handleGetVerifyCodeAsync(const ZhKeyesIMHttp::HttpRequest& request, ZhKeyesIMHttp::HttpServer::AsyncDone done, const std::map<std::string, std::string>& params)
{
    /*
   * 请求格式：
   * {
   *   "email":"xxx@xxx.xxx"
   * }
   */
    try
    {
        auto jsonOpt = ZhKeyes::Util::JsonUtil::parseSafe(request.getBody());
        if (!jsonOpt)
        {
            HttpResponse response;
            setErrorRequest(response,
                ZhKeyesIMHttp::HttpStatusCode::BadRequest,
                ServerStatus::ErrorCodes::InternalError,
                "Invalid request JSON format");
            done(std::move(response));
            return;
        }

        auto emailOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(*jsonOpt, "email");
        if (!emailOpt || emailOpt->empty())
        {
            HttpResponse response;
            setErrorRequest(response,
                ZhKeyesIMHttp::HttpStatusCode::BadRequest,
                ServerStatus::ErrorCodes::InternalError,
                "Email field is required and cannot be empty");
            done(std::move(response));
            return;
        }

        std::string email = *emailOpt;

        // 异步gRPC调用 - 不会阻塞EventLoop线程
        m_spGrpcVerifyClient->GetVerifyCodeAsync(email,
            [done = std::move(done), this, email](const message::GetVerifyResponse& grpcResponse) mutable {
                // 这个回调在gRPC线程中执行
                HttpResponse response;

                LOG_DEBUG("Email: %s, Error code: %d", email.c_str(), grpcResponse.error());

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

                // 调用done回调，TCPConnection会自动处理线程安全
                done(std::move(response));
            });
    }
    catch (const nlohmann::json::exception& e) {
        HttpResponse response;
        LOG_ERROR("JSON parse error: %s", e.what());
        setErrorRequest(response,
            ZhKeyesIMHttp::HttpStatusCode::BadRequest,
            ServerStatus::ErrorCodes::InternalError,
            "Invalid JSON format");
        done(std::move(response));
    }
    catch (const std::exception& e) {
        HttpResponse response;
        LOG_ERROR("Exception in handleGetVerifyCodeAsync: %s", e.what());
        setErrorRequest(response,
            ZhKeyesIMHttp::HttpStatusCode::InternalServerError,
            ServerStatus::ErrorCodes::InternalError,
            e.what());
        done(std::move(response));
    }
}

void GateServer::handleUserLoginAsync(const ZhKeyesIMHttp::HttpRequest& request, ZhKeyesIMHttp::HttpServer::AsyncDone done, const std::map<std::string, std::string>& params)
{
    /*
   * 请求格式：
   * {
   *   "user":"xxx",
   *   "password" : "xxxx"
   * }
   */
    try {
        auto jsonOpt = ZhKeyes::Util::JsonUtil::parseSafe(request.getBody());
        if (!jsonOpt)
        {
            HttpResponse response;
            setErrorRequest(response,
                ZhKeyesIMHttp::HttpStatusCode::BadRequest,
                ServerStatus::ErrorCodes::InternalError,
                "Invalid request JSON format");
            done(std::move(response));
            return;
        }

        auto userOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(*jsonOpt, "user");
        auto passwordOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(*jsonOpt, "password");

        if (!userOpt || !passwordOpt)
        {
            HttpResponse response;
            setErrorRequest(response,
                ZhKeyesIMHttp::HttpStatusCode::BadRequest,
                ServerStatus::ErrorCodes::InternalError,
                "Error Json Param");
            done(std::move(response));
            return;
        }

        std::string user = *userOpt;
        std::string password = *passwordOpt;

        // 数据库操作是阻塞的，使用std::async在线程池中异步执行
        // 注意：生产环境建议使用专用线程池而不是std::async
        std::async(std::launch::async,
            [this, user, password, done = std::move(done)]() mutable {
                try {
                    UserInfo userInfo;
                    bool isPasswordValid = m_spMySqlManager->checkPassword(user, password, userInfo);

                    HttpResponse response;
                    if (!isPasswordValid)
                    {
                        setErrorRequest(response,
                            ZhKeyesIMHttp::HttpStatusCode::BadRequest,
                            ServerStatus::ErrorCodes::Error_Json,
                            "Password not matched");
                    }
                    else
                    {
                        setSuccessReqeust(response,
                            ZhKeyesIMHttp::HttpStatusCode::OK,
                            ServerStatus::ErrorCodes::Success,
                            "User logged in successfully");
                    }

                    done(std::move(response));
                }
                catch (const std::exception& e) {
                    HttpResponse response;
                    LOG_ERROR("Exception in login async task: %s", e.what());
                    setErrorRequest(response,
                        ZhKeyesIMHttp::HttpStatusCode::InternalServerError,
                        ServerStatus::ErrorCodes::InternalError,
                        e.what());
                    done(std::move(response));
                }
            });
    }
    catch (const std::exception& e) {
        HttpResponse response;
        LOG_ERROR("Exception in handleUserLoginAsync: %s", e.what());
        setErrorRequest(response,
            ZhKeyesIMHttp::HttpStatusCode::BadRequest,
            ServerStatus::ErrorCodes::InternalError,
            e.what());
        done(std::move(response));
    }

}

void GateServer::handleUserRegisterAsync(const ZhKeyesIMHttp::HttpRequest& request, ZhKeyesIMHttp::HttpServer::AsyncDone done, const std::map<std::string, std::string>& params)
{
    /*
    * 请求格式：
    * {
    *   "username":"xxx",
    *   "email" : "xxxx@xxx.com",
    *   "password" : "xxxx",
    *   "code"  : "123456"
    * }
    */
    try {
        auto jsonOpt = ZhKeyes::Util::JsonUtil::parseSafe(request.getBody());
        if (!jsonOpt)
        {
            HttpResponse response;
            setErrorRequest(response,
                ZhKeyesIMHttp::HttpStatusCode::BadRequest,
                ServerStatus::ErrorCodes::InternalError,
                "Invalid request JSON format");
            done(std::move(response));
            return;
        }

        auto usernameOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(*jsonOpt, "username");
        auto passwordOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(*jsonOpt, "password");
        auto emailOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(*jsonOpt, "email");
        auto codeOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(*jsonOpt, "code");

        if (!usernameOpt || !passwordOpt || !emailOpt || !codeOpt)
        {
            HttpResponse response;
            setErrorRequest(response,
                ZhKeyesIMHttp::HttpStatusCode::BadRequest,
                ServerStatus::ErrorCodes::InternalError,
                "Error request JSON format");
            done(std::move(response));
            return;
        }

        std::string username = *usernameOpt;
        std::string password = *passwordOpt;
        std::string email = *emailOpt;
        std::string code = *codeOpt;

        // 使用异步任务处理Redis和MySQL操作
        std::async(std::launch::async,
            [this, username, password, email, code, done = std::move(done)]() mutable {
                try {
                    // 验证Redis中的验证码
                    std::string redisKey = ServerParam::code_prefix + email;
                    std::string verifyCode;

                    bool isVerified = m_spRedisManager->get(redisKey, verifyCode);
                    if (!isVerified)
                    {
                        HttpResponse response;
                        setErrorRequest(response,
                            ZhKeyesIMHttp::HttpStatusCode::InternalServerError,
                            ServerStatus::ErrorCodes::VarifyExpired,
                            "Failed to request verification code");
                        done(std::move(response));
                        return;
                    }

                    if (code != verifyCode)
                    {
                        HttpResponse response;
                        setErrorRequest(response,
                            ZhKeyesIMHttp::HttpStatusCode::InternalServerError,
                            ServerStatus::ErrorCodes::VarifyCodeErr,
                            "Verification code error");
                        done(std::move(response));
                        return;
                    }

                    // 注册用户到MySQL
                    int uid = m_spMySqlManager->registerUser(username, email, password);

                    HttpResponse response;
                    if (uid == 0) {
                        setErrorRequest(response,
                            ZhKeyesIMHttp::HttpStatusCode::InternalServerError,
                            ServerStatus::ErrorCodes::UserExist,
                            "User Already Exist");
                    }
                    else if (uid == -1)
                    {
                        setErrorRequest(response,
                            ZhKeyesIMHttp::HttpStatusCode::InternalServerError,
                            ServerStatus::ErrorCodes::UidInvalid,
                            "User Register Error");
                    }
                    else
                    {
                        setSuccessReqeust(response,
                            ZhKeyesIMHttp::HttpStatusCode::OK,
                            ServerStatus::ErrorCodes::Success,
                            "User registered successfully");
                    }

                    done(std::move(response));
                }
                catch (const std::exception& e) {
                    HttpResponse response;
                    LOG_ERROR("Exception in register async task: %s", e.what());
                    setErrorRequest(response,
                        ZhKeyesIMHttp::HttpStatusCode::InternalServerError,
                        ServerStatus::ErrorCodes::InternalError,
                        e.what());
                    done(std::move(response));
                }
            });
    }
    catch (const std::exception& e) {
        HttpResponse response;
        LOG_ERROR("Exception in handleUserRegisterAsync: %s", e.what());
        setErrorRequest(response,
            ZhKeyesIMHttp::HttpStatusCode::BadRequest,
            ServerStatus::ErrorCodes::InternalError,
            e.what());
        done(std::move(response));
    }
}

void GateServer::handleUserResetPassAsync(const ZhKeyesIMHttp::HttpRequest& request, ZhKeyesIMHttp::HttpServer::AsyncDone done, const std::map<std::string, std::string>& params)
{
    /*
   * 请求格式：
   * {
   *   "username":"xxx",
   *   "email" : "xxxx@xxx.com",
   *   "password" : "xxxx",
   *   "code"  : "123456"
   * }
   */
    try {
        auto jsonOpt = ZhKeyes::Util::JsonUtil::parseSafe(request.getBody());
        if (!jsonOpt)
        {
            HttpResponse response;
            setErrorRequest(response,
                ZhKeyesIMHttp::HttpStatusCode::BadRequest,
                ServerStatus::ErrorCodes::InternalError,
                "Invalid request JSON format");
            done(std::move(response));
            return;
        }

        auto usernameOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(*jsonOpt, "username");
        auto passwordOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(*jsonOpt, "password");
        auto emailOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(*jsonOpt, "email");
        auto codeOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(*jsonOpt, "code");

        if (!usernameOpt || !passwordOpt || !emailOpt || !codeOpt)
        {
            HttpResponse response;
            setErrorRequest(response,
                ZhKeyesIMHttp::HttpStatusCode::BadRequest,
                ServerStatus::ErrorCodes::InternalError,
                "error request JSON format");
            done(std::move(response));
            return;
        }

        std::string username = *usernameOpt;
        std::string password = *passwordOpt;
        std::string email = *emailOpt;
        std::string code = *codeOpt;

        // 使用异步任务处理Redis和MySQL操作
        std::async(std::launch::async,
            [this, username, password, email, code, done = std::move(done)]() mutable {
                try {
                    // 验证Redis中的验证码
                    std::string redisKey = ServerParam::code_prefix + email;
                    std::string verifyCode;

                    bool isVerified = m_spRedisManager->get(redisKey, verifyCode);
                    if (!isVerified)
                    {
                        HttpResponse response;
                        setErrorRequest(response,
                            ZhKeyesIMHttp::HttpStatusCode::InternalServerError,
                            ServerStatus::ErrorCodes::VarifyExpired,
                            "Failed to request verification code");
                        done(std::move(response));
                        return;
                    }

                    if (code != verifyCode)
                    {
                        HttpResponse response;
                        setErrorRequest(response,
                            ZhKeyesIMHttp::HttpStatusCode::InternalServerError,
                            ServerStatus::ErrorCodes::VarifyCodeErr,
                            "Verification code error");
                        done(std::move(response));
                        return;
                    }

                    // 验证邮箱是否匹配
                    bool email_valid = m_spMySqlManager->checkEmail(username, email);
                    if (!email_valid)
                    {
                        HttpResponse response;
                        setErrorRequest(response,
                            ZhKeyesIMHttp::HttpStatusCode::InternalServerError,
                            ServerStatus::ErrorCodes::EmailNotMatch,
                            "email not match");
                        done(std::move(response));
                        return;
                    }

                    // 更新密码
                    bool bUpdate = m_spMySqlManager->updatePassword(username, password);

                    HttpResponse response;
                    if (!bUpdate)  // 注意：原代码逻辑可能有误，这里假设返回true表示成功
                    {
                        setErrorRequest(response,
                            ZhKeyesIMHttp::HttpStatusCode::InternalServerError,
                            ServerStatus::ErrorCodes::PasswdUpFailed,
                            "password update failed");
                    }
                    else
                    {
                        setSuccessReqeust(response,
                            ZhKeyesIMHttp::HttpStatusCode::OK,
                            ServerStatus::ErrorCodes::Success,
                            "Reset password successfully");
                    }

                    done(std::move(response));
                }
                catch (const std::exception& e) {
                    HttpResponse response;
                    LOG_ERROR("Exception in reset password async task: %s", e.what());
                    setErrorRequest(response,
                        ZhKeyesIMHttp::HttpStatusCode::InternalServerError,
                        ServerStatus::ErrorCodes::InternalError,
                        e.what());
                    done(std::move(response));
                }
            });
    }
    catch (const std::exception& e) {
        HttpResponse response;
        LOG_ERROR("Exception in handleUserResetPassAsync: %s", e.what());
        setErrorRequest(response,
            ZhKeyesIMHttp::HttpStatusCode::BadRequest,
            ServerStatus::ErrorCodes::InternalError,
            e.what());
        done(std::move(response));
    }
}


void GateServer::handleGetRoot(const ZhKeyesIMHttp::HttpRequest& request, ZhKeyesIMHttp::HttpResponse& response, const std::map<std::string, std::string>& params)
{
    response.setStatusCode(ZhKeyesIMHttp::HttpStatusCode::OK);
    response.setBody("Welcome to ZhKeyesIM Gateway");
}

void GateServer::handleGetVerifyCode(const ZhKeyesIMHttp::HttpRequest& request, ZhKeyesIMHttp::HttpResponse& response, const std::map<std::string, std::string>& params)
{
    /*
    * {
    *   "email":"xxx@xxx.xxx"
    * }
    */
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
    /*
    * {
    *   "user":"xxx",
    *   "email" : "xxxx",
    *   "password" : "xxxx",
    *   "confirm" : "xxxx",
    *   "code"  : "xxxx
    * }
    */

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

        auto userOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(*jsonOpt, "user");
        auto passwordOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(*jsonOpt, "password");

        if (!userOpt || !passwordOpt  )
        {
            setErrorRequest(response,
                ZhKeyesIMHttp::HttpStatusCode::BadRequest,
                ServerStatus::ErrorCodes::InternalError,
                "Error Json Param");
            return;
        }

        std::string user = *userOpt;
        std::string password = *passwordOpt;

        UserInfo userInfo;
        bool isPasswordValid = m_spMySqlManager->checkPassword(user, password, userInfo);
        if (!isPasswordValid)
        {
            setErrorRequest(response,
                ZhKeyesIMHttp::HttpStatusCode::BadRequest,
                ServerStatus::ErrorCodes::Error_Json,
                "Password not matched");
            return;
        }

        setSuccessReqeust(response,
            ZhKeyesIMHttp::HttpStatusCode::OK,
            ServerStatus::ErrorCodes::Success,
            "User registered successfully");

        return;
    }
    catch (const std::exception& e) {
        setErrorRequest(response,
            ZhKeyesIMHttp::HttpStatusCode::BadRequest,
            ServerStatus::ErrorCodes::InternalError,
            e.what());
        return;
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
                "Error request JSON format");
            return;
        }

        std::string username = *usernameOpt;
        std::string password = *passwordOpt;
        std::string email = *emailOpt;
        std::string code = *codeOpt;

        std::string redisKey = ServerParam::code_prefix + email;
        std::string verifyCode;

        bool isVerified = m_spRedisManager->get(redisKey, verifyCode);
        if (!isVerified)
        {
            setErrorRequest(response,
                ZhKeyesIMHttp::HttpStatusCode::InternalServerError,
                ServerStatus::ErrorCodes::VarifyExpired,
                "Failed to request verification code");
            return;
        }

        if (code != verifyCode)
        {
            setErrorRequest(response,
                ZhKeyesIMHttp::HttpStatusCode::InternalServerError,
                ServerStatus::ErrorCodes::VarifyCodeErr,
                "Error request JSON format");
            return;
        }

        //TODO验证MySQL中是否存在用户
        int uid = m_spMySqlManager->registerUser(username, email, password);
        if (uid == 0 ) {

            setErrorRequest(response,
                ZhKeyesIMHttp::HttpStatusCode::InternalServerError,
                ServerStatus::ErrorCodes::UserExist,
                "User Already Exist");
            return;
        }
        else if (uid == -1)
        {
            setErrorRequest(response,
                ZhKeyesIMHttp::HttpStatusCode::InternalServerError,
                ServerStatus::ErrorCodes::UidInvalid,
                "User Register Error");
            return;
        }

        setSuccessReqeust(response,
            ZhKeyesIMHttp::HttpStatusCode::OK,
            ServerStatus::ErrorCodes::Success,
            "User registered successfully");
        return;
    }
    catch (const std::exception& e) {
        setErrorRequest(response,
            ZhKeyesIMHttp::HttpStatusCode::BadRequest,
            ServerStatus::ErrorCodes::InternalError,
            e.what());
        return;
    }
}

void GateServer::handleUserResetPass(const ZhKeyesIMHttp::HttpRequest& request, ZhKeyesIMHttp::HttpResponse& response, const std::map<std::string, std::string>& params)
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

        std::string redisKey = ServerParam::code_prefix + email;
        std::string verifyCode;

        bool isVerified = m_spRedisManager->get(redisKey, verifyCode);
        if (!isVerified)
        {
            setErrorRequest(response,
                ZhKeyesIMHttp::HttpStatusCode::InternalServerError,
                ServerStatus::ErrorCodes::VarifyExpired,
                "Failed to request verification code");
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

        //查询 和执行MySQL
        bool email_valid = m_spMySqlManager->checkEmail(username, email);
        if (!email_valid)
        {
            setErrorRequest(response,
                ZhKeyesIMHttp::HttpStatusCode::InternalServerError,
                ServerStatus::ErrorCodes::EmailNotMatch,
                "email not match");
            return;
        }

        bool bUpdate = m_spMySqlManager->updatePassword(username, password);
        if (bUpdate)
        {
            setErrorRequest(response,
                ZhKeyesIMHttp::HttpStatusCode::InternalServerError,
                ServerStatus::ErrorCodes::PasswdUpFailed,
                "password update failed");
            return;
        }

        setSuccessReqeust(response,
            ZhKeyesIMHttp::HttpStatusCode::OK,
            ServerStatus::ErrorCodes::Success,
            "Reset password successfully");
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
        std::bind(&GateServer::handleUserRegisterAsync, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    m_router.addAsyncRoute(HttpMethod::POST, "api/user/resetPass",
        std::bind(&GateServer::handleUserResetPassAsync, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    m_router.addAsyncRoute(HttpMethod::POST, "api/user/login",
        std::bind(&GateServer::handleUserLoginAsync, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));


  /*  m_router.addRoute(ZhKeyesIMHttp::HttpMethod::GET, "/",
        std::bind(&GateServer::handleGetRoot, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    m_router.addRoute(HttpMethod::POST, "api/verify/getCode",
        std::bind(&GateServer::handleGetVerifyCode, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    m_router.addRoute(HttpMethod::POST, "api/user/register",
        std::bind(&GateServer::handleUserRegister, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    m_router.addRoute(HttpMethod::POST, "api/user/resetPass",
        std::bind(&GateServer::handleUserResetPass,this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    m_router.addRoute(HttpMethod::POST, "api/user/login",
        std::bind(&GateServer::handleUserLogin, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));*/
}
