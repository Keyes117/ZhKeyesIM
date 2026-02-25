#include "RedisRepository.h"

#include <optional>

#include "const.h"

RedisRepository::RedisRepository(std::shared_ptr<RedisManager> spRedis)
    :m_spRedis(spRedis)
{
}

bool RedisRepository::saveVerifyCode(const std::string& email, 
                                    const std::string& code, 
                                    int expireSeconds)
{
    if (!m_spRedis)
        return false;

    // key: code_<email>
    std::string key = std::string(ServerParam::code_prefix) + email;

    // 当前 RedisManager 只有 Set，没有带过期时间的接口，这里先忽略 expireSeconds
    return m_spRedis->Set(key, code);
}

std::optional<std::string> RedisRepository::getVerifyCode(const std::string& email)
{
    std::string key = ServerParam::code_prefix + email;
    std::string value;

    bool success = m_spRedis && m_spRedis->Get(key, value);
    if (success && !value.empty())
    {
        return value;
    }

    return std::nullopt;
}

bool RedisRepository::deleteVerifyCode(const std::string& email)
{
    if (!m_spRedis)
        return false;

    std::string key = ServerParam::code_prefix + email;
    return m_spRedis->del(key);
}

bool RedisRepository::saveToken(int uid, const std::string& token, int expireSeconds)
{
    if (!m_spRedis)
        return false;

    // key: token_<uid>
    std::string key = std::string(ServerParam::token_prefix) + std::to_string(uid);

    // 同样，当前没有带过期时间的接口，先忽略 expireSeconds
    return m_spRedis->Set(key, token);
}

std::optional<std::string> RedisRepository::getToken(int uid)
{
    if (!m_spRedis)
        return std::nullopt;

    std::string key = std::string(ServerParam::token_prefix) + std::to_string(uid);
    std::string value;

    bool success = m_spRedis->Get(key, value);
    if (success && !value.empty())
    {
        return value;
    }

    return std::nullopt;
}

bool RedisRepository::deleteToken(int uid)
{
    if (!m_spRedis)
        return false;

    std::string key = std::string(ServerParam::token_prefix) + std::to_string(uid);
    return m_spRedis->del(key);
}
