#include "StatusService.h"

StatusService::StatusService(std::shared_ptr<ChatServerRepository> chatServerRepo, 
    std::shared_ptr<TokenRepository> tokenRepo) :
    m_spChatServerRepo(chatServerRepo),
    m_spTokenRepo(tokenRepo)
{
}

ChatServerInfo StatusService::getChatServer(int32_t uid)
{
    ChatServerInfo info;
    info.error = 0;

    try {
        LOG_INFO("StatusService: 为用户 %d 分配聊天服务器", uid);

        // 1. 从 Repository 获取连接数最少的服务器（负载均衡）
        auto serverOpt = m_spChatServerRepo->getServerWithMinConnections();

        if (!serverOpt.has_value()) {
            LOG_ERROR("StatusService: 没有可用的聊天服务器");
            info.error = 1;
            return info;
        }

        info = serverOpt.value();

        // 验证服务器信息是否完整
        if (info.host.empty() || info.port == 0) {
            LOG_ERROR("StatusService: 服务器信息不完整, name=%s", info.name.c_str());
            info.error = 1;
            return info;
        }


        LOG_INFO("StatusService: 成功为用户 %d 分配服务器 %s (%s:%d)",
            uid, info.name.c_str(), info.host.c_str(), info.port);

        return info;
    }
    catch (const std::exception& e) {
        LOG_ERROR("StatusService: getChatServer 异常: %s", e.what());
        info.error = 1;
        return info;
    }
}

bool StatusService::validateToken(int32_t uid, const std::string& token)
{
    try {
        return m_spTokenRepo->validate(uid, token);
    }
    catch (const std::exception& e) {
        LOG_ERROR("StatusService: validateToken 异常: %s", e.what());
        return false;
    }
}

std::string StatusService::getToken(int32_t uid)
{
    try {
        auto tokenOpt = m_spTokenRepo->getToken(uid);
        if (!tokenOpt)
            return "";

        std::string token = *tokenOpt;
        return token;
    }
    catch (const std::exception& e) {
        LOG_ERROR("StatusService: validateToken 异常: %s", e.what());
        return false;
    }
}
