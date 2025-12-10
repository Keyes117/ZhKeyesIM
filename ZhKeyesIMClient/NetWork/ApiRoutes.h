#ifndef ZHKEYESIMCLIENT_NETWORK_APIROUTES_H_
#define ZHKEYESIMCLIENT_NETWORK_APIROUTES_H_

#include <string>

namespace ApiRoutes
{
    constexpr const char* API_VERIFY_GETCODE = "/api/verify/getCode";

    constexpr const char* API_USER_REGISTER = "/api/user/register"; 

    constexpr const char* API_USER_RESETPASS = "/api/user/resetPassword";

    constexpr const char* API_USER_LOGIN = "/api/user/login";

}


#endif //!ZHKEYESIMCLIENT_NETWORK_APIROUTES_H_