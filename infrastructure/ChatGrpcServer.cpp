#include "ChatGrpcServer.h"

#include "Logger.h"

// ==================== ChatServiceImpl ====================

grpc::Status ChatServiceImpl::ForwardMessage(grpc::ServerContext* context,
    const message::ForwardMessageRequest* request,
    message::ForwardMessageResponse* response)
{
    if (!m_forwardHandler)
    {
        response->set_error(1);
        response->set_error_msg("No forward handler registered");
        return grpc::Status::OK;
    }

    uint32_t targetUid = static_cast<uint32_t>(request->target_uid());
    const std::string& messageData = request->message_data();

    bool success = m_forwardHandler(targetUid, messageData);

    if (success)
    {
        response->set_error(0);
    }
    else
    {
        response->set_error(2);
        response->set_error_msg("Target user not found on this server");
    }

    return grpc::Status::OK;
}

// ==================== ChatGrpcServer ====================

ChatGrpcServer::ChatGrpcServer()
{
}

ChatGrpcServer::~ChatGrpcServer()
{
    shutdown();
}

bool ChatGrpcServer::start(const std::string& ip, int32_t port,
    ChatServiceImpl::ForwardHandler handler)
{
    m_service.setForwardHandler(std::move(handler));

    std::string serverAddress = ip + ":" + std::to_string(port);

    grpc::ServerBuilder builder;
    builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
    builder.RegisterService(&m_service);

    m_server = builder.BuildAndStart();
    if (!m_server)
    {
        LOG_ERROR("ChatGrpcServer: Failed to start on %s", serverAddress.c_str());
        return false;
    }

    LOG_INFO("ChatGrpcServer: Listening on %s", serverAddress.c_str());

    // 在独立线程中运行 gRPC 服务器
    m_serverThread = std::thread([this]() {
        m_server->Wait();
        });

    return true;
}

void ChatGrpcServer::shutdown()
{
    if (m_server)
    {
        m_server->Shutdown();
    }
    if (m_serverThread.joinable())
    {
        m_serverThread.join();
    }
}