#ifndef GATESERVER_REPOSITORY_USERREPOSITORY_H_
#define GATESERVER_REPOSITORY_USERREPOSITORY_H_

#include <memory>
#include <optional>
#include <string>

#include "model/User.h"
#include "infrastructure/MySqlManager.h"

class UserRepository
{
public:
    explicit UserRepository(std::shared_ptr<MySqlManager> mySqlManager);
    ~UserRepository() = default;

    std::optional<UserInfo> findByUsername(const std::string& username);
    std::optional<UserInfo> findByEmail(const std::string& email);
    std::optional<UserInfo> findByUid(int uid);

    bool exsitsByUsername(const std::string& username);
    bool exsitsByEmail(const std::string& email);


    int create(const std::string& username,
        const std::string& email,
        const std::string& passwordHash);
    bool updatePassword(const std::string& email,
        const std::string& newPasswordHash);
    bool updateLastLoginTime(int uid);

private:
    UserInfo parseUserInfoFromResultSet(sql::ResultSet* pResultSet);
private:
    std::shared_ptr<MySqlManager> m_spMySql;
};


#endif //!GATESERVER_REPOSITORY_USERREPOSITORY_H_