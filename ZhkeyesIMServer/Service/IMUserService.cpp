#include "IMUserService.h"

#include "IMSession.h"
#include <memory>

#include "log/Logger.h"

#include "util/Defer.h"
#include "IMProtocol/BinaryWriter.h"

IMUserService::IMUserService(std::shared_ptr<IMUserRepository> userRepo,
    std::string serverName,
    std::string serverIp, 
    uint16_t    serverPort,
    uint16_t    grpcPort):
    m_spUserRepo(userRepo),
    m_serverName(serverName),
    m_serverIp(serverIp),
    m_serverPort(serverPort),
    m_grpcPort(grpcPort)
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
        
        //设置当前用户所在服务的 ip 和 session
        std::string sessionId = std::to_string(session->getSessionId());

        bool mappingOk = m_spUserRepo->setUserServerMapping(
            static_cast<int32_t>(uid),
            m_serverName,
            m_serverIp,
            m_serverPort,
            m_grpcPort,
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

    }
}

void IMUserService::searchUser(uint32_t uid, uint64_t seqId,
    std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender)
{
    ZhKeyesIM::Protocol::IMMessage msg;
    msg.setSeqId(seqId);
    msg.setType(ZhKeyesIM::Protocol::MessageType::SEARCH_USER_RESP);

    auto session = std::dynamic_pointer_cast<IMSession>(sender);
    if (session)
    {
        ZhKeyes::Util::Defer def([this, &msg, &sender]() {
            sender->sendMessage(msg);
            });

        m_spUserRepo->getUserInfo(uid);

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

        ZhKeyesIM::Protocol::BinaryWriter writer;
        writer.writeUInt8(1);                          // success = true
        writer.writeUInt32(uid);                       // userId

        // 用户信息
        writer.writeString(userInfo.name);             // 用户名
        writer.writeString(userInfo.nick);             // 昵称
        writer.writeString(userInfo.desc);             // 个性签名/简介
        writer.writeUInt32(userInfo.sex);              // 性别
        writer.writeString(userInfo.icon);             // 头像 URL

        msg.setBody(writer.getData());
    }
}

void IMUserService::applyFriend(uint32_t toUid, uint64_t seqId, std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender)
{
    ZhKeyesIM::Protocol::IMMessage msg;
    msg.setSeqId(seqId);
    msg.setType(ZhKeyesIM::Protocol::MessageType::APPLY_USER_RESP);

    auto session = std::dynamic_pointer_cast<IMSession>(sender);
    if (!session)
    {
        LOG_ERROR("IMUserService::applyFriend: sender is not IMSession");
        return;
    }

    ZhKeyes::Util::Defer def([&msg, &sender]() {
        sender->sendMessage(msg);
        });

    uint32_t fromUid = session->getUid();

    // 1. 不能加自己为好友
    if (fromUid == toUid)
    {
        LOG_WARN("IMUserService::applyFriend: cannot apply self, uid=%u", fromUid);
        ZhKeyesIM::Protocol::BinaryWriter writer;
        writer.writeUInt8(0);           // success = false
        writer.writeUInt32(toUid);
        writer.writeString("Cannot add yourself as friend");
        msg.setBody(writer.getData());
        return;
    }

    // 2. 检查目标用户是否存在
    auto targetUserOpt = m_spUserRepo->getUserInfo(toUid);
    if (!targetUserOpt)
    {
        LOG_WARN("IMUserService::applyFriend: target user not found, toUid=%u", toUid);
        ZhKeyesIM::Protocol::BinaryWriter writer;
        writer.writeUInt8(0);           // success = false
        writer.writeUInt32(toUid);
        writer.writeString("Target user not found");
        msg.setBody(writer.getData());
        return;
    }

    // 3. 检查是否已有待处理的申请
    if (m_spUserRepo->hasPendingApply(fromUid, toUid))
    {
        LOG_WARN("IMUserService::applyFriend: already has pending apply, from=%u to=%u", fromUid, toUid);
        ZhKeyesIM::Protocol::BinaryWriter writer;
        writer.writeUInt8(0);           // success = false
        writer.writeUInt32(toUid);
        writer.writeString("Already has pending friend request");
        msg.setBody(writer.getData());
        return;
    }

    // 4. 保存好友申请到数据库
    if (!m_spUserRepo->saveFriendApply(fromUid, toUid))
    {
        LOG_ERROR("IMUserService::applyFriend: save apply failed, from=%u to=%u", fromUid, toUid);
        ZhKeyesIM::Protocol::BinaryWriter writer;
        writer.writeUInt8(0);           // success = false
        writer.writeUInt32(toUid);
        writer.writeString("Failed to save friend request");
        msg.setBody(writer.getData());
        return;
    }

    LOG_INFO("IMUserService::applyFriend: success, from=%u to=%u", fromUid, toUid);

    // 6. 构造成功响应给申请人
    {
        ZhKeyesIM::Protocol::BinaryWriter writer;
        writer.writeUInt8(1);
        writer.writeUInt32(toUid);
        msg.setBody(writer.getData());
    }

    // 7. 推送通知给目标用户（本地/跨服务器）
    if (m_notifyCallback)
    {
        // 查询申请人信息，构造推送消息
        auto fromUserOpt = m_spUserRepo->getUserInfo(static_cast<int32_t>(fromUid));
        if (fromUserOpt)
        {
            UserInfo& fromUser = *fromUserOpt;

            ZhKeyesIM::Protocol::IMMessage notifyMsg;
            notifyMsg.setType(ZhKeyesIM::Protocol::MessageType::NOTIFY_FRIEND_APPLY);
            notifyMsg.setSeqId(0);  // 服务端推送，seqId=0

            ZhKeyesIM::Protocol::BinaryWriter nw;
            nw.writeUInt32(fromUser.uid);           // from_uid
            nw.writeString(fromUser.name);          // name
            nw.writeString(fromUser.nick);          // nick
            nw.writeString(fromUser.desc);          // desc
            nw.writeUInt32(fromUser.sex);           // sex
            nw.writeString(fromUser.icon);          // icon
            notifyMsg.setBody(nw.getData());

            bool notified = m_notifyCallback(toUid, notifyMsg);
            if (notified)
            {
                LOG_INFO("IMUserService::applyFriend: notification sent to uid=%u", toUid);
            }
            else
            {
                LOG_INFO("IMUserService::applyFriend: target uid=%u offline, will load from DB on login", toUid);
            }
        }
    }
}

