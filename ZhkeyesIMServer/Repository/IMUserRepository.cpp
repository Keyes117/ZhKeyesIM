#include "IMUserRepository.h"

#include "util/JsonUtil.h"
#include "util/Defer.h"

IMUserRepository::IMUserRepository(std::shared_ptr<RedisManager> spRedis
    , std::shared_ptr<MySqlManager> spMysql) :
    m_spRedis(spRedis),
    m_spMysql(spMysql)
{
}

std::optional<std::string> IMUserRepository::getToken(int32_t uid)
{
    if (!m_spRedis)
        return std::nullopt;

    std::string strUidKey = token_prefix + std::to_string(uid);
    std::string token;
    if (!m_spRedis->Get(strUidKey, token))
        return std::nullopt;

    return token;
}

std::optional<UserInfo> IMUserRepository::getUserInfo(int32_t uid)
{
    std::string infoKey = userInfo_prefix + std::to_string(uid);
    std::string infoString;
    if (m_spRedis && m_spRedis->Get(infoKey, infoString))
    {
        auto infoJson = ZhKeyes::Util::JsonUtil::parseSafe(infoString);
        if (!infoJson)
            return std::nullopt;

        // 反序列化为 UserInfo
        UserInfo userInfo;

        if (auto uidOpt = ZhKeyes::Util::JsonUtil::getSafe<uint32_t>(*infoJson, "uid"))
            userInfo.uid = *uidOpt;
        else
            return std::nullopt; // uid 是必需字段

        if (auto nameOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(*infoJson, "name"))
            userInfo.name = *nameOpt;

        if (auto emailOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(*infoJson, "email"))
            userInfo.email = *emailOpt;

        if (auto nickOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(*infoJson, "nick"))
            userInfo.nick = *nickOpt;

        if (auto descOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(*infoJson, "desc"))
            userInfo.desc = *descOpt;

        if (auto sexOpt = ZhKeyes::Util::JsonUtil::getSafe<uint32_t>(*infoJson, "sex"))
            userInfo.sex = *sexOpt;

        if (auto iconOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(*infoJson, "icon"))
            userInfo.icon = *iconOpt;

        if (auto backOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(*infoJson, "back"))
            userInfo.back = *backOpt;

        return userInfo;
    }
    else if(m_spMysql)
    {
        

        //如果Redis 中没有缓存，则去数据库中查询
        auto spConn = m_spMysql->getConnection();
        if (!spConn)
            return std::nullopt;

        ZhKeyes::Util::Defer def([this, &spConn]() {
            m_spMysql->returnConnection(spConn);
            });

        try
        {
            auto spStmt = m_spMysql->prepareStatement(spConn,
                "SELECT uid, name, email, pwd, nick, desc, sex, icon, back "
                "FROM user WHERE uid = ?"
            );
            spStmt->setInt(1, uid);
            std::unique_ptr<sql::ResultSet> res(spStmt->executeQuery());

            if (res->next())
            {
                UserInfo userInfo;
                userInfo.uid = res->getInt("uid");
                userInfo.name = res->getString("name");
                userInfo.email = res->getString("email");
                userInfo.pwd = res->getString("pwd");

                // 处理可能为 NULL 的字段
                if (!res->isNull("nick"))
                    userInfo.nick = res->getString("nick");
                if (!res->isNull("desc"))
                    userInfo.desc = res->getString("desc");
                if (!res->isNull("sex"))
                    userInfo.sex = res->getUInt("sex");
                if (!res->isNull("icon"))
                    userInfo.icon = res->getString("icon");
                if (!res->isNull("back"))
                    userInfo.back = res->getString("back");

                // 3. 将查询结果写入缓存（可选，提高后续查询性能）
                if (m_spRedis)
                {
                    nlohmann::json infoJson;
                    infoJson["uid"] = userInfo.uid;
                    infoJson["name"] = userInfo.name;
                    infoJson["email"] = userInfo.email;
                    infoJson["pwd"] = userInfo.pwd;
                    infoJson["nick"] = userInfo.nick;
                    infoJson["desc"] = userInfo.desc;
                    infoJson["sex"] = userInfo.sex;
                    infoJson["icon"] = userInfo.icon;
                    infoJson["back"] = userInfo.back;

                    std::string cacheValue = infoJson.dump();
                    m_spRedis->Set(infoKey, cacheValue);
                    // 可以设置过期时间，比如 1 小时
                     //m_spRedis->Expire(infoKey, 3600);
                }

                return userInfo;
            }
        }
        catch (sql::SQLException& e)
        {
            LOG_ERROR("IMUserRepository: getUserInfo failed: %s", e.what());
            return std::nullopt;
        }
    }

    return std::nullopt;
}

