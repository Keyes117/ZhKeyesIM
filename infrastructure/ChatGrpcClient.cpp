#include "ChatGrpcClient.h"

ChatGrpcClient::ChatGrpcClient()
{
}

ChatGrpcClient::~ChatGrpcClient()
{
}

bool ChatGrpcClient::forwardMessage(const std::string& serverIp, int32_t grpcPort,
    uint32_t targetUid, const std::string& messageData)
{
    auto stub = getOrCreateStub(serverIp, grpcPort);
    if (!stub)
    {
        LOG_ERROR("ChatGrpcClient: Failed to get stub for %s:%d", serverIp.c_str(), grpcPort);
        return false;
    }

    grpc::ClientContext context;
    // 设置超时 3 秒
    context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(3));

    message::ForwardMessageRequest request;
    request.set_target_uid(static_cast<int32_t>(targetUid));
    request.set_message_data(messageData);

    message::ForwardMessageResponse response;

    grpc::Status status = stub->ForwardMessage(&context, request, &response);

    if (!status.ok())
    {
        LOG_ERROR("ChatGrpcClient: ForwardMessage RPC failed: %s", status.error_message().c_str());
        return false;
    }

    if (response.error() != 0)
    {
        LOG_WARN("ChatGrpcClient: ForwardMessage error=%d msg=%s",
            response.error(), response.error_msg().c_str());
        return false;
    }

    LOG_INFO("ChatGrpcClient: ForwardMessage success, targetUid=%u -> %s:%d",
        targetUid, serverIp.c_str(), grpcPort);
    return true;
}

std::shared_ptr<message::ChatService::Stub> ChatGrpcClient::getOrCreateStub(
    const std::string& serverIp, int32_t grpcPort)
{
    std::string key = serverIp + ":" + std::to_string(grpcPort);

    {
        std::lock_guard<std::mutex> lock(m_stubMutex);
        auto it = m_stubs.find(key);
        if (it != m_stubs.end())
        {
            return it->second;
        }
    }

    // 创建新连接
    auto channel = grpc::CreateChannel(key, grpc::InsecureChannelCredentials());
    auto stub = std::make_shared<message::ChatService::Stub>(channel);

    {
        std::lock_guard<std::mutex> lock(m_stubMutex);
        m_stubs[key] = stub;
    }

    LOG_INFO("ChatGrpcClient: Created new stub for %s", key.c_str());
    return stub;
}