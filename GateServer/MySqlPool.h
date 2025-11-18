#ifndef GATESERVER_MYSQLPOOL_H_
#define GATESERVER_MYSQLPOOL_H_

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <string>


#include "mysql/jdbc.h"

class MySqlPool
{
public:
    MySqlPool(const std::string& url, const std::string& user,
        const std::string& pass, const std::string& schema, int poolSize);

private:
    std::string m_url;
    std::string m_user;
    std::string m_pass;
    std::string m_schema;
    int         m_poolSize;

    std::queue<std::unique_ptr<sql::Connection>> m_pool;
    std::mutex  m_mutex;
    std::condition_variable m_cv;
    std::atomic<bool>   m_bStop;
};

#endif /!GATESERVER_MYSQLPOOL_H_