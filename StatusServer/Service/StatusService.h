#ifndef STATUSSERVER_SERVICE_STATUSSERVICE_H_
#define STATUSSERVER_SERVICE_STATUSSERVICE_H_

#include <string>
#include <memory>

#include "Repository/ChatServerRepository.h"
#include "Repository/TokenRepository.h"
#include "Model/ChatServerInfo.h"

class StatusService
{
public:
    StatusService(std::shared_ptr< ChatServerRepository> chatServerRepo,
        std::shared_ptr<TokenRepository> tokenRepo);
    ~StatusService() = default;


    // 为用户分配聊天服务器（带负载均衡）
    // 同时从 Redis 获取用户的 Token（由 GateServer 生成）
    ChatServerInfo getChatServer(int32_t uid);

    // 验证用户 Token（可选，用于额外的验证）
    bool validateToken(int32_t uid, const std::string& token);

    std::string getToken(int32_t uid);

private:
    std::shared_ptr<ChatServerRepository> m_spChatServerRepo;
    std::shared_ptr<TokenRepository> m_spTokenRepo;
};


#endif