#include "UserService.h"

UserService::UserService(std::shared_ptr<UserRepository> userRepo, 
    std::shared_ptr<AuthService> authService,
    std::shared_ptr<WorkThreadPool> threadPool)
    :m_spUserRepo(userRepo),
    m_spAuthService(authService),
    m_spWorkThreadPool(threadPool)
{

}

LoginResult UserService::login(const std::string& username, 
    const std::string& password, LoginCallback callback)
{
    LOG_INFO("UserService: Processing login");

    LoginResult result;

    try
    {
        if (username.empty() || password.empty())
        {
            result = LoginResult::createFailure(
                "Username or password cannot be empty",
                ServerStatus::ErrorCodes::ParamError
            );

            callback(result);
            return;
        }

        UserInfo userInfo;
        if (!m_spUserRepo->checkPassword(username, password, userInfo))
        {

        }

        //TODO: 验证密码是否正确
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
            callback(result);
            return;
        }

        //TODO: 生成Token
        std::string token = m_spAuthService->generateToken(userInfo.uid);
        //TODO: 更新最后登录时间
        result = LoginResult::createSuccess(
            userInfo,
            token
        );
        callback(result);
       
    }
    catch (const std::exception& e)
    {
        result = LoginResult::createFailure("unexcepted error ",
            ServerStatus::ErrorCodes::ParamError);

        callback(result);
        return;
    }
}
