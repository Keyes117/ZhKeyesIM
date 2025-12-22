#include "VerifyController.h"


#include "const.h"
#include "JsonUtil.h"
#include "Logger.h"

VerifyController::VerifyController(std::shared_ptr<VerifyService> verifyService)
    : m_spVerifyService(verifyService)
{
}

void VerifyController::handleGetVerifyCode(const HttpRequest& request,
    HttpServer::AsyncDone done, const std::map<std::string, std::string>& params)
{
    LOG_INFO("VerifyController: Received verify code request");

    std::string msg = "";
    try
    {
        auto jsonOpt = ZhKeyes::Util::JsonUtil::parseSafe(request.getBody());
        if (!jsonOpt)
        {
            msg = "Invalid JSON format";
            LOG_WARN("VerifyController: %s", msg);

            sendError(done, HttpStatusCode::BadRequest,
                ServerStatus::ErrorCodes::InternalError,
                msg);
            return;
        }

        auto emailOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(*jsonOpt, "email");
        if (!emailOpt || emailOpt->empty())
        {
            msg = "Missing Email Fields";

            LOG_WARN("VerifyController: %s ", msg);

            sendError(done, HttpStatusCode::BadRequest,
                ServerStatus::ErrorCodes::InternalError,
                msg);
            return;
        }

        std::string email = *emailOpt;

        m_spVerifyService->getVerifyCodeAsync(
            email, std::bind(&VerifyController::onHandlerGetVerifyCodeDone, this, done, std::placeholders::_1)
        );
    }
    catch (const std::exception& e)
    {

    }
}

void VerifyController::onHandlerGetVerifyCodeDone(HttpServer::AsyncDone done,
    const VerifyCodeResult& result)
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
