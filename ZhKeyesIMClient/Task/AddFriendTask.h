#ifndef ZHKEYESIMCLIENT_TASK_ADDFRIENDTASK_H_
#define ZHKEYESIMCLIENT_TASK_ADDFRIENDTASK_H_

#include <memory>
#include <functional>
#include <string>

#include "NetWork/IMClient.h"
#include "Task/Task.h"
#include "Base/UserData.h"

class AddFriendTask : public Task
{
    Q_OBJECT
public:

    AddFriendTask(
        Task::ConstructorKey key,
        Task::TaskId id,
        std::shared_ptr<IMClient>   m_spClient,
        uint32_t    uid
        );
    ~AddFriendTask() = default;

    void doTask() override;

signals:
    void applySuccess(std::shared_ptr<AddFriendApply> searchInfo);
    void applyFailed(const QString& reason);

private:
    void onAddFriendResponse(std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg,
        std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender);

private:

    std::shared_ptr<IMClient>   m_spClient;
    uint32_t        m_uid;
    
};


#endif