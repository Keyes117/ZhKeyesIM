#ifndef GATESERVER_MYSQLMANAGER_H_
#define GATESERVER_MYSQLMANAGER_H_


#include <string>
#include <memory>

#include "ConfigManager.h"
#include "MySqlDao.h"

class MySqlManager
{
public:
    MySqlManager();
    ~MySqlManager();

    bool init(const ConfigManager& config);

    int registerUser(const std::string& name, const std::string& email, const std::string& pwd);

    bool checkEmail(const std::string& name, const std::string& email);

    bool updatePassword(const std::string& name, const std::string newPass);

    bool checkPassword(const std::string& name, const std::string& password, UserInfo& userInfo);

private:
    std::unique_ptr<MySqlDao> m_dao;
};

#endif