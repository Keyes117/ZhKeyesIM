#ifndef STATUSSERVER_CHATSERVERREPOSITORY_H_
#define STATUSSERVER_CHATSERVERREPOSITORY_H_


#include <string>
#include <vector>
#include <optional>
#include <memory>
#include <climits>

#include "Infrastructure/RedisManager.h"
#include "Model/ChatServerInfo.h"

class ChatServerRepository
{
public:
    explicit ChatServerRepository(std::shared_ptr<RedisManager> redis);
    ~ChatServerRepository() = default;


    /**
     * @brief 获取连接数最少的服务器
     * @return 服务器信息
     */
    std::optional<ChatServerInfo> getServerWithMinConnections();

    /**
     * @brief 增加服务器连接数
     * @param servername 服务器地址
     * @return true 增加成功 false 失败
     */
    bool incrementConnectionCount(const std::string& servername);

    /**
     * @brief 减少服务器连接数
     * @param servername 服务器地址
     * @return true 减少成功 false 失败
     */
    bool decrementConnectionCount(const std::string& servername);
    
    /**
     * @brief 获取服务器连接数
     * @param serverName 
     * @return 连接数
     */
    std::optional<int> getConnectionCount(const std::string& serverName);

    /**
     * @brief  设置服务器连接数（初始化或重置）
     * @param serverName 
     * @param count 
     * @return 
     */
    bool setConnectionCount(const std::string& serverName, int count);

    // ========== 服务器管理 ==========

    /**
     * @brief 获取所有服务器名称列表
     * @return 
     */
    std::vector<std::string> getAllServerNames();

    /**
     * @brief 获取服务器详细信息
     * @param serverName 服务器名称
     * @return 服务器信息
     */
    ChatServerInfo getServerInfo(const std::string& serverName);


    /**
     * @brief 添加服务器到列表
     * @param serverName 服务器名称
     * @return 
     */
    bool addServer(const std::string& serverName);

   
    /**
     * @brief 移除服务器
     * @param serverName 
     * @return 
     */
    bool removeServer(const std::string& serverName);

private:
    std::shared_ptr<RedisManager> m_spRedis;


    // Redis Key 常量
    static constexpr const char* CHAT_SERVERS_LIST_KEY = "chat_servers";
    static constexpr const char* LOGIN_COUNT_HASH_KEY = "login_count";
    static constexpr const char* SERVER_INFO_PREFIX = "server:";

    // 生成服务器信息 Key
    std::string getServerInfoKey(const std::string& serverName);
};

#endif