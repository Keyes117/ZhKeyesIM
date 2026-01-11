#include "IMUserService.h"

#include "IMSession.h"
#include <memory>

void IMUserService::auth(uint32_t uid, const std::string& token, std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender)
{
    ZhKeyesIM::Protocol::IMMessage msg;

    auto session = std::dynamic_pointer_cast<IMSession>(sender);
    if (!session)
    {
        msg.setSeqId(0);
        msg.setType(ZhKeyesIM::Protocol::MessageType::AUTH_RESP);
        session->sendMessage(msg);
    }
}
