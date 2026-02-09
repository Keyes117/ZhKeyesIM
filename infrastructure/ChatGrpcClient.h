#ifndef INFRASTRUCTURE_CHATGRPCCLIENT_H_
#define INFRASTRUCTURE_CHATGRPCCLIENT_H_

#include <memory>
#include <functional>
#include <thread>
#include <atomic>

#include "Logger.h"


#include "util/ConfigManager.h"
#include "grpcpp/grpcpp.h"
#include "protobuf/message.grpc.pb.h"

class ChatGrpcClient
{
public:
    ChatGrpcClient();

    ~ChatGrpcClient();

    bool init(const ZhKeyes::Util::ConfigManager& config);

private:
    void processCQ();

    //struct AsyncClientCall
    //{
    //    grpc::ClientContext context;
    //    message::GetChatServerRequest request;
    //    message::GetChatServerResponse response;
    //    grpc::Status status;
    //    GetStatusCallback callback;
    //    std::unique_ptr<grpc::ClientAsyncResponseReader<message::GetChatServerResponse>> rpc;
    //};

    std::unique_ptr<message::StatusService::Stub> m_stub;
    std::unique_ptr<grpc::CompletionQueue> m_cq;
    std::thread m_cqThread;
    std::atomic<bool>	m_running;


private:
    ChatGrpcClient(const ChatGrpcClient&) = delete;
    ChatGrpcClient& operator=(const ChatGrpcClient&) = delete;
    ChatGrpcClient(ChatGrpcClient&&) = delete;
    ChatGrpcClient& operator=(ChatGrpcClient&&) = delete;

    
};


#endif