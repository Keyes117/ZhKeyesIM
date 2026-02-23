#include "AddFriendTask.h"


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
     
}
