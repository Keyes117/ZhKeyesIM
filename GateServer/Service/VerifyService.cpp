#include "VerifyService.h"

#include <regex>

VerifyService::VerifyService(std::shared_ptr<VerifyGrpcClient> grpcClient,
    std::shared_ptr<RedisRepository> redisRepo):
    m_spGrpcClient(grpcClient),
    m_spRedisRepo(redisRepo)
{
}
 
void VerifyService::getVerifyCodeAsync(const std::string& email, AsyncCallback callback)
{
    LOG_INFO("VerifyService: Processing verify code request for %s", email.c_str());
    VerifyCodeResult result;
    try
    {
        if (!validateEmail(email))
        {
            result = VerifyCodeResult::createFailure("unvalidate email format",
                ServerStatus::ErrorCodes::ParamError);
            callback(result);
            return;
        }

        m_spGrpcClient->GetVerifyCodeAsync(email,
            std::bind(&VerifyService::onGetVerifyCodeAsyncDone,
                this, std::move(callback), std::placeholders::_1)
        );
    }
    catch (std::exception& e)
    {
        result = VerifyCodeResult::createFailure("unexcepted error ",
            ServerStatus::ErrorCodes::ParamError);

        callback(result);
        return;
    }

   
}

bool VerifyService::validateEmail(const std::string& email)
{
    if (email.empty())
        return false;

    std::regex emailPattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, emailPattern);
}

void VerifyService::onGetVerifyCodeAsyncDone(AsyncCallback callback, const message::GetVerifyResponse& grpcResponse)
{
    VerifyCodeResult result;
    ServerStatus::GrpcErrors grpcStatus =
        static_cast<ServerStatus::GrpcErrors>(grpcResponse.error());

    if (grpcStatus == ServerStatus::GrpcErrors::Success)
    {
        result = VerifyCodeResult::createSuccess();
        callback(result);
    }
    else
    {
        result = VerifyCodeResult::createFailure("failed to send verification code !",
            ServerStatus::ErrorCodes::RPCFailed);
        callback(result);
    }
}
