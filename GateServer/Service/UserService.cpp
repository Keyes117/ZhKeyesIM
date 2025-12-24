#include "UserService.h"

#include "const.h"

UserService::UserService(std::shared_ptr<UserRepository> userRepo,
    std::shared_ptr<RedisRepository> redisRepo,
    std::shared_ptr<AuthService> authService,
    std::shared_ptr<WorkThreadPool> threadPool)
    :m_spUserRepo(userRepo),
    m_spAuthService(authService),
    m_spWorkThreadPool(threadPool)
{

}

void UserService::login(const std::string& username, 
    const std::string& password, LoginCallback callback)
{
    LOG_INFO("UserService: Processing login");

    LoginResult result;

    ServerUtil::Defer def([this, callback, &result]() {
        callback(result);
        });

    try
    {
        if (username.empty() || password.empty())
        {
            result = LoginResult::createFailure(
                "Username or password cannot be empty",
                ServerStatus::ErrorCodes::ParamError
            );

            //callback(result);
            return;
        }

        auto userInfoOpt = m_spUserRepo->findByUsername(username);
        if (!userInfoOpt)
        {
            result = LoginResult::createFailure(
                "User not Fount", ServerStatus::ErrorCodes::UserExist
            );
        }

        UserInfo userInfo = *userInfoOpt;

        // 验证密码是否正确
        bool passwordValid = m_spAuthService->verifyPassword(
            password,
            userInfo.passwordHash
        );

        if (!passwordValid)
        {
            LOG_WARN("UserService: Invalid password for user: %s", username.c_str());
            result = LoginResult::createFailure(
                "Invalid password",
                ServerStatus::ErrorCodes::PasswdErr
            );
            //callback(result);
            return;
        }

        //生成Token
        std::string token = m_spAuthService->generateToken(userInfo.uid);

        if (token.empty())
        {
            LOG_ERROR("UserService: Failed to generate token for user: %s", username.c_str());
            result = LoginResult::createFailure(
                "Failed to generate authentication token",
                ServerStatus::ErrorCodes::InternalError
            );
            //callback(result);
            return;
        }

        //修改 最近一次登录时间 
        bool timeUpdated = m_spUserRepo->updateLastLoginTime(userInfo.uid);
        if (!timeUpdated)
        {
            LOG_WARN("UserService: Failed to update last login time");
        }


        //TODO: 更新最后登录时间
        result = LoginResult::createSuccess(
            userInfo,
            token
        );
        //callback(result);
       
    }
    catch (const std::exception& e)
    {
        result = LoginResult::createFailure("unexcepted error ",
            ServerStatus::ErrorCodes::ParamError);

        //callback(result);
        return;
    }
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
        result.code = ServerStatus::ErrorCodes::Success;
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


