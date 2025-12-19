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
    explicit UserRepository(const std::shared_ptr<MySqlManager> mySqlManager);
    ~UserRepository();

    std::optional<UserInfo> findByUsername(const std::string& username);
    std::optional<UserInfo> findByEmail(const std::string& email);
    std::optional<UserInfo> findByUid(int uid);

    
    bool exsitsByUsername(const std::string& username);
    bool exsitsByEmail(const std::string& email);


    int create(const std::string& username,
            const std::string& email,
            const std::string& passwordHash);
    bool updatePassword(const std::string& username,
            const std::string& newPasswordHash);

    bool checkPassword(const std::string& username, 
            const std::string& password,
            UserInfo& outUserInfo);

    bool checkEmail(const std::string& username,
            const std::string& email);

    bool updateLastLoginTime(int uid);
private:
    std::shared_ptr<MySqlManager> m_mySqlManager;
}


#endif //!GATESERVER_REPOSITORY_USERREPOSITORY_H_