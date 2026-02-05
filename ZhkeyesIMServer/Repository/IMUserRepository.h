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
constexpr const char* user_server_prefix = "user_server:";
constexpr const char* login_count_hash_key = "login_count";
//constexpr const char* 

struct ServerInfo {
    std::string serverName;
    std::string serverIp;
    int32_t serverPort;
    int64_t connectTime;
    std::string sessionId;
};

class IMUserRepository
{
public:
    IMUserRepository(std::shared_ptr<RedisManager> spRedis,
        std::shared_ptr<MySqlManager> spMysql);

    ~IMUserRepository() = default;
    
    std::optional<std::string> getToken(int32_t uid);

    std::optional<UserInfo> getUserInfo(int32_t uid);

    bool setUserServerMapping(int32_t uid, const std::string& serverName,
        const std::string& serverIp, int32_t serverPort,
        const std::string& sessionId);

    /**
     * @brief 获取用户所在的服务器信息
     * @param uid 用户ID
     * @return 服务器信息，如果不存在返回 nullopt
     */
    std::optional<ServerInfo> getUserServerMapping(int32_t uid);

    /**
     * @brief 删除用户与服务器的映射
     * @param uid 用户ID
     * @param sessionId 会话ID
     * @return true 成功，false 失败
     */
    bool removeUserSessionMapping(int32_t uid, const std::string& sessionId);

    // ========== 服务器连接数相关 ==========
    /**
     * @brief 增加服务器连接数
     * @param serverName 服务器名称
     * @return true 成功，false 失败
     */
    bool incrementServerConnectionCount(const std::string& serverName);

    /**
     * @brief 减少服务器连接数
     * @param serverName 服务器名称
     * @return true 成功，false 失败
     */
    bool decrementServerConnectionCount(const std::string& serverName);

    /**
     * @brief 获取服务器连接数（可选，用于监控）
     * @param serverName 服务器名称
     * @return 连接数，如果不存在返回 nullopt
     */
    std::optional<int> getServerConnectionCount(const std::string& serverName);

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

