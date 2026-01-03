#ifndef STATUSSERVER_MYSQLCONNPOOL_H_
#define STATUSSERVER_MYSQLCONNPOOL_H_

#include <cstdint>
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <string>


#include "mysql/jdbc.h"
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/exception.h>

// ==================== MySqlConnection ===============
class MySqlConnection
{
public:
    MySqlConnection(sql::Connection* conn, int64_t lasttime);

    int64_t m_lastOptTime;
    std::unique_ptr<sql::Connection> m_spConn;
  
};


// ==================== MySqlConnPool ===============
class MySqlConnPool
{
public:
    MySqlConnPool(const std::string& url, const std::string& user,
        const std::string& pass, const std::string& schema, int poolSize);

    ~MySqlConnPool();

    bool init();

    void close();

    std::shared_ptr<MySqlConnection> getConnection();

    void returnConnection(std::shared_ptr<MySqlConnection>);

private:
    void checkThreadFunc();

    void checkConnectionPro();

    bool reconnect(long long timestamp);

    void checkConnection();



private:
    std::string m_url;
    std::string m_user;
    std::string m_pass;
    std::string m_schema;
    int         m_poolSize;

    std::queue<std::shared_ptr<MySqlConnection>> m_pool;
    std::mutex  m_mutex;
    std::condition_variable m_cv;
    std::atomic<bool>   m_bStop = false;
    std::thread m_checkThread;
    std::atomic<int>    m_failCount;
};

#endif /!STATUSSERVER_MYSQLPOOL_H_