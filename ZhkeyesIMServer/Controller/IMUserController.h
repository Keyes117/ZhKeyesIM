#ifndef ZHKEYESIMSERVER_CONTROLLER_IMUSERCONTROLLER_H_
#define ZHKEYESIMSERVER_CONTROLLER_IMUSERCONTROLLER_H_

#include "IMBaseController.h"
#include "IMProtocol/IMMessage.h"
#include "IMProtocol/IMMessageSender.h"
#include "Service/IMUserService.h"
#include <memory>

class IMUserController : public IMBaseController
{
public:
    IMUserController(std::shared_ptr<IMUserService> UserService);
    virtual ~IMUserController() = default;

    void auth(std::shared_ptr<ZhKeyesIM::Protocol::IMMessage>, std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender>);

private:
    std::shared_ptr<IMUserService> m_spUserService;
};



#endif

