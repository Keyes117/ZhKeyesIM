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

void UserService::registerUser(const std::string& username, const std::string& email, const std::string& password, const std::string& verifyCode)
{
}