void IMUserService::fetchFriendApplyList(uint32_t uid, uint64_t seqId, 
    std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender)
{
    ZhKeyesIM::Protocol::IMMessage msg;
    msg.setSeqId(seqId);
    msg.setType(ZhKeyesIM::Protocol::MessageType::FETCH_FRIEND_APPLY_LIST_RESP);

    ZhKeyes::Util::Defer def([&msg, &sender]() {
        sender->sendMessage(msg);
        });

    auto session = std::dynamic_pointer_cast<IMSession>(sender);
    if (!session)
    {
        LOG_ERROR("IMUserService::getFriendApplyList: sender is not IMSession");
        ZhKeyesIM::Protocol::BinaryWriter writer;
        writer.writeUInt8(0);  // success = false
        writer.writeString("Internal server error");
        msg.setBody(writer.getData());
        return;
    }

    uint32_t uid = session->getUid();
    if (uid == 0)
    {
        LOG_WARN("IMUserService::getFriendApplyList: uid=0 (not authed)");
        ZhKeyesIM::Protocol::BinaryWriter writer;
        writer.writeUInt8(0);  // success = false
        writer.writeString("User not authed");
        msg.setBody(writer.getData());
        return;
    }

    auto infos = m_spUserRepo->getFriendAppliesForUser(static_cast<int32_t>(uid));

    ZhKeyesIM::Protocol::BinaryWriter writer;
    writer.writeUInt8(1);  // success = true
    writer.writeUInt32(static_cast<uint32_t>(infos.size()));

    // 注意顺序严格对应 ApplyInfo 构造函数：
    // ApplyInfo(int uid, QString name, QString desc,
    //           QString icon, QString nick, int sex, int status)
    for (const auto& info : infos)
    {
        writer.writeUInt32(static_cast<uint32_t>(info.fromUid)); // uid
        writer.writeString(info.name);                           // name
        writer.writeString(info.desc);                           // desc
        writer.writeString(info.icon);                           // icon
        writer.writeString(info.nick);                           // nick
        writer.writeUInt32(info.sex);                            // sex
        writer.writeUInt32(info.status);                         // status
    }

    msg.setBody(writer.getData());
}

