#include "TokenRepository.h"

TokenRepository::TokenRepository(std::shared_ptr<RedisManager> redis)
    :m_spRedis(redis)
{
}

std::optional<std::string> TokenRepository::getToken(uint32_t uid)
{
    std::string key = generateKey(uid);
    std::string token;


    if (!m_spRedis->Get(key, token)) {
        LOG_DEBUG("TokenRepository: 获取 Token 失败, uid=%d (可能未登录或 Token 已过期)", uid);
        return std::nullopt;
    }

    if (token.empty()) {
        LOG_WARN("TokenRepository: Token 为空, uid=%d", uid);
        return std::nullopt;
    }

    LOG_DEBUG("TokenRepository: 成功获取 Token, uid=%d", uid);
    return token;
}

bool TokenRepository::validate(uint32_t uid, const std::string& token)
{
    auto storedToken = getToken(uid);

    if (!storedToken.has_value()) {
        LOG_WARN("TokenRepository: Token 不存在, uid=%d", uid);
        return false;
    }

    bool isValid = storedToken.value() == token;

    if (!isValid) {
        LOG_WARN("TokenRepository: Token 验证失败, uid=%d (Token 不匹配)", uid);
    }
    else {
        LOG_DEBUG("TokenRepository: Token 验证成功, uid=%d", uid);
    }

    return isValid;
}

bool TokenRepository::remove(uint32_t uid)
{
    std::string key = generateKey(uid);

    if (!m_spRedis->del(key)) {
        LOG_ERROR("TokenRepository: 删除 Token 失败, uid=%d", uid);
        return false;
    }

    LOG_INFO("TokenRepository: 成功删除 Token, uid=%d", uid);
    return true;
}

std::string TokenRepository::generateKey(uint32_t uid)
{
    return std::string(TOKEN_PREFIX) + std::to_string(uid);
}
