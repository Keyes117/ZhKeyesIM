#ifndef GATESERVER_MODEL_USER_H_
#define GATESERVER_MODEL_USER_H_

#include <string>
#include <cstdint>

struct UserInfo {
    int uid = -1;
    std::string username;
    std::string email;
    std::string passwordHash;

    int64_t createTime = 0;
    int64_t lastLoginTime = 0;
};

#endif // GATESERVER_MODEL_USER_H_