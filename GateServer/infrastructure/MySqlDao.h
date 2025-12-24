#ifndef GATESERVER_MYSQLDAO_H_
#define GATESERVER_MYSQLDAO_H_

#include <memory>
#include <string>

#include "MySqlConnPool.h"

#include "model/User.h"

class MySqlDao
{
public:

    MySqlDao();
    ~MySqlDao();

    bool init(const std::string& host, const std::string& port, const std::string& password,
        const std::string& schema, const std::string& user, int connNum);

    int registerUser(const std::string& name, const std::string& email, const std::string& pwd);
    int registerUserTransaction(const std::string& name, const std::string& email, const std::string& pwd, const std::string& icon);
    bool checkEmail(const std::string& name, const std::string& email);
    bool updatePassword(const std::string& name, const std::string& newpwd);

    bool checkPassword(const std::string& name, const std::string& pwd, UserInfo& userInfo);

    //bool TestProcedure(const std::string& email, int& uid, std::string& name);

private:
    std::unique_ptr<MySqlConnPool> m_pool;
};

#endif //!GATESERVER_MYSQLDAO_H_