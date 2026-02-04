#include "TcpConnectTask.h"

TcpConnectTask::TcpConnectTask(
    Task::ConstructorKey key,
    std::shared_ptr<IMClient> client,
    Task::TaskId id, std::string ip, uint16_t port):
    Task(key, id,Task::TaskType::TASK_TYPE_TCPCONNECT),
    m_client(client), m_ip(ip), m_port(port)
{

}

void TcpConnectTask::doTask()
{
    m_client->connect(m_ip, m_port,
        std::bind(&TcpConnectTask::onTaskSuccess, this),
        std::bind(&TcpConnectTask::onTaskError, this, std::placeholders::_1));

}


