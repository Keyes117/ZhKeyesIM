#include "RedisManager.h"


#include "Logger.h"

RedisManager::RedisManager()
{
}

RedisManager::~RedisManager()
{
    close();
}

bool RedisManager::init(const ConfigManager& config)
{
    auto hostOpt = config.getSafe<std::string>({ "redis","host" });
    auto portOpt = config.getSafe<std::string>({ "redis","port" });
    auto passwordOpt = config.getSafe<std::string>({ "redis","password" });

    if (!hostOpt || !portOpt || !passwordOpt)
    {
        return false;
    }

    std::string host = *hostOpt;
    std::string port = *portOpt;
    std::string password = *passwordOpt;
    m_spConnPool = std::make_unique<RedisConnPool>(10, host.c_str(),
        std::atoi(port.c_str()), password.c_str());
    m_inited = true;

    return true;
}

bool RedisManager::get(const std::string& key, std::string& value)
{
    if (!m_inited)
        return false;

    redisContext* pConnection = m_spConnPool->getConnection();
    if (pConnection == nullptr)
    {
        LOG_ERROR("Redis: Failed to get redis Connection");
        return false;
    }

    redisReply* pReply = static_cast<redisReply*>(redisCommand(pConnection, "GET %s", key.c_str()));
    if (!pReply)
    {
        LOG_ERROR("Redis: failed to execute command [ Get %s ]", key.c_str());
        freeReplyObject(pReply);
        return false;
    }

    if (pReply->type != REDIS_REPLY_STRING)
    {
        LOG_ERROR("Redis: failed to execute command [ Get %s ]", key.c_str());
        freeReplyObject(pReply);
        return false;
    }

    value = pReply->str;
    freeReplyObject(pReply);

    LOG_INFO("Redis: Succeed to execute command [ Get %s ]", key.c_str());
    return true;

}

bool RedisManager::set(const std::string& key, const std::string& value)
{
    if (!m_inited)
        return false;

    redisContext* pConnection = m_spConnPool->getConnection();
    if (pConnection == nullptr)
    {
        LOG_ERROR("Redis: Failed to get redis Connection");
        return false;
    }

    redisReply* pReply = static_cast<redisReply*>(redisCommand(
        pConnection, "SET %s %s", key.c_str(), value.c_str())
        );

    if (!pReply)
    {
        LOG_ERROR("Redis: failed to execute command [Set %s %s ]", key.c_str(), value.c_str());
        freeReplyObject(pReply);
        return false;
    }

    //如果执行失败则释放连接
    if (!(pReply->type == REDIS_REPLY_STATUS && (strcmp(pReply->str, "OK") == 0 
        || strcmp(pReply->str, "ok") == 0)))
    {
        LOG_ERROR("Redis: failed to execute command [ SET %s %s ]", key.c_str(), value.c_str());
        freeReplyObject(pReply);
        return false;
    }

    //执行成功
    freeReplyObject(pReply);
    LOG_INFO("Redis: Succeed to execute command [ SET %s %s]", key.c_str(), value.c_str());
    return true;
}

bool RedisManager::auth( const std::string& password)
{
    if (!m_inited)
        return false;

    redisContext* pConnection = m_spConnPool->getConnection();
    if (pConnection == nullptr)
    {
        LOG_ERROR("Redis: Failed to get redis Connection");
        return false;
    }

    redisReply* pReply = static_cast<redisReply*>(redisCommand(
        pConnection, "AUTH %s", password.c_str())
    );

    if (pReply->type == REDIS_REPLY_ERROR)
    {
        LOG_ERROR("redis: failed to authorization ");
        freeReplyObject(pReply);
        return false;
    }
    else
    {
        freeReplyObject(pReply);
        LOG_INFO("Redis: succeed to authorization ");
        return true;
    }
}

