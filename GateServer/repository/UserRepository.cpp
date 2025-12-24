#include "UserRepository.h"

#include "const.h"
UserRepository::UserRepository(std::shared_ptr<MySqlManager> mySqlManager)
    : m_spMySql(mySqlManager)
{
}

std::optional<UserInfo> UserRepository::findByUsername(const std::string& username)
{
    auto spConn = m_spMySql->getConnection();

    ServerUtil::Defer def([this, &spConn]() {
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
    return std::optional<UserInfo>();
}

std::optional<UserInfo> UserRepository::findByUid(int uid)
{
    return std::optional<UserInfo>();
}

bool UserRepository::exsitsByUsername(const std::string& username)
{
    return false;
}

bool UserRepository::exsitsByEmail(const std::string& email)
{
    return false;
}

int UserRepository::create(const std::string& username, const std::string& email, const std::string& passwordHash)
{
    return 0;
}

bool UserRepository::updatePassword(const std::string& username, const std::string& newPasswordHash)
{
    return false;
}



bool UserRepository::updateLastLoginTime(int uid)
{
    return false;
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
