// MySqlManager.h
#ifndef infrastructure_MYSQLMANAGER_H_
#define infrastructure_MYSQLMANAGER_H_

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

    // ��ʼ��
    bool init(const ZhKeyes::Util::ConfigManager& config);

    // ===== ���ӹ��� =====
    std::shared_ptr<MySqlConnection> getConnection();
    void returnConnection(std::shared_ptr<MySqlConnection> conn);

    // ===== ������ѯ���������߷�����=====

    // ִ�в�ѯ��SELECT��
    std::unique_ptr<sql::ResultSet> executeQuery(
        std::shared_ptr<MySqlConnection> conn,
        const std::string& sql);

    // ִ�и��£�INSERT/UPDATE/DELETE��
    int executeUpdate(
        std::shared_ptr<MySqlConnection> conn,
        const std::string& sql);

    // ����Ԥ�������
    std::unique_ptr<sql::PreparedStatement> prepareStatement(
        std::shared_ptr<MySqlConnection> conn,
        const std::string& sql);

    // ����֧��
    bool beginTransaction(std::shared_ptr<MySqlConnection> conn);
    bool commit(std::shared_ptr<MySqlConnection> conn);
    bool rollback(std::shared_ptr<MySqlConnection> conn);

    // ��ȡ������� ID
    int getLastInsertId(std::shared_ptr<MySqlConnection> conn);

    // ���ӳ�״̬
    bool isHealthy() const;
    void close();

private:
    std::unique_ptr<MySqlConnPool> m_connPool;
};

#endif