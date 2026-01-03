#ifndef STATUSSERVER_REDISCONNPOOL_H_
#define STATUSSERVER_REDISCONNPOOL_H_

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>



#include "hiredis.h"

class RedisConnPool
{
public:
    RedisConnPool(size_t poolSize, const char* host, int port, const char* pwd);

    ~RedisConnPool();

    redisContext* getConnection();

    void returnConnection(redisContext* context);

    void close();

private:
    
    std::atomic<bool>   m_bStop = true;
    size_t              m_poolSize;
    const char* m_host;
    int         m_port;
    std::queue<redisContext*>   m_connections;
    std::mutex      m_mutex;
    std::condition_variable     m_cv;

};

#endif