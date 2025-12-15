#include "MySqlDao.h"

#include "const.h"


MySqlDao::MySqlDao()
{
    
}

MySqlDao::~MySqlDao()
{
    m_pool->close();
}

bool MySqlDao::init(const std::string& host, const std::string& port, const std::string& password,
    const std::string& schema, const std::string& user, int connNum)
{

    m_pool = std::make_unique<MySqlConnPool>(host + ":" + port, user, password, schema, connNum);
    return m_pool->init();

}




int MySqlDao::registerUser(const std::string& name, const std::string& email, const std::string& pwd)
{
    auto spConn = m_pool->getConnection();

    try
    {
        if (spConn == nullptr)
            return -1;

        spConn->m_spConn->setAutoCommit(false);

        std::unique_ptr <sql::PreparedStatement> checkNameStmt(
            spConn->m_spConn->prepareStatement(
                "select count(*) as cnt from 'user' where 'email' = ?"
            ));

        checkNameStmt->setString(1, email);
        std::unique_ptr<sql::ResultSet> nameRes(checkNameStmt->executeQuery());

        if (nameRes->next() && nameRes->getInt("cnt") > 0)
        {
            spConn->m_spConn->rollback();
            spConn->m_spConn->setAutoCommit(true);
            m_pool->returnConnection(std::move(spConn));
            return 0;
        }


        std::unique_ptr<sql::PreparedStatement> insertStmt
        (spConn->m_spConn->prepareStatement(
            "INSERT INTO `user` (`name`, `email`, `pwd`) VALUES (?, ?, ?)"
        ));
        insertStmt->setString(1, name);
        insertStmt->setString(2, email);
        insertStmt->setString(3, pwd);
        insertStmt->executeUpdate();

        // 4. 获取插入的自增 id
        std::unique_ptr<sql::Statement> getIdStmt(spConn->m_spConn->createStatement());
        std::unique_ptr<sql::ResultSet> idRes(getIdStmt->executeQuery("SELECT LAST_INSERT_ID() as uid"));

        int uid = -1;
        if (idRes->next())
        {
            uid = idRes->getInt("uid");

            // 5. 如果 uid 字段和 id 字段是分开的，需要更新 uid 字段
            // 如果表结构中 uid 就是 id，可以跳过这一步
            if (uid > 0)
            {
                std::unique_ptr<sql::PreparedStatement> updateUidStmt
                (spConn->m_spConn->prepareStatement("UPDATE `user` SET `uid` = ? WHERE `id` = ?"));
                updateUidStmt->setInt(1, uid);
                updateUidStmt->setInt(2, uid);
                updateUidStmt->executeUpdate();
            }
        }

        // 6. 提交事务
        spConn->m_spConn->commit();
        spConn->m_spConn->setAutoCommit(true);

        m_pool->returnConnection(std::move(spConn));

        if (uid > 0)
        {
            LOG_INFO("User registered successfully: username=[%s], email=[%s], uid=[%d]",
                name.c_str(), email.c_str(), uid);
            return uid;
        }
        else
        {
            LOG_ERROR("Failed to get inserted user id");
            return -1;
        }
    }
    catch (sql::SQLException& e) {
        try
        {
            spConn->m_spConn->rollback();
            spConn->m_spConn->setAutoCommit(true);
        }
        catch (sql::SQLException& rollbackEx)
        {
            LOG_ERROR("Failed to rollback transaction: %s", rollbackEx.what());
        }

        m_pool->returnConnection(std::move(spConn));

        LOG_ERROR("SQLException in registerUser: %s \n"
            "MySQL error code: %d \n"
            "SQLState: %s",
            e.what(), e.getErrorCode(), e.getSQLState());

        return -1;
    }
}

int MySqlDao::registerUserTransaction(const std::string& name, const std::string& email, const std::string& pwd, const std::string& icon)
{
    return 0;
}

bool MySqlDao::checkEmail(const std::string& name, const std::string& email)
{
    std::shared_ptr<MySqlConnection> conn = m_pool->getConnection();

    try
    {
        if (conn == nullptr)
        {
            m_pool->returnConnection(std::move(conn));
            return false;
        }

        std::unique_ptr<sql::PreparedStatement> pstmt(conn->m_spConn->prepareStatement(
            "SELECT email FROM user WHERE name = ?"
        ));

        pstmt->setString(1, name);

        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        while (res->next())
        {
            if (email != res->getString("email"))
            {
                m_pool->returnConnection(std::move(conn));
                return false;
            }

            m_pool->returnConnection(std::move(conn));
            return true;
        }
    }
    catch (sql::SQLException& e)
    {
        LOG_ERROR("SQLException: %s \n  \
                   MySQL error code : %d \n , \
                   SQLState: %s", e.what(), e.getErrorCode(), e.getSQLState()
        );
        m_pool->returnConnection(std::move(conn));
        return false;
    }
}

bool MySqlDao::updatePassword(const std::string& name, const std::string& newPass)
{
    std::shared_ptr<MySqlConnection> conn = m_pool->getConnection();

    try
    {
        if (conn == nullptr)
        {
            m_pool->returnConnection(std::move(conn));
            return false;
        }

        std::unique_ptr<sql::PreparedStatement> pstmt(conn->m_spConn->prepareStatement(
            "UPDATE user SET pwd = ? WHERE name = ?"
        ));

        pstmt->setString(1, newPass);
        pstmt->setString(2, name);

        int updateCount = pstmt->executeUpdate();

        LOG_INFO("Updated rows: %d ", updateCount);
        m_pool->returnConnection(std::move(conn));

        return true;
    }
    catch (sql::SQLException& e)
    {
        LOG_ERROR("SQLException: %s \n  \
                   MySQL error code : %d \n , \
                   SQLState: %s", e.what(), e.getErrorCode(), e.getSQLState()
        );
        m_pool->returnConnection(std::move(conn));
        return false;
    }
}

bool MySqlDao::checkPassword(const std::string& name, const std::string& pwd, UserInfo& userInfo)
{
    std::shared_ptr<MySqlConnection> conn = m_pool->getConnection();

    try
    {
        if (conn == nullptr)
        {
            m_pool->returnConnection(std::move(conn));
            return false;
        }

        std::unique_ptr<sql::PreparedStatement> pstmt(conn->m_spConn->prepareStatement(
            "SELECT * FROM user WHERE name = ?"
        ));

        pstmt->setString(1, name);

        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        std::string origin_pwd = "";
        while (res->next())
        {
            origin_pwd = res->getString("pwd");
            LOG_INFO("Password: %s", origin_pwd.c_str());
            break;
        }

        if (pwd != origin_pwd)
        {
            m_pool->returnConnection(std::move(conn));
            return false;
        }

        m_pool->returnConnection(std::move(conn));
        userInfo.name = name;
        userInfo.email = res->getString("email");
        userInfo.uid = res->getInt("uid");
        userInfo.password = origin_pwd;
        return true;
    }
    catch(sql::SQLException& e)
    {
        LOG_ERROR("SQLException: %s \n  \
                   MySQL error code : %d \n , \
                   SQLState: %s", e.what(), e.getErrorCode(), e.getSQLState()
        );
        m_pool->returnConnection(std::move(conn));
        return false;
    }
}
