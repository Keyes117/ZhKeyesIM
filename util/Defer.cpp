#include "Defer.h"

        // 接受一个lambda表达式或者函数指针
Defer::Defer(std::function<void()> func) 
        : m_func(func) 
{}

    // 析构函数中执行传入的函数
Defer::~Defer()
{
    m_func();
}