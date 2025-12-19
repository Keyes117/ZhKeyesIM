#ifndef GATESERVER_REPOSITORY_REDISREPOSITORY_H_
#define GATESERVER_REPOSITORY_REDISREPOSITORY_H_

#include <string>
#include "infrastructure/RedisManager.h"

class RedisRepository {
public:
    explicit RedisRepository(RedisManager* redis);
    ~RedisRepository() = default;

    // 验证码相关
    bool saveVerifyCode(const std::string& email, const std::string& code, int expireSeconds = 300);
    std::optional<std::string> getVerifyCode(const std::string& email);
    bool deleteVerifyCode(const std::string& email);
    
    // Token相关
    bool saveToken(int uid, const std::string& token, int expireSeconds = 7200);
    std::optional<std::string> getToken(int uid);
    bool deleteToken(int uid);

private:
    RedisManager* m_redis;
};

#endif // GATESERVER_REPOSITORY_REDISREPOSITORY_H_