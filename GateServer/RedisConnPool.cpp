#include "RedisConnPool.h"

#include "Logger.h"

RedisConnPool::RedisConnPool(size_t poolSize, const char* host,
    int port, const char* pwd) :
    m_poolSize(poolSize),
    m_host(host),
    m_port(port),
    m_bStop(false)
{

    int successCount = 0;
    for (size_t i = 0; i < m_poolSize; i++)
    {
        auto* context = redisConnect(host, port);
        if (context == nullptr || context->err != 0)
        {
            if (context != nullptr)
                redisFree(context);

            continue;
        }

        auto reply = static_cast<redisReply*>(redisCommand(context, "AUTH %s", pwd));

        if (reply->type == REDIS_REPLY_ERROR)
        {
            freeReplyObject(reply);
            continue;
        }

        successCount++;
        freeReplyObject(reply);
   
    }

    LOG_INFO("Redis: 连接池期望 初始化 %d 个连接，成功初始化 %d 个连接 ",m_poolSize, successCount);
}

RedisConnPool::~RedisConnPool()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    while (m_connections.empty())
        m_connections.pop();
}

redisContext* RedisConnPool::getConnection()
{
    std::unique_lock<std::mutex> lock(m_mutex);

    m_cv.wait(lock, [this] {
        if (m_bStop)
            return true;

        return !m_connections.empty();
        });

    if (m_bStop)
        return nullptr;

    auto* context = m_connections.front();
    m_connections.pop();

    return context;
}

void RedisConnPool::returnConnection(redisContext* context)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_bStop)
        return;

    m_connections.push(context);
    m_cv.notify_one();
}


void RedisConnPool::close()
{
    m_bStop = true;
    m_cv.notify_all();
}

