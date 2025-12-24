#ifndef GATESERVER_CONST_H_
#define GATESERVER_CONST_H_

#include <functional>

namespace ServerParam
{
    constexpr const char* code_prefix = "code_";
}

namespace ServerStatus
{
    enum class ErrorCodes {
        Success = 0,
        Error_Json = 1001,  //Json解析错误
        RPCFailed = 1002,  //RPC请求错误
        VarifyExpired = 1003, //验证码过期
        VarifyCodeErr = 1004, //验证码错误
        UserExist = 1005,       //用户已经存在
        PasswdInValid = 1006,
        PasswdErr = 1007,    //密码错误
        EmailNotMatch = 1008,  //邮箱不匹配
        PasswdUpFailed = 1009,  //更新密码失败
        PasswdInvalid = 1010,   //密码更新失败
        TokenInvalid = 1011,   //Token失效
        UidInvalid = 1012,  //uid无效
   
      
        InternalError = 1013,    //Http 内部错误
        ParamError = 1014,        //参数错误
        UserNotFount = 1015
    };

    enum class GrpcErrors
    {
        Success= 10000,
        RedisError = 10001,
        EmailError = 10002,
        Exception = 10003,
        Timeout = 10004
    };
}

namespace ServerUtil
{
    class Defer {
    public:
        // 接受一个lambda表达式或者函数指针
        Defer(std::function<void()> func) : m_func(func) {}

        // 析构函数中执行传入的函数
        ~Defer() {
            m_func();
        }

    private:
        std::function<void()> m_func;
    };
}

#endif