#include "UserRepository.h"

#include "const.h"
#include "Defer.h"

UserRepository::UserRepository(std::shared_ptr<MySqlManager> mySqlManager)
    : m_spMySql(mySqlManager)
{
}

std::optional<UserInfo> UserRepository::findByUsername(const std::string& username)
{
    auto spConn = m_spMySql->getConnection();

    Defer def([this, &spConn]() {
        m_spMySql->returnConnection(std::move(spConn));
        });

    if (!spConn)
    {
        LOG_ERROR("UserRepository: Failed to get database connect");
        return std::nullopt;
    }

    try
    {
        auto spStmt = m_spMySql->prepareStatement(spConn,
            "Select uid, name, email, pwd, create_time, last_login_time From user Where name = ?"
        );

        spStmt->setString(1, username);
        std::unique_ptr<sql::ResultSet> res(spStmt->executeQuery());

        if (res->next())
        {
            UserInfo user = parseUserInfoFromResultSet(res.get());         
            return user;
        }

        return std::nullopt;
    }
    catch (sql::SQLException& e)
    {
        LOG_ERROR("UserRepository: findByUsername failed: %s", e.what());
        return std::nullopt;
    }
    
}

std::optional<UserInfo> UserRepository::findByEmail(const std::string& email)
{
    auto spConn = m_spMySql->getConnection();

    Defer def([this, &spConn]() {
        m_spMySql->returnConnection(std::move(spConn));
        });

    if (!spConn)
    {
        LOG_ERROR("UserRepository: Failed to get database connect");
        return std::nullopt;
    }

    try
    {
        auto spStmt = m_spMySql->prepareStatement(spConn,
            "Select uid, name, email, pwd, create_time, last_login_time From user Where email = ?"
        );

        spStmt->setString(1, email);
        std::unique_ptr<sql::ResultSet> res(spStmt->executeQuery());

        if (res->next())
        {
            UserInfo user = parseUserInfoFromResultSet(res.get());
            return user;
        }

        return std::nullopt;
    }
    catch (sql::SQLException& e)
    {
        LOG_ERROR("UserRepository: findByUsername failed: %s", e.what());
        return std::nullopt;
    }
}

std::optional<UserInfo> UserRepository::findByUid(int uid)
{
    return std::optional<UserInfo>();
}

bool UserRepository::exsitsByUsername(const std::string& username)
{
    auto spConn = m_spMySql->getConnection();

    Defer def([this, &spConn]() {
        m_spMySql->returnConnection(std::move(spConn));
        });

    if (!spConn)
    {
        LOG_ERROR("UserRepository: Failed to get database connection");
        return false;
    }

    try
    {
        auto spStmt = m_spMySql->prepareStatement(spConn,
            "SELECT COUNT(*) as count FROM user WHERE name = ?"
        );

        spStmt->setString(1, username);
        std::unique_ptr<sql::ResultSet> res(spStmt->executeQuery());

        if (res->next())
        {
            int count = res->getInt("count");
            return count > 0;
        }

        return false;
    }
    catch (sql::SQLException& e)
    {
        LOG_ERROR("UserRepository: exsitsByUsername failed: %s", e.what());
        return false;
    }
}

bool UserRepository::exsitsByEmail(const std::string& email)
{
    auto spConn = m_spMySql->getConnection();

    Defer def([this, &spConn]() {
        m_spMySql->returnConnection(std::move(spConn));
        });

    if (!spConn)
    {
        LOG_ERROR("UserRepository: Failed to get database connection");
        return false;
    }

    try
    {
        auto spStmt = m_spMySql->prepareStatement(spConn,
            "SELECT COUNT(*) as count FROM user WHERE email = ?"
        );

        spStmt->setString(1, email);
        std::unique_ptr<sql::ResultSet> res(spStmt->executeQuery());

        if (res->next())
        {
            int count = res->getInt("count");
            return count > 0;
        }

        return false;
    }
    catch (sql::SQLException& e)
    {
        LOG_ERROR("UserRepository: exsitsByEmail failed: %s", e.what());
        return false;
    }
}

