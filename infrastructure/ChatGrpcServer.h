#ifndef INFRASTRUCTURE_CHATGRPCSERVER_H_
#define INFRASTRUCTURE_CHATGRPCSERVER_H_

#include <memory>
#include <functional>
#include <thread>
#include <string>

#include "grpcpp/grpcpp.h"
#include "protobuf/message.grpc.pb.h"

// gRPC 服务实现
class ChatServiceImpl final : public message::ChatService::Service
{
public:
    // 收到转发消息时的处理回调
    using ForwardHandler = std::function<bool(uint32_t targetUid, const std::string& messageData)>;

    void setForwardHandler(ForwardHandler handler) { m_forwardHandler = std::move(handler); }

    grpc::Status ForwardMessage(grpc::ServerContext* context,
        const message::ForwardMessageRequest* request,
        message::ForwardMessageResponse* response) override;

private:
    ForwardHandler m_forwardHandler;
};

// gRPC 服务器管理类
class ChatGrpcServer
{
public:
    ChatGrpcServer();
    ~ChatGrpcServer();

    /**
     * @brief 启动 gRPC 服务器
     * @param ip 监听IP
     * @param port gRPC 端口
     * @param handler 消息转发处理回调
     */
    bool start(const std::string& ip, int32_t port,
        ChatServiceImpl::ForwardHandler handler);

    void shutdown();

private:
    ChatServiceImpl m_service;
    std::unique_ptr<grpc::Server> m_server;
    std::thread m_serverThread;
};


#endif