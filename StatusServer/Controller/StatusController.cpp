#include "StatusController.h"


#include "Model/ChatServerInfo.h"

#include "Logger.h"

StatusController::StatusController(std::shared_ptr<StatusService> statusService):
    m_spStatusService(statusService)
{
}

grpc::Status StatusController::GetChatServer(grpc::ServerContext* context, const message::GetChatServerRequest* request, message::GetChatServerResponse* response)
{
    try
    {
        // 1. 提取请求参数
        int32_t uid = request->uid();

        LOG_INFO("StatusController: 收到 GetChatServer 请求, uid=%d", uid);

        // 2. 参数验证
        if (uid <= 0) {
            LOG_ERROR("StatusController: 无效的 uid: %d", uid);
            response->set_error(1);
            return grpc::Status::OK;
        }

        // 3. 调用 Service 层获取聊天服务器和 Token
        ChatServerInfo info = m_spStatusService->getChatServer(uid);

        if (info.error != 0) {
            LOG_ERROR("StatusController: 获取聊天服务器失败, uid=%d", uid);
            response->set_error(1);
            return grpc::Status::OK;
        }

        // 4. 封装响应（Token 已经包含在 info 中）
        response->set_token("0");
        response->set_error(0);
        response->set_host(info.host);
        response->set_port(std::to_string(info.port));  // 注意：protobuf 中 port 是 string

        LOG_INFO("StatusController: 成功处理请求, uid=%d, server=%s:%d",
            uid, info.host.c_str(), info.port);

        return grpc::Status::OK;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("StatusController: GetChatServer 异常: %s", e.what());
        response->set_error(1);
        return grpc::Status::OK;
    }
}
