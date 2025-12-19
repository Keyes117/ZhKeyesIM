#ifndef GATESERVER_SERVICE_USERSERVICE_H_
#define GATESERVER_SERVICE_USERSERVICE_H_

#include "../model/User.h"
#include "../model/ServiceResult.h"
#include "../repository/UserRepository.h"
#include "AuthService.h"

class UserService {
public:
    UserService(UserRepository* userRepo, AuthService* authService);
    ~UserService() = default;

    // 用户业务逻辑
    LoginResult login(const std::string& username, const std::string& password);
    
    RegisterResult registerUser(const std::string& username,
                               const std::string& email,
                               const std::string& password,
                               const std::string& verifyCode);
    
    ResetPasswordResult resetPassword(const std::string& username,
                                     const std::string& email,
                                     const std::string& newPassword,
                                     const std::string& verifyCode);

private:
    UserRepository* m_userRepo;
    AuthService* m_authService;
};

#endif // GATESERVER_SERVICE_USERSERVICE_H_