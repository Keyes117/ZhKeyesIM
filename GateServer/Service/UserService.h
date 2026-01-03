#ifndef GATESERVER_SERVICE_USERSERVICE_H_
#define GATESERVER_SERVICE_USERSERVICE_H_

#include <functional>

#include "model/User.h"
#include "model/ServiceResult.h"
#include "repository/UserRepository.h"
#include "service/AuthService.h"
#include "infrastructure/StatusGrpcClient.h"

#include "WorkThreadPool.h"

class UserService {
public:

    using LoginCallback = std::function<void(const LoginResult&)>;
    using RegisterCallback = std::function<void(const RegisterResult&)>;
    using ResetPasswordCallback = std::function<void(const ResetPasswordResult&)>;

    UserService(std::shared_ptr<UserRepository> userRepo,
        std::shared_ptr<RedisRepository> redisRepo,
        std::shared_ptr<AuthService> authService,
        std::shared_ptr<StatusGrpcClient> grpcStatusClient,
        std::shared_ptr<WorkThreadPool> threadPool);

    ~UserService() = default;

    // 用户业务逻辑
    void login(const std::string& email, 
                    const std::string& password,
                    LoginCallback callback
    );
    
    void registerUser(const std::string& username,
                               const std::string& email,
                               const std::string& password,
                               const std::string& verifyCode,
                               RegisterCallback callback
    );
    
    void resetPassword(const std::string& email,
                               const std::string& newPassword,
                               const std::string& verifyCode,
                               ResetPasswordCallback callback    
    );

private:
    std::shared_ptr<UserRepository>         m_spUserRepo;
    std::shared_ptr<RedisRepository>        m_spRedisRepo;
    std::shared_ptr<AuthService>            m_spAuthService;
    std::shared_ptr<StatusGrpcClient>       m_spGrpcStatusClient;
    std::shared_ptr< WorkThreadPool>        m_spWorkThreadPool;
};

#endif // GATESERVER_SERVICE_USERSERVICE_H_