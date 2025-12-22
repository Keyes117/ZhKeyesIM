#include "MySqlManager.h"

MySqlManager::MySqlManager():
    m_dao(std::make_unique<MySqlDao>())
{
}

MySqlManager::~MySqlManager()
{
}

bool MySqlManager::init(const ConfigManager& config)
{
    auto hostOpt = config.getSafe<std::string>({ "mysql","host" });
    auto portOpt = config.getSafe<std::string>({ "mysql","port" });
    auto passwordOpt = config.getSafe<std::string>({ "mysql","password" });
    auto schemaOpt = config.getSafe<std::string>({ "mysql","schema" });
    auto userOpt = config.getSafe<std::string>({ "mysql","user" });
    auto connNumOpt = config.getSafe<int>({ "mysql","connNumber" });

    if (!hostOpt || !portOpt || !passwordOpt || !schemaOpt || !userOpt ||!connNumOpt)
    {
        return false;
    }

    std::string host = *hostOpt;
    std::string port = *portOpt;
    std::string password = *passwordOpt;
    std::string schema = *schemaOpt;
    std::string user = *userOpt;
    int connNumber = *connNumOpt;
    
    return m_dao->init(host, port, password, schema, user, connNumber);

}

int MySqlManager::registerUser(const std::string& name, const std::string& email, const std::string& password)
{
    return m_dao->registerUser(name, email, password);
}

bool MySqlManager::checkEmail(const std::string& name, const std::string& email)
{
    return m_dao->checkEmail(name, email);
}

bool MySqlManager::updatePassword(const std::string& name, const std::string newPassword)
{
    return m_dao->updatePassword(name, newPassword);
}

bool MySqlManager::checkPassword(const std::string& name, const std::string& password, UserInfo& userInfo)
{
    return m_dao->checkPassword(name, password, userInfo);
}
