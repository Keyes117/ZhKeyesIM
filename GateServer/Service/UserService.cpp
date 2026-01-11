#include "UserService.h"

#include "const.h"
#include "Defer.h"

UserService::UserService(std::shared_ptr<UserRepository> userRepo,
    std::shared_ptr<RedisRepository> redisRepo,
    std::shared_ptr<AuthService> authService,
    std::shared_ptr<StatusGrpcClient> grpcStatusClient,
    std::shared_ptr<WorkThreadPool> threadPool)
    :m_spUserRepo(userRepo),
    m_spRedisRepo(redisRepo),
    m_spAuthService(authService),
    m_spGrpcStatusClient(grpcStatusClient),
    m_spWorkThreadPool(threadPool)
{

}

void UserService::login(const std::string& email, 
    const std::string& password, LoginCallback callback)
{
    LOG_INFO("UserService: Processing login");

    m_spWorkThreadPool->enqueue([this, email, password, callback]() {

        LoginResult result;

        try
        {
            // 1. 参数验证
            if (email.empty() || password.empty())
            {
                result = LoginResult::createFailure(
                    "Username or password cannot be empty",
                    ServerStatus::ErrorCodes::ParamError
                );
                callback(result);
                return;
            }

            // 2. 查询用户（数据库操作，在工作线程中执行）
            auto userInfoOpt = m_spUserRepo->findByEmail(email);
            if (!userInfoOpt)
            {
                result = LoginResult::createFailure(
                    "User not Found",
                    ServerStatus::ErrorCodes::UserNotFound
                );
                callback(result);
                return;
            }

            UserInfo userInfo = *userInfoOpt;

            // 3. 验证密码（CPU密集型操作）
            bool passwordValid = m_spAuthService->verifyPassword(
                password,
                userInfo.passwordHash
            );

            if (!passwordValid)
            {
                LOG_WARN("UserService: Invalid password for email: %s", email.c_str());
                result = LoginResult::createFailure(
                    "Invalid password",
                    ServerStatus::ErrorCodes::PasswdErr
                );
                callback(result);
                return;
            }

            //查询是否登录

            // 4. 生成Token
            std::string token = m_spAuthService->generateToken(userInfo.uid);
            if (token.empty())
            {
                LOG_ERROR("UserService: Failed to generate token for email: %s", email.c_str());
                result = LoginResult::createFailure(
                    "Failed to generate authentication token",
                    ServerStatus::ErrorCodes::InternalError
                );
                callback(result);
                return;
            }

            m_spRedisRepo->saveToken(userInfo.uid, token);

            // 5. 更新登录时间（异步执行，不阻塞主流程）
            bool timeUpdated = m_spUserRepo->updateLastLoginTime(userInfo.uid);
            if (!timeUpdated)
            {
                LOG_WARN("UserService: Failed to update last login time");
                result = LoginResult::createFailure(
                    "UserService: Failed to update last login time",
                    ServerStatus::ErrorCodes::InternalError
                );
                callback(result);
            }

            // 6. 异步获取聊天服务器地址
            // 注意：这里从工作线程池的线程中发起gRPC异步调用
            m_spGrpcStatusClient->GetChatStatus(userInfo.uid,
                [callback, userInfo, token](const message::GetChatServerResponse& response) {

                    LoginResult finalResult;

                    // 检查gRPC响应
                    if (response.error() != 0) {
                        LOG_ERROR("UserService: Failed to get chat server, error: %d",
                            response.error());
                        finalResult = LoginResult::createFailure(
                            "Failed to get chat server address",
                            static_cast<ServerStatus::ErrorCodes>(response.error())
                        );
                        callback(finalResult);
                        return;
                    }

                    // 解析聊天服务器地址
                    std::string chatServerIp = response.host();
                    int chatServerPort = 0;
                    try {
                        chatServerPort = std::stoi(response.port());
                    }
                    catch (...) {
                        LOG_ERROR("UserService: Invalid port in response");
                        finalResult = LoginResult::createFailure(
                            "Invalid chat server port",
                            ServerStatus::ErrorCodes::InternalError
                        );
                        callback(finalResult);
                        return;
                    }

                    LOG_INFO("UserService: Login successful - uid: %d, chat: %s:%d",
                        userInfo.uid, chatServerIp.c_str(), chatServerPort);

                    // 创建成功结果
                    finalResult = LoginResult::createSuccess(
                        userInfo,
                        token,
                        chatServerIp,
                        chatServerPort
                    );

                    callback(finalResult);
                }
            );
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("UserService: Exception during login: %s", e.what());
            LoginResult result = LoginResult::createFailure(
                "Internal server error",
                ServerStatus::ErrorCodes::InternalError
            );
            callback(result);
        }
        });
}

