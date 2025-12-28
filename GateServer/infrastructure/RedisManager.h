#ifndef GATESERVER_REDISMANAGER_H_
#define GATESERVER_REDISMANAGER_H_

#include <string>
#include <optional>

#include "hiredis.h"

#include "ConfigManager.h"
#include "RedisConnPool.h"

class RedisManager
{
public:
    RedisManager();

    ~RedisManager();

    bool init(const ConfigManager& config);

    bool Get(const std::string& key, std::string& value);
    bool Set(const std::string& key, const std::string& value);
    bool Auth(const std::string& password);
    bool LPush(const std::string& key, const std::string& value);
    bool LPop(const std::string& key,  std::string& value);
    bool RPush(const std::string& key, const std::string& value);
    bool RPop(const std::string& key,  std::string& value);
    bool HSet(const std::string& key, const std::string& hkey,  const std::string& value);
    bool HSet(const char* key, const char* hkey, const char* hvalue,
        size_t hvaluelen);
    std::string HGet(const std::string& key, const std::string& hkey);
    bool del(const std::string& key);
    bool existsKey(const std::string& key);
    void close();


private:
    bool    m_inited = false;
    std::unique_ptr<RedisConnPool> m_spConnPool;
private:
    RedisManager(const RedisManager& rhs) = delete;
    RedisManager& operator=(const RedisManager& rhs) = delete;
    RedisManager(RedisManager&& rhs) = delete;
    RedisManager& operator=(RedisManager&& rhs) = delete;

};

#endif // !GATESERVER_REDISMANAGER_H_