int UserRepository::create(const std::string& username, const std::string& email, const std::string& passwordHash)
{
    auto spConn = m_spMySql->getConnection();

    Defer def([this, &spConn]() {
        m_spMySql->returnConnection(std::move(spConn));
        });

    if (!spConn)
    {
        LOG_ERROR("UserRepository: Failed to get database connection");
        return -1;
    }

    try
    {
        // 开始事务
        m_spMySql->beginTransaction(spConn);

        // 1. 更新 user_id 表获取新的 uid
        auto spUpdateStmt = m_spMySql->prepareStatement(spConn,
            "UPDATE user_id SET id = id + 1"
        );
        spUpdateStmt->executeUpdate();

        // 2. 查询新的 uid
        auto spQueryStmt = m_spMySql->prepareStatement(spConn,
            "SELECT id FROM user_id"
        );
        std::unique_ptr<sql::ResultSet> res(spQueryStmt->executeQuery());

        int newUid = -1;
        if (res->next())
        {
            newUid = res->getInt("id");
        }
        else
        {
            m_spMySql->rollback(spConn);
            LOG_ERROR("UserRepository: Failed to get new uid");
            return -1;
        }

        // 3. 插入新用户
        auto spInsertStmt = m_spMySql->prepareStatement(spConn,
            "INSERT INTO user (uid, name, email, pwd, create_time) VALUES (?, ?, ?, ?, UNIX_TIMESTAMP())"
        );

        spInsertStmt->setInt(1, newUid);
        spInsertStmt->setString(2, username);
        spInsertStmt->setString(3, email);
        spInsertStmt->setString(4, passwordHash);

        int affectedRows = spInsertStmt->executeUpdate();

        if (affectedRows > 0)
        {
            // 提交事务
            m_spMySql->commit(spConn);
            LOG_INFO("UserRepository: User created successfully with uid: %d", newUid);
            return newUid;
        }
        else
        {
            m_spMySql->rollback(spConn);
            LOG_ERROR("UserRepository: Failed to insert user");
            return -1;
        }
    }
    catch (sql::SQLException& e)
    {
        m_spMySql->rollback(spConn);
        LOG_ERROR("UserRepository: create failed: %s", e.what());
        return -1;
    }
}

bool UserRepository::updatePassword(const std::string& email, const std::string& newPasswordHash)
{
    return false;
}



bool UserRepository::updateLastLoginTime(int uid)
{
    auto spConn = m_spMySql->getConnection();

    Defer def([this, &spConn]() {
        m_spMySql->returnConnection(std::move(spConn));
        });

    if (!spConn)
    {
        LOG_ERROR("UserRepository: Failed to get database connection");
        return false;
    }

    try
    {
        auto spStmt = m_spMySql->prepareStatement(spConn,
            "UPDATE user SET last_login_time = UNIX_TIMESTAMP() WHERE uid = ?"
        );

        spStmt->setInt(1, uid);
        int affectedRows = spStmt->executeUpdate();

        return affectedRows > 0;
    }
    catch (sql::SQLException& e)
    {
        LOG_ERROR("UserRepository: updateLastLoginTime failed: %s", e.what());
        return false;
    }
}

UserInfo UserRepository::parseUserInfoFromResultSet(sql::ResultSet* pResultSet)
{
    UserInfo userInfo;
    userInfo.uid = pResultSet->getInt("uid");
    userInfo.username = pResultSet->getString("name");
    userInfo.email = pResultSet->getString("email");
    userInfo.passwordHash = pResultSet->getString("pwd");

    // 处理时间字段（可能为 NULL）
    if (!pResultSet->isNull("create_time")) {
        userInfo.createTime = pResultSet->getInt64("create_time");
    }
    if (!pResultSet->isNull("last_login_time")) {
        userInfo.lastLoginTime = pResultSet->getInt64("last_login_time");
    }

    return userInfo;
}
