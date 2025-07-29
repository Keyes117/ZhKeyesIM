#ifndef  UTILS_UTIL_H_
#define UTILS_UTIL_H_

#include "util_export.h"

class UTIL_API Util
{
public:
    static void crash();

private:
    Util() = delete;
};

#endif // ! UTILS_UTIL_H_