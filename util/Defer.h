#ifndef UTIL_DEFER_H_
#define UTIL_DEFER_H_

#include <functional>

#include "util_export.h"

class Util_API Defer
{
public:
    // 接受一个lambda表达式或者函数指针
    Defer(std::function<void()> func);
    // 析构函数中执行传入的函数
    ~Defer();

private:
    std::function<void()> m_func;
};
#endif