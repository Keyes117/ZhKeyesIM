#ifndef  UtilS_Util_H_
#define UtilS_Util_H_

#include "util_export.h"

class Util_API Util
{
public:
    static void crash();

private:
    Util() = delete;
};

#endif // ! UtilS_Util_H_