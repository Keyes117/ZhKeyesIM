#include "MySqlDao.h"

#include "const.h"


MySqlDao::MySqlDao()
{
    
}

MySqlDao::~MySqlDao()
{
    m_pool->close();
}


bool MySqlDao::init(const ConfigManager& config)
{
    auto hostOpt = config.getSafe<std::string>({ "mysql","host" });
    auto portOpt = config.getSafe<std::string>({ "mysql","port" });
    auto passwordOpt = config.getSafe<std::string>({ "mysql","password" });
    auto schemaOpt = config.getSafe<std::string>({ "mysql","scheme" });
    auto userOpt = config.getSafe<std::string>({ "mysql","user" });
   

    if (!hostOpt || !portOpt || !passwordOpt ||!schemaOpt ||!userOpt)
    {
        return false;
    }

    std::string host = *hostOpt;
    std::string port = *portOpt;
    std::string password = *passwordOpt;
    std::string schema = *schemaOpt;
    std::string user = *userOpt;

    m_pool = std::make_unique<MySqlConnPool>(host + ":" + port, user, password, schema, 10);
   

    return true;
}

int MySqlDao::RegisterUser(const std::string& name, const std::string& email, const std::string& pwd)
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

int MySqlDao::RegisterUserTransaction(const std::string& name, const std::string& email, const std::string& pwd, const std::string& icon)
{
    return 0;
}
