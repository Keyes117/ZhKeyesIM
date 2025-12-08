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
    auto schemaOpt = config.getSafe<std::string>({ "mysql","scheme" });
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
