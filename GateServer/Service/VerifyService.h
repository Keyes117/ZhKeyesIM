#ifndef GATESERVER_SERVICE_VERIFYSERVICE_H_
#define GATESERVER_SERVICE_VERIFYSERVICE_H_

#include <functional>
#include <string>

#include "model/ServiceResult.h"
#include "infrastructure/VerifyGrpcClient.h"
#include "service/AuthService.h"

class VerifyService {
public:
    using AsyncCallback = std::function<void(const VerifyCodeResult&)>;

    VerifyService(std::shared_ptr<VerifyGrpcClient> grpcClient, 
        std::shared_ptr<RedisRepository> redisRepo);
    ~VerifyService() = default;

    // 异步获取验证码
    void getVerifyCodeAsync(const std::string& email, AsyncCallback callback);

private:
    bool validateEmail(const std::string& email);

    void onGetVerifyCodeAsyncDone(AsyncCallback callback, const message::GetVerifyResponse& grpcResponse);

private:
    std::shared_ptr<VerifyGrpcClient> m_spGrpcClient;
    std::shared_ptr<RedisRepository> m_spRedisRepo;
};

#endif // GATESERVER_SERVICE_VERIFYSERVICE_H_