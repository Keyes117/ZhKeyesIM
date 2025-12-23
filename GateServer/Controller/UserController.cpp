#include "UserController.h"

UserController::UserController(std::shared_ptr<UserService> userService)
    :m_spUserService(userService)
{
}

void UserController::handleLogin(const HttpRequest& request,
    HttpServer::AsyncDone done,
    const std::map<std::string, std::string>& params)
{
    LOG_INFO("UserController: Received user login request");

    std::string msg = "";
    try
    {
        auto jsonOpt = ZhKeyes::Util::JsonUtil::parseSafe(request.getBody());
        if (!jsonOpt)
        {
            msg = "Invalid JSON format";
            LOG_WARN("UserController: request error : [ %s ]", msg);

            sendError(done, HttpStatusCode::BadRequest,
                ServerStatus::ErrorCodes::InternalError,
                msg);
            return;
        }

        auto userOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(*jsonOpt, "user");
        auto passwordOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(*jsonOpt, "password");

        if(!userOpt || !passwordOpt)
        {
            msg = "Error Json Param";
            LOG_WARN("UserController: param error : [ %s ]", msg);

            sendError(done, HttpStatusCode::BadRequest,
                ServerStatus::ErrorCodes::InternalError,
                msg);
            return;
        }

        std::string username = *userOpt;
        std::string password = *passwordOpt;

        m_spUserService->login(username, password,
            std::bind(&UserController::onHandleLoginDone, this, done, std::placeholders::_1)
        );
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("UserController: Exception : [ %s ]", e.what());
        msg = "Exception Occured " + std::string(e.what());
        sendError(done, HttpStatusCode::InternalServerError,
            ServerStatus::ErrorCodes::InternalError, msg);
        return;
    }
}

void UserController::handleRegisterUser(const HttpRequest& request, HttpServer::AsyncDone done, const std::map<std::string, std::string>& params)
{
}

void UserController::handleResetPassword(const HttpRequest& request, HttpServer::AsyncDone done, const std::map<std::string, std::string>& params)
{
}

void UserController::onHandleLoginDone(HttpServer::AsyncDone done, const LoginResult& result)
{
    if (result.success)
    {
        sendSuccess(done, HttpStatusCode::OK, result.code, result.message);
    }
    else
    {
        sendError(done, HttpStatusCode::InternalServerError, result.code, result.message);
    }
}

void UserController::onHandleRegisterUserDone(HttpServer::AsyncDone done, const LoginResult& result)
{
    if (result.success)
    {
        sendSuccess(done, HttpStatusCode::OK, result.code, result.message);
    }
    else
    {
        sendError(done, HttpStatusCode::InternalServerError, result.code, result.message);
    }
}

void UserController::onHandleResetPasswordDone(HttpServer::AsyncDone done, const LoginResult& result)
{
    if (result.success)
    {
        sendSuccess(done, HttpStatusCode::OK, result.code, result.message);
    }
    else
    {
        sendError(done, HttpStatusCode::InternalServerError, result.code, result.message);
    }
}
