#include "IMUserService.h"

#include "IMSession.h"
#include <memory>

IMUserService::IMUserService(std::shared_ptr<IMUserRepository> userRepo):
    m_spUserRepo(userRepo)
{
}

void IMUserService::auth(uint32_t uid, const std::string& token, std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender)
{
    ZhKeyesIM::Protocol::IMMessage msg;

    auto session = std::dynamic_pointer_cast<IMSession>(sender);
    if (session)
    {
        msg.setSeqId(0);
        msg.setType(ZhKeyesIM::Protocol::MessageType::AUTH_RESP);

        session->setUserInfo(uid, token);

        auto tokenOpt = m_spUserRepo->getToken(uid);
        if (!tokenOpt)
        {
            //设置返回错误信息
        }


        std::string userToken = *tokenOpt;
        if (userToken != token)
        {
            //设置返回错误信息
        }

        


        sender->sendMessage(msg);
    }
}
