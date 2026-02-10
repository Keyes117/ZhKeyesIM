#ifndef ZHKEYESIMCLIENT_TASK_TCPCONNECTTASK_H_
#define ZHKEYESIMCLIENT_TASK_TCPCONNECTTASK_H_

#include <functional>
#include <memory>
#include <string>

#include "NetWork/IMClient.h"
#include "Task/Task.h"
#include "Base/global.h"


class TcpConnectTask : public Task
{
    Q_OBJECT

public:
    TcpConnectTask(
        Task::ConstructorKey key,
        Task::TaskId id,
        std::shared_ptr<IMClient> client,
        std::string ip,
        uint16_t port);


    virtual ~TcpConnectTask() override = default;

    virtual void doTask() override;

    void onTcpConnected();

    void onAuthResponse(std::shared_ptr<ZhKeyesIM::Protocol::IMMessage>,
        std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender>);

private:
    std::shared_ptr<IMClient> m_client;
    std::string m_ip;
    uint16_t m_port;
};


#endif