void UserService::registerUser(const std::string& username, 
    const std::string& email, const std::string& password,
    const std::string& verifyCode, RegisterCallback callback)
{
    
    RegisterResult result;

    try
    {
        // 1. 参数验证
        if (username.empty() || password.empty() || email.empty() || verifyCode.empty())
        {
            result = RegisterResult::createFailure(
                "All fields are required",
                ServerStatus::ErrorCodes::ParamError
            );
            callback(result);
            return;
        }

        // 2. 从Redis获取验证码
        auto storedCodeOpt = m_spRedisRepo->getVerifyCode(email);
        if (!storedCodeOpt)
        {
            LOG_WARN("UserService: Verification code expired or not found for email: %s", email.c_str());
            result = RegisterResult::createFailure(
                "Verification code expired or not found",
                ServerStatus::ErrorCodes::VarifyExpired
            );
            callback(result);
            return;
        }

        // 3. 验证验证码是否正确
        std::string storedCode = *storedCodeOpt;
        if (verifyCode != storedCode)
        {
            LOG_WARN("UserService: Invalid verification code for email: %s", email.c_str());
            result = RegisterResult::createFailure(
                "Invalid verification code",
                ServerStatus::ErrorCodes::VarifyCodeErr
            );
            callback(result);
            return;
        }

        // 5. 检查邮箱是否已存在
        if (m_spUserRepo->exsitsByEmail(email))
        {
            LOG_WARN("UserService: Email already exists: %s", email.c_str());
            result = RegisterResult::createFailure(
                "Email already exists",
                ServerStatus::ErrorCodes::UserExist
            );
            callback(result);
            return;
        }

        // 6. 对密码进行哈希处理
        std::string passwordHash = m_spAuthService->hashPassword(password);
        if (passwordHash.empty())
        {
            LOG_ERROR("UserService: Failed to hash password");
            result = RegisterResult::createFailure(
                "Failed to process password",
                ServerStatus::ErrorCodes::InternalError
            );
            callback(result);
            return;
        }

        // 7. 创建用户
        int uid = m_spUserRepo->create(username, email, passwordHash);
        if (uid <= 0)
        {
            LOG_ERROR("UserService: Failed to create user: %s", username.c_str());
            result = RegisterResult::createFailure(
                "Failed to create user account",
                ServerStatus::ErrorCodes::InternalError
            );
            callback(result);
            return;
        }

        // 8. 删除已使用的验证码
        m_spRedisRepo->deleteVerifyCode(email);

        // 9. 注册成功
        LOG_INFO("UserService: User registered successfully - uid: %d, username: %s", uid, username.c_str());
        result = RegisterResult::createSuccess(uid);
        callback(result);

    }
    catch (const std::exception& e)
    {
        LOG_ERROR("UserService: Exception during registration: %s", e.what());
        result = RegisterResult::createFailure(
            "Internal server error",
            ServerStatus::ErrorCodes::InternalError
        );
        callback(result);
    }
}

void UserService::resetPassword(const std::string& email, 
    const std::string& newPassword,
    const std::string& verifyCode, 
    ResetPasswordCallback callback)
{
    //1 从Redis获取验证码
    //2 验证验证码
    //3 从Mysql查询Email
    //4 用户存在、修改密码， 用户不存在则返回

    ResetPasswordResult result;

    ZhKeyes::Util::Defer def([this, callback, &result]() {
        callback(result);
        });

    try
    {
        // 1. 参数验证
        if (email.empty() || newPassword.empty() ||  verifyCode.empty())
        {
            result = ResetPasswordResult::createFailure(
                "All fields are required",
                ServerStatus::ErrorCodes::ParamError
            );
            return;
        }

        // 2. 从Redis获取验证码
        auto storedCodeOpt = m_spRedisRepo->getVerifyCode(email);
        if (!storedCodeOpt)
        {
            LOG_WARN("UserService: Verification code expired or not found for email: %s", email.c_str());
            result = ResetPasswordResult::createFailure(
                "Verification code expired or not found",
                ServerStatus::ErrorCodes::VarifyExpired
            );
            return;
        }

        // 3. 验证验证码是否正确
        std::string storedCode = *storedCodeOpt;
        if (verifyCode != storedCode)
        {
            LOG_WARN("UserService: Invalid verification code for email: %s", email.c_str());
            result = ResetPasswordResult::createFailure(
                "Invalid verification code",
                ServerStatus::ErrorCodes::VarifyCodeErr
            );
            return;
        }


        auto userInfoOpt = m_spUserRepo->findByEmail(email);
        // 5. 检查用户是否注册
        if (!userInfoOpt)
        {
            LOG_WARN("UserService: user is not exists: %s", email.c_str());
            result = ResetPasswordResult::createFailure(
                "The User is not Registered",
                ServerStatus::ErrorCodes::UserNotFound
            );
            return;
        }

        UserInfo userInfo = *userInfoOpt;
        

        if (m_spAuthService->verifyPassword(newPassword, userInfo.passwordHash))
        {
            LOG_WARN("UserService: newPass is the same as previous", email.c_str());
            result = ResetPasswordResult::createFailure(
                "newPass is the same as previous",
                ServerStatus::ErrorCodes::NewPassIsSame
            );
            return;
        }

        std::string newPassHash = m_spAuthService->hashPassword(newPassword);

        if (newPassHash.empty())
        {
            LOG_WARN("UserService: resetPassword failed ");
            result = ResetPasswordResult::createFailure(
                "resetPassword failed",
                ServerStatus::ErrorCodes::ResetPassFailed
            );
            callback(result);
            return;
        }

        if (!m_spUserRepo->updatePassword(email, newPassHash))
        {
            LOG_WARN("UserService: resetPassword failed ");
            result = ResetPasswordResult::createFailure(
                "resetPassword failed",
                ServerStatus::ErrorCodes::ResetPassFailed
            );
            callback(result);
            return;
        }

        // 8. 删除已使用的验证码
        m_spRedisRepo->deleteVerifyCode(email);

        // 9. 注册成功
        LOG_INFO("UserService: User resetPass successfully - email: %s", email);
        result = ResetPasswordResult::createSuccess();
        callback(result);

    }
    catch (const std::exception& e)
    {
        LOG_ERROR("UserService: Exception during registration: %s", e.what());
        result = ResetPasswordResult::createFailure(
            "Internal server error",
            ServerStatus::ErrorCodes::InternalError
        );
    }

}


