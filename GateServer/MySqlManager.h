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

    int registerUser(const std::string& name, const std::string& email, const std::string& pwd)
    {
        return m_dao->registerUser(name, email, pwd);
    }

private:
    std::unique_ptr<MySqlDao> m_dao;
};

#endif