bool RedisManager::LPush(const std::string& key, const std::string& value)
{
    if (!m_inited)
        return false;

    redisContext* pConnection = m_spConnPool->getConnection();
    if (pConnection == nullptr)
    {
        LOG_ERROR("Redis: Failed to get redis Connection");
        return false;
    }

    redisReply* pReply = static_cast<redisReply*>(redisCommand(
        pConnection, "LPUSH %s %s", key.c_str(), value.c_str())
        );

    if (!pReply)
    {
        LOG_ERROR("Redis: failed to execute [ LPUSH %s %s ]", key.c_str(), value.c_str());
        freeReplyObject(pReply);
        return false;
    }

    if (pReply->type != REDIS_REPLY_INTEGER || pReply->integer <= 0)
    {
        LOG_ERROR("Redis: failed to execute [ LPUSH %s %s ]", key.c_str(), value.c_str());
        freeReplyObject(pReply);
        return false;
    }

    LOG_INFO("Redis: succeed to execute [ LPUSH %s %s ] ", key.c_str(), value.c_str());
    freeReplyObject(pReply);
    return true;
}

bool RedisManager::LPop(const std::string& key,  std::string& value)
{
    if (!m_inited)
        return false;

    redisContext* pConnection = m_spConnPool->getConnection();
    if (pConnection == nullptr)
    {
        LOG_ERROR("Redis: Failed to get redis Connection");
        return false;
    }

    redisReply* pReply = static_cast<redisReply*>(redisCommand(
        pConnection, "LPOP %s %s", key.c_str(), value.c_str())
        );

    if (pReply == nullptr || pReply->type != REDIS_REPLY_NIL)
    {
        LOG_ERROR("Redis: failed to execute [ LPOP %s %s ]", key.c_str(), value.c_str());
        freeReplyObject(pReply);
        return false;
    }

    value = pReply->str;
    LOG_INFO("Redis: succeed to execute [ LPOP %s %s ] ", key.c_str(), value.c_str());
    freeReplyObject(pReply);
    return true;
}

bool RedisManager::RPush(const std::string& key, const std::string& value)
{
    if (!m_inited)
        return false;

    redisContext* pConnection = m_spConnPool->getConnection();
    if (pConnection == nullptr)
    {
        LOG_ERROR("Redis: Failed to get redis Connection");
        return false;
    }

    redisReply* pReply = static_cast<redisReply*>(redisCommand(
        pConnection, "RPUSH %s %s", key.c_str(), value.c_str())
        );

    if (!pReply)
    {
        LOG_ERROR("Redis: failed to execute [ RPUSH %s %s ]", key.c_str(), value.c_str());
        freeReplyObject(pReply);
        return false;
    }

    if (pReply->type != REDIS_REPLY_INTEGER || pReply->integer <= 0)
    {
        LOG_ERROR("Redis: failed to execute [ RPUSH %s %s ]", key.c_str(), value.c_str());
        freeReplyObject(pReply);
        return false;
    }

    LOG_INFO("Redis: succeed to execute [ RPUSH %s %s ] ", key.c_str(), value.c_str());
    freeReplyObject(pReply);
    return true;
}

bool RedisManager::RPop(const std::string& key,  std::string& value)
{
    if (!m_inited)
        return false;

    redisContext* pConnection = m_spConnPool->getConnection();
    if (pConnection == nullptr)
    {
        LOG_ERROR("Redis: Failed to get redis Connection");
        return false;
    }

    redisReply* pReply = static_cast<redisReply*>(redisCommand(
        pConnection, "RPOP %s %s", key.c_str(), value.c_str())
        );

    if (pReply == nullptr || pReply->type != REDIS_REPLY_NIL)
    {
        LOG_ERROR("Redis: failed to execute [ RPOP %s %s ]", key.c_str(), value.c_str());
        freeReplyObject(pReply);
        return false;
    }

    value = pReply->str;

    LOG_INFO("Redis: succeed to execute [ RPOP %s %s ] ", key.c_str(), value.c_str());
    freeReplyObject(pReply);
    return true;
}

bool RedisManager::HSet(const std::string& key, const std::string& hkey, const std::string& value)
{
    if (!m_inited)
        return false;

    redisContext* pConnection = m_spConnPool->getConnection();
    if (pConnection == nullptr)
    {
        LOG_ERROR("Redis: Failed to get redis Connection");
        return false;
    }

    redisReply* pReply = static_cast<redisReply*>(redisCommand(
        pConnection, "HSET %s %s %s", key.c_str(), hkey.c_str(), value.c_str())
        );

    if (pReply == nullptr || pReply->type != REDIS_REPLY_INTEGER)
    {
        LOG_ERROR("Redis: failed to execute [ HSET %s %s %s ]", key.c_str(), hkey.c_str(), value.c_str());
        freeReplyObject(pReply);
        return false;
    }

    LOG_INFO("Redis: succeed to execute [ HSET %s %s %s ] ", key.c_str(), hkey.c_str(), value.c_str());
    freeReplyObject(pReply);
    return true;
}