bool IMUserRepository::setUserServerMapping(int32_t uid, const std::string& serverName, const std::string& serverIp, int32_t serverPort, const std::string& sessionId)
{
    if(!m_spRedis)
        return false;

    try {
        // 1. 设置用户->服务器映射（使用 sessionId 作为唯一标识）
        std::string userServerKey = std::string(user_server_prefix) +
            std::to_string(uid) + ":" + sessionId;

        int64_t connectTime = std::time(nullptr);

        bool success = true;
        success = success && m_spRedis->HSet(userServerKey, "server_name", serverName);
        success = success && m_spRedis->HSet(userServerKey, "server_ip", serverIp);
        success = success && m_spRedis->HSet(userServerKey, "server_port", std::to_string(serverPort));
        success = success && m_spRedis->HSet(userServerKey, "connect_time", std::to_string(connectTime));
        success = success && m_spRedis->HSet(userServerKey, "session_id", sessionId);

        // 设置过期时间（24小时）
        if (success) {
            m_spRedis->Expire(userServerKey, 24 * 3600);
        }

        // 2. 设置默认查询键（用于快速查找用户所在的任意一个服务器）
        std::string defaultKey = std::string(user_server_prefix) + std::to_string(uid);
        m_spRedis->HSet(defaultKey, "server_name", serverName);
        m_spRedis->HSet(defaultKey, "server_ip", serverIp);
        m_spRedis->HSet(defaultKey, "server_port", std::to_string(serverPort));
        m_spRedis->HSet(defaultKey, "session_id", sessionId);
        m_spRedis->HSet(defaultKey, "connect_time", std::to_string(connectTime));
        m_spRedis->Expire(defaultKey, 24 * 3600);

        return success;
    }
    catch (const std::exception& e) {
        LOG_ERROR("IMUserRepository: setUserServerMapping exception: %s", e.what());
        return false;
    }
}

std::optional<ServerInfo> IMUserRepository::getUserServerMapping(int32_t uid)
{
    if (!m_spRedis)
        return std::nullopt;

    try {
        std::string key = std::string(user_server_prefix) + std::to_string(uid);

        std::string serverName = m_spRedis->HGet(key, "server_name");
        if (serverName.empty()) {
            return std::nullopt;
        }

        std::string serverIp = m_spRedis->HGet(key, "server_ip");
        std::string serverPortStr = m_spRedis->HGet(key, "server_port");
        std::string sessionId = m_spRedis->HGet(key, "session_id");
        std::string connectTimeStr = m_spRedis->HGet(key, "connect_time");

        ServerInfo info;
        info.serverName = serverName;
        info.serverIp = serverIp;
        info.sessionId = sessionId;

        try {
            info.serverPort = std::stoi(serverPortStr);
            info.connectTime = std::stoll(connectTimeStr);
        }
        catch (...) {
            LOG_ERROR("IMUserRepository: Invalid data for uid=%d", uid);
            return std::nullopt;
        }

        return info;
    }
    catch (const std::exception& e) {
        LOG_ERROR("IMUserRepository: getUserServerMapping exception: %s", e.what());
        return std::nullopt;
    }
}

bool IMUserRepository::removeUserSessionMapping(int32_t uid, const std::string& sessionId)
{
    if (!m_spRedis)
        return false;

    try {
        std::string userServerKey = std::string(user_server_prefix) +
            std::to_string(uid) + ":" + sessionId;
        m_spRedis->del(userServerKey);

        // 如果这是默认键中的会话，清空默认键
        std::string defaultKey = std::string(user_server_prefix) + std::to_string(uid);
        std::string defaultSessionId = m_spRedis->HGet(defaultKey, "session_id");

        if (defaultSessionId == sessionId) {
            m_spRedis->del(defaultKey);
        }

        return true;
    }
    catch (const std::exception& e) {
        LOG_ERROR("IMUserRepository: removeUserSessionMapping exception: %s", e.what());
        return false;
    }
}

bool IMUserRepository::incrementServerConnectionCount(const std::string& serverName)
{
    if (!m_spRedis)
        return false;

    // 使用 Redis HINCRBY 原子操作
    // Key: login_count, Field: serverName, Value: 连接数
    return m_spRedis->HIncrBy(login_count_hash_key, serverName, 1);
}

bool IMUserRepository::decrementServerConnectionCount(const std::string& serverName)
{
    if (!m_spRedis)
        return false;

    // 原子减少
    bool success = m_spRedis->HIncrBy(login_count_hash_key, serverName, -1);

    if (success) {
        // 检查连接数是否小于 0，如果是则设为 0（防止负数）
        auto count = getServerConnectionCount(serverName);
        if (count.has_value() && count.value() < 0) {
            m_spRedis->HSet(login_count_hash_key, serverName, "0");
        }
    }

    return success;
}

std::optional<int> IMUserRepository::getServerConnectionCount(const std::string& serverName)
{
    if (!m_spRedis)
        return std::nullopt;

    std::string countStr = m_spRedis->HGet(login_count_hash_key, serverName);

    if (countStr.empty()) {
        return std::nullopt;
    }

    try {
        return std::stoi(countStr);
    }
    catch (const std::exception& e) {
        LOG_ERROR("IMUserRepository: 解析连接数失败: %s", e.what());
        return std::nullopt;
    }
}
