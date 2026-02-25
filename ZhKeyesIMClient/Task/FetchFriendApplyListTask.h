#ifndef ZHKEYESIMCLIENT_TASK_FETCHFRIENDAPPLYLISTTASK_H_
#define ZHKEYESIMCLIENT_TASK_FETCHFRIENDAPPLYLISTTASK_H_

#include <memory>
#include <functional>
#include <string>

#include "NetWork/IMClient.h"
#include "Task/Task.h"
#include "Base/UserData.h"
#include <IMProtocol/IMMessage.h>

class FetchFriendApplyListTask : public Task
{
    Q_OBJECT
public:

    FetchFriendApplyListTask(
        Task::ConstructorKey key,
        Task::TaskId id,
        std::shared_ptr<IMClient>   m_spClient,
        uint32_t    uid
    );
    ~FetchFriendApplyListTask() = default;

    void doTask() override;

private:
    void onFetchFriendApplyListResponse(std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg,
        std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender);

private:

    std::shared_ptr<IMClient>   m_spClient;
    uint32_t        m_uid;

};


#endif