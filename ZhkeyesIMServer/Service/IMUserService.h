#ifndef ZHKEYESIMSERVER_CONTROLLER_IMUSERSERVICE_H_
#define ZHKEYESIMSERVER_CONTROLLER_IMUSERSERVICE_H_

#include <memory.h>

#include "IMProtocol/IMMessage.h"
#include "IMProtocol/IMMessageSender.h"

class IMUserService
{
public:
    IMUserService() = default;
    ~IMUserService() = default;

    void auth(uint32_t uid, const std::string& token, std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender);
};


#endif

