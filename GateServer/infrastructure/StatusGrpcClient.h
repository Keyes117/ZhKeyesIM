#ifndef GATESERVER_STATUSGRPCCLIENT_H_
#define GATESERVER_STATUSGRPCCLIENT_H_

#include <memory>
#include <functional>
#include <thread>
#include <atomic>

#include "Logger.h"

#include "const.h"
#include "ConfigManager.h"
#include "grpcpp/grpcpp.h"
#include "message.grpc.pb.h"


using GetStatusCallback = std::function<void(const message::GetChatServerResponse&)>;

class StatusGrpcClient
{
public:
    StatusGrpcClient();

    ~StatusGrpcClient();

    bool init(const ZhKeyes::Util::ConfigManager& config);

    void GetChatStatus(int uid, GetStatusCallback&& callback);

private:


    void processCQ();

    struct AsyncClientCall
    {
        grpc::ClientContext context;
        message::GetChatServerRequest request;
        message::GetChatServerResponse response;
        grpc::Status status;
        GetStatusCallback callback;
        std::unique_ptr<grpc::ClientAsyncResponseReader<message::GetChatServerResponse>> rpc;
    };



    std::unique_ptr<message::StatusService::Stub> m_stub;
    std::unique_ptr<grpc::CompletionQueue> m_cq;
    std::thread m_cqThread;
    std::atomic<bool>	m_running;


private:
    StatusGrpcClient(const StatusGrpcClient&) = delete;
    StatusGrpcClient& operator=(const StatusGrpcClient&) = delete;
    StatusGrpcClient(StatusGrpcClient&&) = delete;
    StatusGrpcClient& operator=(StatusGrpcClient&&) = delete;
};

#endif //!GATESERVER_STATUSGRPCCLIENT_H_