#ifndef GATESERVER_MODEL_USER_H_
#define GATESERVER_MODEL_USER_H_

#include <string>
#include <cstdint>

struct UserInfo {
    int uid;
    std::string username;
    std::string email;
    std::string passwordHash;
};

#endif // GATESERVER_MODEL_USER_H_