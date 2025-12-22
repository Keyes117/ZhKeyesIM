#ifndef GATESERVER_MODEL_SERVICERESULT_H_
#define GATESERVER_MODEL_SERVICERESULT_H_

#include <string>
#include "model/User.h"

struct LoginResult {
    bool success;
    std::string message;
    UserInfo userInfo;
    ServerStatus::ErrorCodes code;
    std::string token;

    static LoginResult createSuccss(const UserInfo& user, 
        const std::string& token)
    {
        LoginResult result;
        result.success = true;
        result.message = "Login successful";
        result.userInfo = user;
        result.token = token;
        return result;
    }

    static LoginResult createFailure(const std::string& msg,
        ServerStatus::ErrorCodes code)
    {
        LoginResult result;
        result.success = false;
        result.message = msg;
        return result;
    }
};

struct RegisterResult {
    bool success;
    std::string message;
    ServerStatus::ErrorCodes code;
    int uid = -1;


    static RegisterResult createSuccess(int uid)
    {
        RegisterResult result;
        result.success = true;
        result.message = "Register successful";
        result.uid = uid;
        return result;
    }


    static RegisterResult createFailure(const std::string& msg,
        ServerStatus::ErrorCodes code)
    {
        RegisterResult result;
        result.success = false;
        result.message = msg;
        return result;
    }    
};

struct ResetPasswordResult {
    bool success;
    ServerStatus::ErrorCodes code;
    std::string message;

    static ResetPasswordResult createSuccess() {
        ResetPasswordResult result;
        result.success = true;
        result.message = "Password reset successful";
        return result;
    }
    
    static ResetPasswordResult createFailure(const std::string& msg,
        ServerStatus::ErrorCodes code) {
        ResetPasswordResult result;
        result.success = false;
        result.message = msg;
        return result;
    }
};

struct VerifyCodeResult {
    bool success = false;
    ServerStatus::ErrorCodes code;
    std::string message;
    
    static VerifyCodeResult createSuccess()
    {
        VerifyCodeResult result;
        result.success = true;
        result.code = ServerStatus::ErrorCodes::Success;
        result.message = "Verification code sent";
        return result;
    }
    
    static VerifyCodeResult createFailure(const std::string& msg,
        ServerStatus::ErrorCodes code) 
    {
        VerifyCodeResult result;
        result.success = false;
        result.code = code;
        result.message = msg;
        return result;
    }
};


#endif