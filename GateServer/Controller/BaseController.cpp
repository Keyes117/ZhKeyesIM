#include "BaseController.h"

void BaseController::sendSuccess(HttpServer::AsyncDone& done,
    HttpStatusCode httpCode,
    ServerStatus::ErrorCodes errorCode,
    const std::string& message)
{
    try
    {
        nlohmann::json responseJson = {
            {"success", 1},
            {"code" , static_cast<int>(errorCode)},
            {"msg", message},
            {"timestamp", std::time(nullptr)}
        };


        HttpResponse response;
        response.setStatusCode(httpCode);
        response.setJsonResponse(responseJson.dump());

        done(std::move(response));
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("VerifyController: sendSuccess Error %s", e.what());

        nlohmann::json errorJson = {
            {"success", 0},
            {"code" , static_cast<int>(ServerStatus::ErrorCodes::InternalError)},
            {"msg", message},
            {"timestamp", std::time(nullptr)}
        };

        HttpResponse response;
        response.setStatusCode(HttpStatusCode::InternalServerError);
        response.setJsonResponse(errorJson);

        done(std::move(response));
    }
}

void BaseController::sendError(HttpServer::AsyncDone& done,
    HttpStatusCode httpCode, 
    ServerStatus::ErrorCodes errorCode,
    const std::string& message)
{
    try
    {
        nlohmann::json responseJson = {
            {"success", 1},
            {"code" , static_cast<int>(errorCode)},
            {"msg", message},
            {"timestamp", std::time(nullptr)}
        };


        HttpResponse response;
        response.setStatusCode(httpCode);
        response.setJsonResponse(responseJson.dump());

        done(std::move(response));
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("VerifyController: sendSuccess Error %s", e.what());

        nlohmann::json errorJson = {
            {"success", 0},
            {"code" , static_cast<int>(ServerStatus::ErrorCodes::InternalError)},
            {"msg", message},
            {"timestamp", std::time(nullptr)}
        };

        HttpResponse response;
        response.setStatusCode(HttpStatusCode::InternalServerError);
        response.setJsonResponse(errorJson);

        done(std::move(response));
    }

}

bool BaseController::validateRequired(const nlohmann::json& json, const std::vector<std::string>& fields, std::string& errorMsg)
{
    return false;
}
