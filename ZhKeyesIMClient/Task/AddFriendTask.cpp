#include "AddFriendTask.h"

#include "IMProtocol/BinaryReader.h"
#include "Log/Logger.h"

AddFriendTask::AddFriendTask(Task::ConstructorKey key, 
    Task::TaskId id, 
    std::shared_ptr<IMClient> client,
    uint32_t uid):
    Task(key,id, Task::TaskType::TASK_TYPE_APPLYFRIEND),
    m_spClient(client),
    m_uid(uid)           
{

}

void AddFriendTask::doTask()
{
    auto self = std::static_pointer_cast<AddFriendTask>(shared_from_this());
    std::weak_ptr<AddFriendTask> weakSelf = self;

    m_spClient->applyFriend(m_uid,
        [weakSelf](std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg,
            std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender)
        {
            if (auto selfLocked = weakSelf.lock())
            {
                selfLocked->onAddFriendResponse(msg, sender);
            }
        },
        [weakSelf](const std::string& err)
        {
            if (auto selfLocked = weakSelf.lock())
            {
                selfLocked->onTaskError(err);
            }
        });
}

void AddFriendTask::onAddFriendResponse(std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg, 
    std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender)
{
    auto fail = [this](const std::string& reason)
        {
            LOG_WARN("TcpManager: AUTH_RESP 失败: %s", reason.c_str());
            onTaskError(reason);
        };

    if (!msg || !msg->hasBody()) {
        fail("申请失败");
        return;
    }

    ZhKeyesIM::Protocol::BinaryReader reader(msg->getBody());

    uint8_t success = 0;
    uint32_t uid = 0;
    if (!reader.readUInt8(success) || !reader.readUInt32(uid)) {
        fail("申请响应解析失败");
        return;
    }

    if (success == 0) {
        std::string err;
        if (!reader.readString(err))
            err = "好友申请失败";
        fail(err);
        return;
    }

    LOG_INFO("好友申请成功");
    onTaskSuccess();

}
