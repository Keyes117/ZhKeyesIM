#include "MySqlConnPool.h"

#include <chrono>

#include "Logger.h"

#include "const.h"

// ==================== MySqlConnection ===============
MySqlConnection::MySqlConnection(sql::Connection* conn, int64_t lasttime)
    : m_spConn(conn), m_lastOptTime (lasttime)
{
}


// ==================== MySqlConnPool ===============

MySqlConnPool::MySqlConnPool(const std::string& url, const std::string& user,
    const std::string& pass, const std::string& schema, int poolSize):
    m_url(url), m_user(user), m_pass(pass), m_schema(schema), m_poolSize(poolSize)
{

    try
    {
        for (int i = 0; i < m_poolSize; i++)
        {
            sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
            auto* conn = driver->connect(m_url, m_user, m_pass);
            conn->setSchema(m_schema);

            auto currentTime = std::chrono::system_clock::now().time_since_epoch();
            //将时间戳转换为 秒
            long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
            m_pool.push(std::make_unique<MySqlConnection>(conn, timestamp));
        }

        m_checkThread = std::thread(std::bind(&MySqlConnPool::checkThreadFunc, this));
        m_checkThread.detach();
        
    }
    catch (sql::SQLException& e)
    {
        LOG_ERROR("Mysql pool init failed, error is");
    }
}

MySqlConnPool::~MySqlConnPool()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    while (!m_pool.empty())
    {
        m_pool.pop();
    }
}

void MySqlConnPool::checkThreadFunc()
{
    while (!m_bStop)
    {
        checkConnectionPro();
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

void MySqlConnPool::checkConnectionPro()
{
    //1.先读取 “目标处理数”
    size_t targetCount;

    {
        std::lock_guard<std::mutex> guard(m_mutex);
        targetCount = m_pool.size();
    }

    //2. 当前已经处理的数量
    size_t processed = 0;

    auto now = std::chrono::system_clock::now().time_since_epoch();
    long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(now).count();

    while (processed < targetCount)
    {
        std::shared_ptr<MySqlConnection> conn;

        {
            std::lock_guard<std::mutex> guard(m_mutex);
            if (m_pool.empty())
                break;

            auto conn = std::move(m_pool.front());
            m_pool.pop();
        }

        bool healthy = true;

        if (timestamp - conn->m_lastOptTime >= 5)
        {
            try
            {
                std::unique_ptr<sql::Statement> stmt(conn->m_spConn->createStatement());
                stmt->executeQuery("SELECT 1");
                conn->m_lastOptTime = timestamp;
            }
            catch (sql::SQLException& e)
            {
                LOG_ERROR("MySQL Error keeping connection alive %d", e.what());
                healthy = false;
                m_failCount++;
            }
        }


        if (healthy)
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            m_pool.push(std::move(conn));
            m_cv.notify_one();
            
        }

        processed++;
    }

    while (m_failCount > 0)
    {
        bool bRes = reconnect(timestamp);
        if (bRes)
        {
            m_failCount--;
        }
        else
        {
            break;
        }
    }
}

bool MySqlConnPool::reconnect(long long timestamp)
{
    try
    {
        sql::mysql::MySQL_Driver* driver = sql::mysql::get_driver_instance();

        auto* conn = driver->connect(m_url, m_user, m_pass);
        conn->setSchema(m_schema);

        auto newConn = std::make_shared<MySqlConnection>(conn, timestamp);
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            m_pool.push(std::move(newConn));
        }

        LOG_INFO("MySQL connection reconnect success ");
        return true;
    }
    catch (sql::SQLException& e)
    {
        LOG_ERROR("MySQL reconnect failed, error is %d", e.what());
        return false;
    }
}

void MySqlConnPool::checkConnection()
{
    std::lock_guard<std::mutex> guard(m_mutex);

    int poolSize = m_pool.size();

    auto currentTime = std::chrono::system_clock::now().time_since_epoch();

    long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();

    for (int i = 0; i < poolSize; i++)
    {
        auto spConn = std::move(m_pool.front());
        m_pool.pop();

        ServerUtil::Defer def([this, &spConn]() {
            m_pool.push(std::move(spConn));
            });

        if (timestamp - spConn->m_lastOptTime < 5)
            continue;
        
        try
        {
            std::unique_ptr<sql::Statement> stmt(spConn->m_spConn->createStatement());
            stmt->executeQuery("SELECT 1");
            spConn->m_lastOptTime = timestamp;
;       }
        catch (sql::SQLException& e)
        {
            LOG_ERROR("MySQL Error keeping connection alive %s", e.what());

            sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
            auto* newConn = driver->connect(m_url, m_user, m_pass);
            newConn->setSchema(m_schema);
            spConn->m_spConn.reset(newConn);
            spConn->m_lastOptTime = timestamp;
        }
    }
}

std::shared_ptr<MySqlConnection> MySqlConnPool::getConnection()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cv.wait(lock, [this] {
        if (m_bStop) {
            return true;
        }
        return !m_pool.empty();

        });

    if (m_bStop)
    {
        return nullptr;
    }

    std::shared_ptr<MySqlConnection> spConn(std::move(m_pool.front()));
    m_pool.pop();
    return spConn;
}

void MySqlConnPool::returnConnection(std::shared_ptr<MySqlConnection> spConn)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_bStop)
        return;

    m_pool.push(std::move(spConn));
    m_cv.notify_one();
}

void MySqlConnPool::close()
{
    m_bStop = true;
    m_cv.notify_all();
}