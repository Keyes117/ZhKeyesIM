#include "IMUserController.h"

#include "IMProtocol/BinaryReader.h"

IMUserController::IMUserController(std::shared_ptr<IMUserService> UserService):
    m_spUserService(UserService)
{
}

void IMUserController::auth(std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg, std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender)
{
    std::string msgBody = msg->getBody();

    ZhKeyesIM::Protocol::BinaryReader reader(msgBody);
    
    uint32_t uid;
    std::string token;

    reader.readUInt32(uid);
    reader.readString(token);

    m_spUserService->auth(uid, token, sender);    
}
