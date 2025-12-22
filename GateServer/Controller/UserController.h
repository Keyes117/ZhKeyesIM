#ifndef GATESERVER_CONTROLLER_USERCONTROLLER_H_
#define GATESERVER_CONTROLLER_USERCONTROLLER_H_

#include <map>

#include "BaseController.h"
#include "service/UserService.h"
#include "WorkThreadPool.h"


class UserController : public BaseController {
public:
    UserController(UserService* userService, WorkThreadPool* threadPool);
    ~UserController() = default;

    void login(const HttpRequest& request,
        HttpServer::AsyncDone done,
        const std::map<std::string, std::string>& params);

    void registerUser(const HttpRequest& request,
        HttpServer::AsyncDone done,
        const std::map<std::string, std::string>& params);

    void resetPassword(const HttpRequest& request,
        HttpServer::AsyncDone done,
        const std::map<std::string, std::string>& params);

private:
    UserService* m_userService;
    WorkThreadPool* m_threadPool;
};

#endif // GATESERVER_CONTROLLER_USERCONTROLLER_H_