bool RedisManager::HSet(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen)
{
    if (!m_inited)
        return false;

    const char* argv[4];
    size_t argvlen[4];
    argv[0] = "HSET";
    argvlen[0] = 4;
    argv[1] = key;
    argvlen[1] = strlen(key);
    argv[2] = hkey;
    argvlen[2] = strlen(hkey);
    argv[3] = hvalue;
    argvlen[3] = hvaluelen;

    redisContext* pConnection = m_spConnPool->getConnection();
    if (pConnection == nullptr)
    {
        LOG_ERROR("Redis: Failed to get redis Connection");
        return false;
    }

    redisReply* pReply = static_cast<redisReply*>(redisCommandArgv(pConnection, 4, argv, argvlen));
    if (pReply == nullptr || pReply->type != REDIS_REPLY_INTEGER) 
    {
        LOG_ERROR("Redis: failed to execute [ HSET %s %s %s ]", key, hkey, hvalue);
        freeReplyObject(pReply);
        return false;
    }
    LOG_INFO("Redis: succeed to execute [ HSET %s %s %s ] ", key, hkey, hvalue);
    freeReplyObject(pReply);
    return true;
}

std::string RedisManager::HGet(const std::string& key, const std::string& hkey)
{
    if (!m_inited)
        return false;

    const char* argv[3];
    size_t argvlen[3];
    argv[0] = "HGET";
    argvlen[0] = 4;
    argv[1] = key.c_str();
    argvlen[1] = key.length();
    argv[2] = hkey.c_str();
    argvlen[2] = hkey.length();

    redisContext* pConnection = m_spConnPool->getConnection();
    if (pConnection == nullptr)
    {
        LOG_ERROR("Redis: Failed to get redis Connection");
        return false;
    }

    redisReply* pReply = static_cast<redisReply*>(redisCommandArgv(pConnection, 3, argv, argvlen));
    if (pReply == nullptr || pReply->type == REDIS_REPLY_NIL) 
    {
        freeReplyObject(pReply);
        LOG_ERROR("Redis: failed to execute [ HGET %s %s ]", key, hkey);
        return "";
    }

    std::string value = pReply->str;
    freeReplyObject(pReply);
    LOG_INFO("Redis: succeed to execute [ HGET %s %s ] ", key, hkey);
    return value;
}

bool RedisManager::del(const std::string& key)
{
    if (!m_inited)
        return false;

    redisContext* pConnection = m_spConnPool->getConnection();
    if (pConnection == nullptr)
    {
        LOG_ERROR("Redis: Failed to get redis Connection");
        return false;
    }

    redisReply* pReply = static_cast<redisReply*>(redisCommand(pConnection, "DEL %s", key.c_str()));
    if (pReply  == nullptr || pReply->type != REDIS_REPLY_INTEGER) {
        LOG_ERROR("Redis: failed to execute [ DEL %s ]", key);
        freeReplyObject(pReply);
        return false;
    }
    LOG_INFO("Redis: succeed to execute [ DEL %s ] ", key);
    freeReplyObject(pReply);
    return true;
}

bool RedisManager::existsKey(const std::string& key)
{
    if (!m_inited)
        return false;

    redisContext* pConnection = m_spConnPool->getConnection();
    if (pConnection == nullptr)
    {
        LOG_ERROR("Redis: Failed to get redis Connection");
        return false;
    }

    redisReply* pReply = static_cast<redisReply*>(redisCommand(pConnection, "exists %s", key.c_str()));
    if (pReply == nullptr || pReply->type != REDIS_REPLY_INTEGER || pReply->integer == 0) 
    {
        LOG_INFO("Redis: Key [ %s ] Not Fount", key.c_str());
        freeReplyObject(pReply);
        return false;
    }
    LOG_INFO("Redis: Key [ %s ] Fount", key.c_str());
    freeReplyObject(pReply);
    return true;
}

void RedisManager::close()
{
    m_spConnPool->close();

}
