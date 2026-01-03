// MySqlManager.cpp
#include "MySqlManager.h"
#include "Logger.h"


MySqlManager::MySqlManager() {
}

MySqlManager::~MySqlManager() {
    if (m_connPool) {
        m_connPool->close();
    }
}

bool MySqlManager::init(const ConfigManager& config) {
    auto hostOpt = config.getSafe<std::string>({ "mysql", "host" });
    auto portOpt = config.getSafe<std::string>({ "mysql", "port" });
    auto passwordOpt = config.getSafe<std::string>({ "mysql", "password" });
    auto schemaOpt = config.getSafe<std::string>({ "mysql", "schema" });
    auto userOpt = config.getSafe<std::string>({ "mysql", "user" });
    auto connNumOpt = config.getSafe<int>({ "mysql", "connNumber" });

    if (!hostOpt || !portOpt || !passwordOpt || !schemaOpt || !userOpt || !connNumOpt) {
        LOG_ERROR("MySqlManager: Missing required MySQL configuration");
        return false;
    }

    std::string host = *hostOpt;
    std::string port = *portOpt;
    std::string password = *passwordOpt;
    std::string schema = *schemaOpt;
    std::string user = *userOpt;
    int connNumber = *connNumOpt;

    m_connPool = std::make_unique<MySqlConnPool>(
        host + ":" + port, user, password, schema, connNumber
    );

    bool success = m_connPool->init();
    if (success) {
        LOG_INFO("MySqlManager: Initialized with pool size %d", connNumber);
    }
    else {
        LOG_ERROR("MySqlManager: Failed to initialize connection pool");
    }

    return success;
}

std::shared_ptr<MySqlConnection> MySqlManager::getConnection() {
    if (!m_connPool) {
        LOG_ERROR("MySqlManager: Connection pool not initialized");
        return nullptr;
    }
    return m_connPool->getConnection();
}

void MySqlManager::returnConnection(std::shared_ptr<MySqlConnection> conn) {
    if (m_connPool && conn) {
        m_connPool->returnConnection(std::move(conn));
    }
}

std::unique_ptr<sql::PreparedStatement> MySqlManager::prepareStatement(
    std::shared_ptr<MySqlConnection> conn,
    const std::string& sql) {

    if (!conn || !conn->m_spConn) {
        LOG_ERROR("MySqlManager: Invalid connection");
        return nullptr;
    }
    return std::unique_ptr<sql::PreparedStatement>(
        conn->m_spConn->prepareStatement(sql)
    );
}

std::unique_ptr<sql::ResultSet> MySqlManager::executeQuery(
    std::shared_ptr<MySqlConnection> conn,
    const std::string& sql) {
    if (!conn || !conn->m_spConn) {
        LOG_ERROR("MySqlManager: Invalid connection");
        return nullptr;
    }
    std::unique_ptr<sql::Statement> stmt(conn->m_spConn->createStatement());
    return std::unique_ptr<sql::ResultSet>(stmt->executeQuery(sql));
}

int MySqlManager::executeUpdate(
    std::shared_ptr<MySqlConnection> conn,
    const std::string& sql) {
    if (!conn || !conn->m_spConn) {
        LOG_ERROR("MySqlManager: Invalid connection");
        return -1;
    }
    std::unique_ptr<sql::Statement> stmt(conn->m_spConn->createStatement());
    return stmt->executeUpdate(sql);
}

bool MySqlManager::beginTransaction(std::shared_ptr<MySqlConnection> conn) {
    if (!conn || !conn->m_spConn) return false;
    conn->m_spConn->setAutoCommit(false);
    return true;
}

bool MySqlManager::commit(std::shared_ptr<MySqlConnection> conn) {
    if (!conn || !conn->m_spConn) return false;
    conn->m_spConn->commit();
    conn->m_spConn->setAutoCommit(true);
    return true;
}

bool MySqlManager::rollback(std::shared_ptr<MySqlConnection> conn) {
    if (!conn || !conn->m_spConn) return false;
    conn->m_spConn->rollback();
    conn->m_spConn->setAutoCommit(true);
    return true;
}

int MySqlManager::getLastInsertId(std::shared_ptr<MySqlConnection> conn) {
    if (!conn || !conn->m_spConn) return -1;

    try {
        std::unique_ptr<sql::Statement> stmt(conn->m_spConn->createStatement());
        std::unique_ptr<sql::ResultSet> res(
            stmt->executeQuery("SELECT LAST_INSERT_ID() as id")
        );

        if (res->next()) {
            return res->getInt("id");
        }
    }
    catch (sql::SQLException& e) {
        LOG_ERROR("MySqlManager: Failed to get last insert ID: %s", e.what());
    }

    return -1;
}

bool MySqlManager::isHealthy() const {
    return m_connPool != nullptr;
}

void MySqlManager::close() {
    if (m_connPool) {
        m_connPool->close();
    }
}