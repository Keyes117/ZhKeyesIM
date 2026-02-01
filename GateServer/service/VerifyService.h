#ifndef GATESERVER_SERVICE_VERIFYSERVICE_H_
#define GATESERVER_SERVICE_VERIFYSERVICE_H_

#include <functional>
#include <string>

#include "model/ServiceResult.h"
#include "infrastructure/VerifyGrpcClient.h"
#include "service/AuthService.h"

class VerifyService {
public:
    using VerifyCallback = std::function<void(const VerifyCodeResult&)>;

    VerifyService(std::shared_ptr<VerifyGrpcClient> grpcClient, 
        std::shared_ptr<RedisRepository> redisRepo);
    ~VerifyService() = default;

    // 异步获取验证码
    void getVerifyCodeAsync(const std::string& email, 
        VerifyCallback callback);

    // 验证码相关
    bool verifyEmailCode(const std::string& email, const std::string& code);
    bool saveVerifyCode(const std::string& email, const std::string& code);

private:
    bool validateEmail(const std::string& email);

    void onGetVerifyCodeAsyncDone(VerifyCallback callback, const message::GetVerifyResponse& grpcResponse);

private:
    std::shared_ptr<VerifyGrpcClient> m_spGrpcClient;
    std::shared_ptr<RedisRepository> m_spRedisRepo;
};

#endif // GATESERVER_SERVICE_VERIFYSERVICE_H_