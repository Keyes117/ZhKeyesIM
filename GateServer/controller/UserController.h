#ifndef GATESERVER_CONTROLLER_USERCONTROLLER_H_
#define GATESERVER_CONTROLLER_USERCONTROLLER_H_

#include <map>

#include "BaseController.h"
#include "service/UserService.h"


class UserController : public BaseController {
public:
    UserController(std::shared_ptr<UserService> userService);
    ~UserController() = default;

    void handleLogin(const HttpRequest& request,
        HttpServer::AsyncDone done,
        const std::map<std::string, std::string>& params);

    void handleRegisterUser(const HttpRequest& request,
        HttpServer::AsyncDone done,
        const std::map<std::string, std::string>& params);

    void handleResetPassword(const HttpRequest& request,
        HttpServer::AsyncDone done,
        const std::map<std::string, std::string>& params);

private:
    void onHandleLoginDone(HttpServer::AsyncDone done, const LoginResult& result);

    void onHandleRegisterUserDone(HttpServer::AsyncDone done, const RegisterResult& result);

    void onHandleResetPasswordDone(HttpServer::AsyncDone done, const ResetPasswordResult& result);

private:
    std::shared_ptr<UserService> m_spUserService;
};

#endif // GATESERVER_CONTROLLER_USERCONTROLLER_H_