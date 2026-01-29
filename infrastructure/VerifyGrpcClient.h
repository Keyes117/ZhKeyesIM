#ifndef infrastructure_VERIFYGRPCCLIENT_H_
#define infrastructure_VERIFYGRPCCLIENT_H_

#include <memory>
#include <functional>
#include <thread>
#include <atomic>


#include "Logger.h"

#include "const.h"
#include "ConfigManager.h"
#include "grpcpp/grpcpp.h"
#include "message.grpc.pb.h"



using GetVerifyCodeCallback = std::function<void(const message::GetVerifyResponse&)>;

class VerifyGrpcClient
{
public:
    VerifyGrpcClient();

    ~VerifyGrpcClient();

    bool init(const ZhKeyes::Util::ConfigManager& config);

    void GetVerifyCode(const std::string& email, GetVerifyCodeCallback&& callback);

private:


    void processCQ();

    struct AsyncClientCall
    {
        grpc::ClientContext context;
        message::GetVerifyRequest request;
        message::GetVerifyResponse response;
        grpc::Status status;
        GetVerifyCodeCallback callback;
        std::unique_ptr<grpc::ClientAsyncResponseReader<message::GetVerifyResponse>> rpc;
    };

    std::unique_ptr<message::VerifyService::Stub> m_stub;
    std::unique_ptr<grpc::CompletionQueue> m_cq;
    std::thread m_cqThread;
    std::atomic<bool>	m_running;


private:
    VerifyGrpcClient(const VerifyGrpcClient&) = delete;
    VerifyGrpcClient& operator=(const VerifyGrpcClient&) = delete;
    VerifyGrpcClient(VerifyGrpcClient&&) = delete;
    VerifyGrpcClient& operator=(VerifyGrpcClient&&) = delete;
};


#endif