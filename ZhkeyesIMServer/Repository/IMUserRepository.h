#ifndef ZHKEYESIMSERVER_CONTROLLER_IMUSERREPOSITORY_H_
#define ZHKEYESIMSERVER_CONTROLLER_IMUSERREPOSITORY_H_

#include <optional>
#include <string>


#include "infrastructure/RedisManager.h"
#include "infrastructure/MySqlManager.h"
#include "Model/User.h"


constexpr const char* code_prefix = "code_";
constexpr const char* token_prefix = "token_";
constexpr const char* userInfo_prefix = "userInfo_";

class IMUserRepository
{
public:
    IMUserRepository(std::shared_ptr<RedisManager> spRedis,
        std::shared_ptr<MySqlManager> spMysql);

    ~IMUserRepository() = default;

    
    std::optional<std::string> getToken(int32_t uid);

    std::optional<UserInfo> getUserInfo(int32_t uid);

private:
    std::shared_ptr<RedisManager> m_spRedis;
    std::shared_ptr<MySqlManager> m_spMysql;

private:

    IMUserRepository(const IMUserRepository&) = delete;
    IMUserRepository& operator=(const IMUserRepository&) = delete;
    IMUserRepository(IMUserRepository&&) = delete;
    IMUserRepository& operator=(IMUserRepository&&) = delete;
        
};



#endif

