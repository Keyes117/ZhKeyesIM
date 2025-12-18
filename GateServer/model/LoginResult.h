#ifndef GATESERVER_MODEL_LOGINRESULT_H_
#define GATESEREVR_MODEL_LOGINRESULT_H_

#include <string>
#include "User.h"

struct LoginResult {
    bool success;
    std::string message;
    UserInfo userInfo;
    std::string token;
};

struct RegisterResult
{
    bool success;
    std::string message;
    int uid;
};

struct ResetPasswordResult
{
    bool success;
    std::string message;
};

#endif // GATESERVER_MODEL_LOGINRESULT_H_