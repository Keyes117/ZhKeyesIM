#include "SearchUserTask.h"


#include "IMProtocol/BinaryReader.h"

SearchUserTask::SearchUserTask(Task::ConstructorKey key,
    Task::TaskId id,
    std::shared_ptr<IMClient> client, uint32_t uid)
    :Task(key,id, Task::TaskType::TASK_TYPE_SEARCHFRIEND),
    m_spClient(client), m_uid(uid)
{
}

void SearchUserTask::doTask()
{

    auto self = std::static_pointer_cast<SearchUserTask>(shared_from_this());
    std::weak_ptr<SearchUserTask> weakSelf = self;

    m_spClient->searchUser(m_uid,
        [weakSelf](std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg,
            std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender)
        {
            if (auto selfLocked = weakSelf.lock())
            {
                selfLocked->onSearchUserResponse(msg, sender);
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

void SearchUserTask::onSearchUserResponse(std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg,
    std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender)
{
    ZhKeyesIM::Protocol::BinaryReader reader(msg->getBody());

    uint32_t uid, sex;
    std::string name, nick, desc, icon;
    if (!reader.readUInt32(uid) ||
        !reader.readString(name) ||
        !reader.readString(nick) ||
        !reader.readString(desc) ||
        !reader.readUInt32(sex) ||
        !reader.readString(icon))
    {
        onTaskError("响应字段不完整");
        return;
    }

    auto searchInfo = std::make_shared<SearchInfo>(
        uid, name, nick, desc, sex, icon
    );

    emit userSearched(searchInfo);
    onTaskSuccess();
}
