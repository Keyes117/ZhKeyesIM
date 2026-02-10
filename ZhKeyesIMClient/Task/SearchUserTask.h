#ifndef ZHKEYESIMCLIENT_TASK_SEARCHUSERTASK_H_
#define ZHKEYESIMCLIENT_TASK_SEARCHUSERTASK_H_


#include <functional>
#include <memory>
#include <string>

#include <QObject>

#include "Base/UserData.h"
#include "Task/Task.h"
#include "NetWork/IMClient.h"


class SearchUserTask : public Task
{
    Q_OBJECT
public:
    SearchUserTask(
        Task::ConstructorKey key,
        Task::TaskId id,
        std::shared_ptr<IMClient> client,
        uint32_t uid
    );

    virtual ~SearchUserTask() override = default;

    virtual void doTask() override;

signals:
    void userSearched(std::shared_ptr<SearchInfo> searchInfo);

private:
    void onSearchUserResponse(std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg,
        std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender);

private:
    std::shared_ptr<IMClient> m_spClient;
    uint32_t m_uid;

};




#endif