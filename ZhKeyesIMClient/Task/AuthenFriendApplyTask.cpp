#include "Task/AuthenFriendApplyTask.h"

#include "IMProtocol/BinaryReader.h"

AuthenFriendApplyTask::AuthenFriendApplyTask(Task::ConstructorKey key,
    Task::TaskId id, 
    std::shared_ptr<IMClient> m_spClient,
    uint32_t fromUid, uint32_t toUid,
    uint8_t decision,
    const std::string back_name):
    Task(key,id, Task::TaskType::TASK_TYPE_AUTHENFRIENDAPPLYTASK),
    m_fromUid(fromUid),
    m_toUid(toUid),
    m_decision(decision),
    m_backName(back_name)
{
}

void AuthenFriendApplyTask::doTask()
{
    auto self = std::static_pointer_cast<AuthenFriendApplyTask>(shared_from_this());
    std::weak_ptr<AuthenFriendApplyTask> weakSelf = self;

    // 调用 IMClient 的方法发送请求
    m_spClient->authenFriendApply(
        m_fromUid,m_toUid,m_decision,m_backName,
        [weakSelf](std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg,
            std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender) {
                if (auto selfLocked = weakSelf.lock()) {
                    selfLocked->onAuthenFriendApplyResponse(msg, sender);
                }
        },
        [weakSelf](const std::string& err) {
            if (auto selfLocked = weakSelf.lock()) {
                selfLocked->onTaskError(err);
            }
        }
    );
}

void AuthenFriendApplyTask::onAuthenFriendApplyResponse(std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg, std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender)
{
    auto fail = [this](const std::string& reason)
        {
            LOG_WARN("AuthenFriendApplyTask:  失败: %s", reason.c_str());
            onTaskError(reason);
        };

    if (!msg || !msg->hasBody()) {
        fail("网络异常");
        return;
    }

    ZhKeyesIM::Protocol::BinaryReader reader(msg->getBody());

    uint8_t success = 0;
    uint32_t uid = 0, toUid = 0;
    std::string errorMsg;
    if (!reader.readUInt8(success) || 
        !reader.readUInt32(uid) || 
        !reader.readUInt32(toUid)   
       )
    {
        fail("申请响应解析失败");
        return;
    }

    if (success == 0) {
        std::string err;
        if (!reader.readString(err))
            err = "收取错误信息失败";
        fail(err);
        return;
    }

    onTaskSuccess();
}
