#include "RedisRepository.h"

#include "const.h"

RedisRepository::RedisRepository(std::shared_ptr<RedisManager> spRedis)
    :m_spRedis(spRedis)
{
}

bool RedisRepository::saveVerifyCode(const std::string& email, 
                                    const std::string& code, 
                                    int expireSeconds)
{
    return false;
}

std::optional<std::string> RedisRepository::getVerifyCode(const std::string& email)
{
    std::string key = ServerParam::code_prefix + email;
    std::string value;

    bool success = m_spRedis->get(key, value);
    if (success && !value.empty())
    {
        return value;
    }

    return std::nullopt;
}

bool RedisRepository::deleteVerifyCode(const std::string& email)
{
    std::string key = ServerParam::code_prefix + email;
    return m_spRedis->del(key);
}

bool RedisRepository::saveToken(int uid, const std::string& token, int expireSeconds)
{
    return false;
}

std::optional<std::string> RedisRepository::getToken(int uid)
{
    return std::optional<std::string>();
}

bool RedisRepository::deleteToken(int uid)
{
    return false;
}
