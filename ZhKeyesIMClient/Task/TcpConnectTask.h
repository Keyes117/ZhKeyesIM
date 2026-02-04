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
        std::shared_ptr<IMClient> client,
        Task::TaskId id,
        std::string ip,
        uint16_t port);


    virtual ~TcpConnectTask() override = default;

    virtual void doTask() override;


private:
    std::shared_ptr<IMClient> m_client;
    std::string m_ip;
    uint16_t m_port;
};


#endif