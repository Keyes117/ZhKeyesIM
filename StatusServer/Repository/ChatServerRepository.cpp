#include "ChatServerRepository.h"

ChatServerRepository::ChatServerRepository(std::shared_ptr<RedisManager> redis):
    m_spRedis(redis)
{
}

std::optional<ChatServerInfo> ChatServerRepository::getServerWithMinConnections()
{
    try
    {
        std::vector<std::string> serverNames = getAllServerNames();

        if (serverNames.empty())
        {
            LOG_ERROR("ChatServerRepository: 没有可用的服务器");
            return std::nullopt;
        }

        std::string minServerName;
        int minCount = INT_MAX;

        for (const auto& serverName : serverNames)
        {
            std::string countStr = m_spRedis->HGet(LOGIN_COUNT_HASH_KEY, serverName);

            int count = INT_MAX;

            if (!countStr.empty())
            {
                try
                {
                    count = std::stoi(countStr);
                }
                catch (const std::exception& e)
                {
                    LOG_WARN("ChatServerRepository: 解析连接数失败 %s: %s",
                        serverName.c_str(), e.what());
                    count = INT_MAX;
                }
            }
            else
            {
                // 如果不存在，初始化为 0
                count = 0;
                setConnectionCount(serverName, 0);
            }

            // 找出最小值
            if (count < minCount) {
                minCount = count;
                minServerName = serverName;
            }

            LOG_DEBUG("ChatServerRepository: 服务器 %s 连接数: %d",
                serverName.c_str(), count);
        }
        if (minServerName.empty()) {
            LOG_ERROR("ChatServerRepository: 未找到可用服务器");
            return std::nullopt;
        }

        LOG_INFO("ChatServerRepository: 选择服务器 %s (连接数: %d)",
            minServerName.c_str(), minCount);

        // 3. 增加该服务器的连接数（原子操作）
        if (!incrementConnectionCount(minServerName)) {
            LOG_ERROR("ChatServerRepository: 增加连接数失败: %s", minServerName.c_str());
            return std::nullopt;
        }

        // 4. 获取服务器详细信息
        ChatServerInfo info = getServerInfo(minServerName);

        if (info.error != 0) {
            LOG_ERROR("ChatServerRepository: 获取服务器信息失败: %s",
                minServerName.c_str());
            // 回滚连接数
            decrementConnectionCount(minServerName);
            return std::nullopt;
        }

        LOG_INFO("ChatServerRepository: 成功分配服务器 %s (%s: %" PRIu16"), 新连接数: %d",
            minServerName.c_str(), info.host.c_str(), info.port, minCount + 1);

        return info;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("ChatServerRepository: getServerWithMinConnections 异常: %s", e.what());
        return std::nullopt;
    }
}

bool ChatServerRepository::incrementConnectionCount(const std::string& serverName)
{
    return m_spRedis->HIncrBy(LOGIN_COUNT_HASH_KEY, serverName, 1);
}

bool ChatServerRepository::decrementConnectionCount(const std::string& serverName)
{
    bool success = m_spRedis->HIncrBy(LOGIN_COUNT_HASH_KEY, serverName, -1);

    if (success) {
        // 检查连接数是否小于 0，如果是则设为 0
        auto count = getConnectionCount(serverName);
        if (count.has_value() && count.value() < 0) {
            setConnectionCount(serverName, 0);
        }
    }

    return success;
}

std::optional<int> ChatServerRepository::getConnectionCount(const std::string& serverName)
{
    std::string countStr = m_spRedis->HGet(LOGIN_COUNT_HASH_KEY, serverName);

    if (countStr.empty()) {
        return std::nullopt;
    }

    try {
        return std::stoi(countStr);
    }
    catch (const std::exception& e) {
        LOG_ERROR("ChatServerRepository: 解析连接数失败: %s", e.what());
        return std::nullopt;
    }
}

bool ChatServerRepository::setConnectionCount(const std::string& serverName, int count)
{
    std::string countStr = std::to_string(count);
    return m_spRedis->HSet(LOGIN_COUNT_HASH_KEY, serverName, countStr);
}

std::vector<std::string> ChatServerRepository::getAllServerNames()
{
    return m_spRedis->LRange(CHAT_SERVERS_LIST_KEY, 0, -1);
}

ChatServerInfo ChatServerRepository::getServerInfo(const std::string& serverName)
{
    ChatServerInfo info;
    info.error = 1;

    std::string serverKey = getServerInfoKey(serverName);

    // 从 Redis Hash 获取服务器信息
    std::string host = m_spRedis->HGet(serverKey, "host");
    std::string port = m_spRedis->HGet(serverKey, "port");
    std::string name = m_spRedis->HGet(serverKey, "name");

    if (host.empty() || port.empty()) {
        LOG_ERROR("ChatServerRepository: 服务器信息不完整: %s", serverName.c_str());
        return info;
    }

    info.host = host;
    info.port = std::stoi(port);
    info.name = name.empty() ? serverName : name;
    info.error = 0;

    return info;
}

bool ChatServerRepository::addServer(const std::string& serverName)
{
    return m_spRedis->LPush(CHAT_SERVERS_LIST_KEY, serverName);
}

bool ChatServerRepository::removeServer(const std::string& serverName)
{
    return false;
}

std::string ChatServerRepository::getServerInfoKey(const std::string& serverName)
{
    return std::string(SERVER_INFO_PREFIX) + serverName;
}