void IMUserService::authFriendApply(uint32_t fromUid, uint32_t toUid,
    uint8_t decision,
    const std::string backName,
    uint64_t seqId, 
    std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender)
{
    ZhKeyesIM::Protocol::IMMessage respMsg;
    respMsg.setSeqId(seqId);
    respMsg.setType(ZhKeyesIM::Protocol::MessageType::AUTH_FRIEND_APPLY_RESP);

    auto session = std::dynamic_pointer_cast<IMSession>(sender);
    if (!session)
    {
        LOG_ERROR("IMUserService::authFriendApply: sender is not IMSession");
        ZhKeyesIM::Protocol::BinaryWriter writer;
        writer.writeUInt8(0);
        writer.writeUInt32(fromUid);
        writer.writeUInt32(toUid);
        writer.writeString("Internal server error");
        respMsg.setBody(writer.getData());
        sender->sendMessage(respMsg);
        return;
    }

    uint32_t currentUid = session->getUid();
    if (currentUid == 0 || currentUid != toUid)
    {
        LOG_WARN("IMUserService::authFriendApply: uid mismatch, sessionUid=%u, toUid=%u",
            currentUid, toUid);
        ZhKeyesIM::Protocol::BinaryWriter writer;
        writer.writeUInt8(0);
        writer.writeUInt32(fromUid);
        writer.writeUInt32(toUid);
        writer.writeString("User not authed or invalid toUid");
        respMsg.setBody(writer.getData());
        sender->sendMessage(respMsg);
        return;
    }

    if (fromUid == toUid)
    {
        LOG_WARN("IMUserService::authFriendApply: fromUid == toUid, uid=%u", fromUid);
        ZhKeyesIM::Protocol::BinaryWriter writer;
        writer.writeUInt8(0);
        writer.writeUInt32(fromUid);
        writer.writeUInt32(toUid);
        writer.writeString("Cannot auth self as friend");
        respMsg.setBody(writer.getData());
        sender->sendMessage(respMsg);
        return;
    }

    // 仅支持 1=同意, 2=拒绝
    if (decision != 0 && decision != 1)
    {
        ZhKeyesIM::Protocol::BinaryWriter writer;
        writer.writeUInt8(0);
        writer.writeUInt32(fromUid);
        writer.writeUInt32(toUid);
        writer.writeString("Invalid decision");
        respMsg.setBody(writer.getData());
        sender->sendMessage(respMsg);
        return;
    }

    bool ok = false;
    if (decision == 1)
    {
        // 同意：更新申请状态+建立好友关系
        ok = m_spUserRepo->acceptFriendApply(static_cast<int32_t>(fromUid),
            static_cast<int32_t>(toUid));
        if (!ok)
        {
            LOG_WARN("IMUserService::authFriendApply: acceptFriendApply failed, from=%u to=%u",
                fromUid, toUid);
        }
    }
    else // decision == 2
    {
        // 拒绝：仅更新申请状态
        ok = m_spUserRepo->rejectFriendApply(static_cast<int32_t>(fromUid),
            static_cast<int32_t>(toUid));
        if (!ok)
        {
            LOG_WARN("IMUserService::authFriendApply: rejectFriendApply failed, from=%u to=%u",
                fromUid, toUid);
        }
    }

    if (!ok)
    {
        ZhKeyesIM::Protocol::BinaryWriter writer;
        writer.writeUInt8(0);
        writer.writeUInt32(fromUid);
        writer.writeUInt32(toUid);
        writer.writeString("Failed to process friend apply");
        respMsg.setBody(writer.getData());
        sender->sendMessage(respMsg);
        return;
    }

    // 给当前客户端的成功响应
    {
        ZhKeyesIM::Protocol::BinaryWriter writer;
        writer.writeUInt8(1);           // success = 1
        writer.writeUInt32(fromUid);
        writer.writeUInt32(toUid);
        writer.writeUInt8(decision);    // 回传下决策（1=同意,2=拒绝）
        respMsg.setBody(writer.getData());
        sender->sendMessage(respMsg);
    }

    LOG_INFO("IMUserService::authFriendApply: success, decision=%u, from=%u to=%u",
        decision, fromUid, toUid);

    // ===== 通知对方客户端 =====
    if (m_notifyCallback)
    {
        // 获取当前用户（被申请人）的信息，用于通知对方
        auto selfUserOpt = m_spUserRepo->getUserInfo(static_cast<int32_t>(toUid));
        if (selfUserOpt)
        {
            const UserInfo& selfUser = *selfUserOpt;

            ZhKeyesIM::Protocol::IMMessage notifyMsg;
            notifyMsg.setType(ZhKeyesIM::Protocol::MessageType::NOTIFY_FRIEND_AUTH);
            notifyMsg.setSeqId(0); // 服务端推送，seqId=0

            ZhKeyesIM::Protocol::BinaryWriter nw;
            // 谁给谁的结果
            nw.writeUInt32(fromUid);             // 原申请人 uid（接收通知一方）
            nw.writeUInt32(toUid);               // 处理方 uid（当前用户）
            nw.writeUInt8(decision);             // 1=同意,2=拒绝

            // 处理方的用户信息，方便对方更新本地 UI / 好友列表
            nw.writeUInt32(selfUser.uid);
            nw.writeString(selfUser.name);
            nw.writeString(selfUser.nick);
            nw.writeString(selfUser.desc);
            nw.writeUInt32(selfUser.sex);
            nw.writeString(selfUser.icon);

            notifyMsg.setBody(nw.getData());

            bool notified = m_notifyCallback(fromUid, notifyMsg);
            if (notified)
            {
                LOG_INFO("IMUserService::authFriendApply: result notified to fromUid=%u", fromUid);
            }
            else
            {
                LOG_INFO("IMUserService::authFriendApply: fromUid=%u offline, will sync on login", fromUid);
            }
        }
        else
        {
            LOG_WARN("IMUserService::authFriendApply: user info not found for toUid=%u, skip notify", toUid);
        }
    }
}
