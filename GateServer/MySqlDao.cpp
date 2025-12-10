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
            return false;

        std::unique_ptr< sql::PreparedStatement> stmt
        (spConn->m_spConn->prepareStatement("CALL reg_user(?,?,?,@result)"));

        stmt->setString(1, name);
        stmt->setString(2, email);
        stmt->setString(3, pwd);

        // 由于PreparedStatement不直接支持注册输出参数，我们需要使用会话变量或其他方法来获取输出参数的值
        //执行存储过程
        stmt->execute();
        std::unique_ptr<sql::Statement> stmtResult(spConn->m_spConn->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmtResult->executeQuery("SELECT @result AS result"));
        
        if (res->next()) {
            int result = res->getInt("result");
            LOG_INFO("Result: %d", result);
            m_pool->returnConnection(std::move(spConn));
            return result;
        }

        m_pool->returnConnection(std::move(spConn));
        return -1;
    }
    catch (sql::SQLException& e) {
        m_pool->returnConnection(std::move(spConn));
        LOG_ERROR("SQLException: %s \n  \
                   MySQL error code : %d \n , \
                   SQLState: %s" , e.what(), e.getErrorCode(), e.getSQLState()
            );
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
