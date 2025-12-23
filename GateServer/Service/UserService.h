#ifndef GATESERVER_SERVICE_USERSERVICE_H_
#define GATESERVER_SERVICE_USERSERVICE_H_

#include "model/User.h"
#include "model/ServiceResult.h"
#include "repository/UserRepository.h"
#include "service/AuthService.h"


#include "WorkThreadPool.h"

class UserService {
public:

    using LoginCallback = std::function<void(const LoginResult&)>;

    UserService(std::shared_ptr<UserRepository> userRepo,
        std::shared_ptr<AuthService> authService,
        std::shared_ptr<WorkThreadPool> threadPool);

    ~UserService() = default;

    // 用户业务逻辑
    LoginResult login(const std::string& username, 
                    const std::string& password,
                    LoginCallback callback
    );
    
    RegisterResult registerUser(const std::string& username,
                               const std::string& email,
                               const std::string& password,
                               const std::string& verifyCode);
    
    ResetPasswordResult resetPassword(const std::string& username,
                                     const std::string& email,
                                     const std::string& newPassword,
                                     const std::string& verifyCode);

private:
    std::shared_ptr<UserRepository> m_spUserRepo;
    std::shared_ptr<AuthService> m_spAuthService;
    std::shared_ptr< WorkThreadPool> m_spWorkThreadPool;
};

#endif // GATESERVER_SERVICE_USERSERVICE_H_