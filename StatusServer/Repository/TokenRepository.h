#ifndef STATUSSERVER_REPOSITORY_TOKENREPOSITORY_H_
#define STATUSSERVER_REPOSITORY_TOKENREPOSITORY_H_

#include <string>
#include <optional>
#include <memory>

#include "infrastructure/RedisManager.h"

class TokenRepository
{
public:
    explicit TokenRepository(std::shared_ptr<RedisManager> redis);
    ~TokenRepository() = default;

    /**
     * @brief 获取token
     * @param uid 用户id
     * @return optional<Token>
     */
    std::optional<std::string> getToken(uint32_t uid);

    /**
     * @brief 验证Token是否有效
     * @param uid 用户id
     * @param token 
     * @return 结果
     */
    bool validate(uint32_t uid, const std::string& token);

    /**
     * @brief 删除用户token(登出)
     * @param uid 用户id
     * @return 
     */
    bool remove(uint32_t uid);

private:
    std::shared_ptr<RedisManager> m_spRedis;

    std::string generateKey(uint32_t uid);

    static constexpr const char* TOKEN_PREFIX = "token_";
};





#endif