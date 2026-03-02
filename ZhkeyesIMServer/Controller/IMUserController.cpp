#include "IMUserController.h"

#include "IMProtocol/BinaryReader.h"
#include "IMProtocol/BinaryWriter.h"

IMUserController::IMUserController(std::shared_ptr<IMUserService> UserService):
    m_spUserService(UserService)
{
}

void IMUserController::auth(std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg, 
    std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender)
{
    std::string msgBody = msg->getBody();

    ZhKeyesIM::Protocol::BinaryReader reader(msgBody);
    
    uint64_t seqId = msg->getSeqId();
    uint32_t uid;
    std::string token;

    reader.readUInt32(uid);
    reader.readString(token);

    m_spUserService->auth(uid, token,seqId, sender);    
}

void IMUserController::searchUser(std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg,
    std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender)
{
    std::string msgBody = msg->getBody();

    ZhKeyesIM::Protocol::BinaryReader reader(msgBody);

    uint64_t seqId = msg->getSeqId();
    uint32_t uid;

    if (!reader.readUInt32(uid))
    {
        //TODO: ·¢ËÍ´íÎóÐÅÏ¢

        return;
    }

    m_spUserService->searchUser(uid, seqId, sender);
    
}

void IMUserController::applyFriend(std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg,
    std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender)
{
    std::string msgBody = msg->getBody();
    ZhKeyesIM::Protocol::BinaryReader reader(msgBody);

    uint64_t seqId = msg->getSeqId();
    uint32_t toUid;

    if (!reader.readUInt32(toUid))
    {
        // ½âÎöÊ§°Ü£¬·¢ËÍ´íÎóÏìÓ¦
        ZhKeyesIM::Protocol::IMMessage resp;
        resp.setSeqId(seqId);
        resp.setType(ZhKeyesIM::Protocol::MessageType::APPLY_USER_RESP);

        ZhKeyesIM::Protocol::BinaryWriter writer;
        writer.writeUInt8(0);           // success = false
        writer.writeUInt32(0);
        writer.writeString("Invalid request data");
        resp.setBody(writer.getData());
        sender->sendMessage(resp);
        return;
    }

    m_spUserService->applyFriend(toUid, seqId, sender);
}

void IMUserController::getFriendApplyList(std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg,
    std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender)
{
    std::string msgBody = msg->getBody();
    ZhKeyesIM::Protocol::BinaryReader reader(msgBody);

    uint64_t seqId = msg->getSeqId();
    uint32_t uid;

    if (!reader.readUInt32(uid))
    {
        // ½âÎöÊ§°Ü£¬·¢ËÍ´íÎóÏìÓ¦
        ZhKeyesIM::Protocol::IMMessage resp;
        resp.setSeqId(seqId);
        resp.setType(ZhKeyesIM::Protocol::MessageType::APPLY_USER_RESP);

        ZhKeyesIM::Protocol::BinaryWriter writer;
        writer.writeUInt8(0);           // success = false
        writer.writeUInt32(0);
        writer.writeString("Invalid request data");
        resp.setBody(writer.getData());
        sender->sendMessage(resp);
        return;
    }

    m_spUserService->fetchFriendApplyList(uid, seqId, sender);
}

void IMUserController::authApplyFriend(std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg,
    std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender)
{
    std::string msgBody = msg->getBody();
    ZhKeyesIM::Protocol::BinaryReader reader(msgBody);

    uint64_t seqId = msg->getSeqId();
    uint32_t uid, toUid;
    uint8_t decision;
    std::string backName;

    if (!reader.readUInt32(uid) ||
        !reader.readUInt32(toUid) ||
        !reader.readUInt8(decision) ||
        !reader.readString(backName))
    {
        // ½âÎöÊ§°Ü£¬·¢ËÍ´íÎóÏìÓ¦
        ZhKeyesIM::Protocol::IMMessage resp;
        resp.setSeqId(seqId);
        resp.setType(ZhKeyesIM::Protocol::MessageType::APPLY_USER_RESP);

        ZhKeyesIM::Protocol::BinaryWriter writer;
        writer.writeUInt8(0);           // success = false
        writer.writeUInt32(0);
        writer.writeString("Invalid request data");
        resp.setBody(writer.getData());
        sender->sendMessage(resp);
        return;
    }


    m_spUserService->authFriendApply(uid, seqId, decision,
        backName, seqId,sender);
}
