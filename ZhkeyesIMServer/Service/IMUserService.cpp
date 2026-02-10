#include "IMUserService.h"

#include "IMSession.h"
#include <memory>

#include "log/Logger.h"

#include "util/Defer.h"
#include "IMProtocol/BinaryWriter.h"

IMUserService::IMUserService(std::shared_ptr<IMUserRepository> userRepo,
    std::string serverName,
    std::string serverIp, 
    uint16_t    serverPort):
    m_spUserRepo(userRepo),
    m_serverName(serverName),
    m_serverIp(serverIp),
    m_serverPort(serverPort)
{
}

void IMUserService::auth(uint32_t uid, const std::string& token, uint64_t seqId,
    std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender)
{
    ZhKeyesIM::Protocol::IMMessage msg;
    msg.setSeqId(seqId);
    msg.setType(ZhKeyesIM::Protocol::MessageType::AUTH_RESP);

    auto session = std::dynamic_pointer_cast<IMSession>(sender);
    if (session)
    {
        ZhKeyes::Util::Defer def([this, &msg, &sender]() {
            sender->sendMessage(msg);
            });

        session->setUserInfo(uid, token);

        auto tokenOpt = m_spUserRepo->getToken(uid);
        if (!tokenOpt)
        {
            //设置返回错误信息
            LOG_ERROR("IMUserService::auth: sender is not IMSession, uid=%u", uid);
            ZhKeyesIM::Protocol::BinaryWriter writer;
            writer.writeUInt8(0);
            writer.writeUInt32(uid);
            writer.writeString("Invalid Token");
            msg.setBody(writer.getData());
            return;
        }


        std::string userToken = *tokenOpt;
        if (userToken != token)
        {
            //设置返回错误信息
            LOG_WARN("IMUserService::auth: token not found, uid=%u", uid);
            ZhKeyesIM::Protocol::BinaryWriter writer;
            writer.writeUInt8(0);           // success = false
            writer.writeUInt32(uid);
            writer.writeString("Token not found or expired");
            msg.setBody(writer.getData());
            return;
        }

        auto userInfoOpt = m_spUserRepo->getUserInfo(uid);
        if (!userInfoOpt)
        {
            //设置返回错误信息
            LOG_WARN("IMUserService::auth: user info not found, uid=%u", uid);
            ZhKeyesIM::Protocol::BinaryWriter writer;
            writer.writeUInt8(0);           // success = false
            writer.writeUInt32(uid);
            writer.writeString("User info not found");
            msg.setBody(writer.getData());
            return;

        }

        UserInfo userInfo = *userInfoOpt;

        std::string sessionId = std::to_string(session->getSessionId());

        bool mappingOk = m_spUserRepo->setUserServerMapping(
            static_cast<int32_t>(uid),
            m_serverName,
            m_serverIp,
            m_serverPort,
            sessionId
        );

        if (!mappingOk)
        {
            LOG_ERROR("IMUserService::auth: setUserServerMapping failed, uid=%u", uid);
            // 映射失败可以视为严重问题，这里仍然返回失败，避免后续路由错误
            ZhKeyesIM::Protocol::BinaryWriter writer;
            writer.writeUInt8(0);           // success = false
            writer.writeUInt32(uid);
            writer.writeString("Failed to set user-server mapping");
            msg.setBody(writer.getData());
            sender->sendMessage(msg);
            return;
        }

        bool incOk = m_spUserRepo->incrementServerConnectionCount(m_serverName);
        if (!incOk)
        {
            LOG_WARN("IMUserService::auth: incrementServerConnectionCount failed, server=%s",
                m_serverName.c_str());
            // 连接数统计失败不影响核心功能，这里仅打日志，不影响成功响应
        }

        LOG_INFO("IMUserService::auth: auth success, uid=%u, server=%s(%s:%d), session=%s",
            uid, m_serverName.c_str(), m_serverIp.c_str(), m_serverPort, sessionId.c_str());

        // 5. 构造成功响应包体
        ZhKeyesIM::Protocol::BinaryWriter writer;
        writer.writeUInt8(1);                          // success = true
        writer.writeUInt32(uid);                       // userId
        writer.writeString(token);                     // token

        // 用户信息
        writer.writeString(userInfo.name);             // 用户名
        writer.writeString(userInfo.email);            // 邮箱
        writer.writeString(userInfo.nick);             // 昵称
        writer.writeString(userInfo.desc);             // 个性签名/简介
        writer.writeUInt32(userInfo.sex);              // 性别
        writer.writeString(userInfo.icon);             // 头像 URL
        writer.writeString(userInfo.back);             // 背景图 URL

        msg.setBody(writer.getData());


        sender->sendMessage(msg);
    }
}
