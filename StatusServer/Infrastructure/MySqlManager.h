// MySqlManager.h
#ifndef STATUSSERVER_MYSQLMANAGER_H_
#define STATUSSERVER_MYSQLMANAGER_H_

#include <string>
#include <memory>
#include <optional>
#include "ConfigManager.h"
#include "MySqlConnPool.h"

#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/resultset.h>

class MySqlManager {
public:
    MySqlManager();
    ~MySqlManager();

    // 初始化
    bool init(const ZhKeyes::Util::ConfigManager& config);

    // ===== 连接管理 =====
    std::shared_ptr<MySqlConnection> getConnection();
    void returnConnection(std::shared_ptr<MySqlConnection> conn);

    // ===== 基础查询操作（工具方法）=====

    // 执行查询（SELECT）
    std::unique_ptr<sql::ResultSet> executeQuery(
        std::shared_ptr<MySqlConnection> conn,
        const std::string& sql);

    // 执行更新（INSERT/UPDATE/DELETE）
    int executeUpdate(
        std::shared_ptr<MySqlConnection> conn,
        const std::string& sql);

    // 创建预处理语句
    std::unique_ptr<sql::PreparedStatement> prepareStatement(
        std::shared_ptr<MySqlConnection> conn,
        const std::string& sql);

    // 事务支持
    bool beginTransaction(std::shared_ptr<MySqlConnection> conn);
    bool commit(std::shared_ptr<MySqlConnection> conn);
    bool rollback(std::shared_ptr<MySqlConnection> conn);

    // 获取最后插入的 ID
    int getLastInsertId(std::shared_ptr<MySqlConnection> conn);

    // 连接池状态
    bool isHealthy() const;
    void close();

private:
    std::unique_ptr<MySqlConnPool> m_connPool;
};

#endif