#ifndef INFRASTRUCTURE_CHATGRPCCLIENT_H_
#define INFRASTRUCTURE_CHATGRPCCLIENT_H_

#include <memory>
#include <string>
#include <mutex>
#include <unordered_map>

#include "Logger.h"
#include "grpcpp/grpcpp.h"
#include "protobuf/message.grpc.pb.h"

class ChatGrpcClient
{
public:
    ChatGrpcClient();
    ~ChatGrpcClient();

    /**
     * @brief 将消息转发到另一台 IMServer
     * @param serverIp 目标服务器IP
     * @param grpcPort 目标服务器gRPC端口
     * @param targetUid 目标用户UID
     * @param messageData 序列化后的 IMMessage 数据
     * @return true 转发成功
     */
    bool forwardMessage(const std::string& serverIp, int32_t grpcPort,
        uint32_t targetUid, const std::string& messageData);

private:
    /**
     * @brief 获取或创建到目标服务器的 gRPC Stub（带连接缓存）
     */
    std::shared_ptr<message::ChatService::Stub> getOrCreateStub(
        const std::string& serverIp, int32_t grpcPort);

    std::mutex m_stubMutex;
    // key: "ip:port" → stub
    std::unordered_map<std::string, std::shared_ptr<message::ChatService::Stub>> m_stubs;

private:
    ChatGrpcClient(const ChatGrpcClient&) = delete;
    ChatGrpcClient& operator=(const ChatGrpcClient&) = delete;
    ChatGrpcClient(ChatGrpcClient&&) = delete;
    ChatGrpcClient& operator=(ChatGrpcClient&&) = delete;
};

#endif