#include "UserRepository.h"

UserRepository::UserRepository(std::shared_ptr<MySqlManager> mySqlManager)
    : m_spMySql(mySqlManager)
{
}

std::optional<UserInfo> UserRepository::findByUsername(const std::string& username)
{
    UserInfo userInfo;
    bool isValid = m_spMySql->checkPassword(username,)
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

bool UserRepository::checkPassword(const std::string& username, const std::string& password, UserInfo& outUserInfo)
{
    return false;
}

bool UserRepository::checkEmail(const std::string& username, const std::string& email)
{
    return false;
}

bool UserRepository::updateLastLoginTime(int uid)
{
    return false;
}
