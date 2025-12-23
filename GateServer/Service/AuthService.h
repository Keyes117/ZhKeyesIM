#ifndef GATESERVER_SERVICE_AUTHSERVICE_H_
#define GATESERVER_SERVICE_AUTHSERVICE_H_

#include <string>
#include "repository/RedisRepository.h"

class AuthService {
public:
    AuthService(std::shared_ptr<RedisRepository> redisRepo);
    ~AuthService() = default;

    // 密码相关
    std::string hashPassword(const std::string& password);
    bool verifyPassword(const std::string& password, const std::string& hash);
    
    // Token相关
    std::string generateToken(int uid);
    bool validateToken(const std::string& token, int& outUid);

private:
    std::shared_ptr<RedisRepository> m_redisRepository;
};

#endif // GATESERVER_SERVICE_AUTHSERVICE_H_