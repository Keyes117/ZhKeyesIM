#ifndef ZHKEYESIMSERVER_CONTROLLER_IMUSERSERVICE_H_
#define ZHKEYESIMSERVER_CONTROLLER_IMUSERSERVICE_H_

#include <memory>


#include "IMProtocol/IMMessage.h"
#include "IMProtocol/IMMessageSender.h"


#include "Repository/IMUserRepository.h"

class IMUserService
{
public:
    IMUserService(std::shared_ptr<IMUserRepository> userRepo);
    ~IMUserService() = default;

    void auth(uint32_t uid, const std::string& token, std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender);

    

private:
    std::shared_ptr< IMUserRepository> m_spUserRepo;

};


#endif

