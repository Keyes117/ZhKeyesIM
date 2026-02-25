#ifndef ZHKEYESIMCLIENT_TASK_AUTHENFRIENDAPPLYTASK_H_
#define ZHKEYESIMCLIENT_TASK_AUTHENFRIENDAPPLYTASK_H_

#include <memory>
#include <functional>
#include <string>

#include "NetWork/IMClient.h"
#include "Task/Task.h"
#include "Base/UserData.h"

#include <IMProtocol/IMMessage.h>

class AuthenFriendApplyTask : public Task
{
    Q_OBJECT
public:

    AuthenFriendApplyTask(
        Task::ConstructorKey key,
        Task::TaskId id,
        std::shared_ptr<IMClient>   m_spClient,
        uint32_t    fromUid,
        uint32_t    toUid,
        uint8_t     decision,
        std::string back_name
    );
    ~AuthenFriendApplyTask() = default;

    void doTask() override;

private:
    void onAuthenFriendApplyResponse(std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg,
        std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender);

private:

    std::shared_ptr<IMClient>   m_spClient;
    uint32_t        m_fromUid;
    uint32_t        m_toUid;
    uint8_t         m_decision;
    std::string     m_backName;
};


#endif