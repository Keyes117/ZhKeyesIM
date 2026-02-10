#include "TcpConnectTask.h"

#include "Base/UserSession.h"

#include "IMProtocol/BinaryReader.h"

TcpConnectTask::TcpConnectTask(
    Task::ConstructorKey key,
    Task::TaskId id,
    std::shared_ptr<IMClient> client,
    std::string ip, uint16_t port):
    Task(key, id,Task::TaskType::TASK_TYPE_TCPCONNECT),
    m_client(client), m_ip(ip), m_port(port)
{

}

void TcpConnectTask::doTask()
{
    // Task 继承自 Task(=enable_shared_from_this<Task>)，这里安全
    auto self = std::static_pointer_cast<TcpConnectTask>(shared_from_this());
    std::weak_ptr<TcpConnectTask> weakSelf = self;

    m_client->tcpConnect(
        m_ip,
        m_port,
        // 成功回调
        [weakSelf]()
        {
            if (auto selfLocked = weakSelf.lock())
            {
                selfLocked->onTcpConnected();
            }
            // 如果 Task 已经被销毁，则什么都不做
        },
        // 失败回调
        [weakSelf](const std::string& err)
        {
            if (auto selfLocked = weakSelf.lock())
            {
                selfLocked->onTaskError(err);
            }
        }
    );

}

void TcpConnectTask::onTcpConnected()
{
    const std::string token = UserSession::getInstance().getToken();
    int64_t uid = UserSession::getInstance().getUid();
    if (token.empty() || uid <= 0)
    {
        onTaskError("用户信息错误，无法进行认证");
        return;
    }
    

    auto self = std::static_pointer_cast<TcpConnectTask>(shared_from_this());
    std::weak_ptr<TcpConnectTask> weakSelf = self;

    m_client->auth(
        static_cast<uint32_t>(uid),
        token,
        [weakSelf](std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg,
            std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender)
        {
            if (auto selfLocked = weakSelf.lock())
            {
                selfLocked->onAuthResponse(std::move(msg), std::move(sender));
            }
            // Task 已经结束/销毁时，直接丢弃这次响应
        },
        [weakSelf](const std::string& err)
        {
            if (auto selfLocked = weakSelf.lock())
            {
                selfLocked->onTaskError(err);
            }
        }
    );
}

void TcpConnectTask::onAuthResponse(std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg,
    std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender)
{
    auto fail = [this](const std::string& reason)
        {
            LOG_WARN("TcpManager: AUTH_RESP 失败: %s", reason.c_str());
            onTaskError(reason);
            m_client->tcpDisconnect();
        };

    if (!msg || !msg->hasBody()) {
        fail("认证响应为空");
        return;
    }

    ZhKeyesIM::Protocol::BinaryReader reader(msg->getBody());

    uint8_t success = 0;
    uint32_t uid = 0;
    if (!reader.readUInt8(success) || !reader.readUInt32(uid)) {
        fail("认证响应解析失败");
        return;
    }

    if (success == 0) {
        std::string err;
        if (!reader.readString(err))
            err = "认证失败";
        fail(err);
        return;
    }

    // success == 1，继续解析剩余字段
    std::string token, name, email, nick, desc, icon, back;
    uint32_t sex = 0;
    if (!reader.readString(token) ||
        !reader.readString(name) ||
        !reader.readString(email) ||
        !reader.readString(nick) ||
        !reader.readString(desc) ||
        !reader.readUInt32(sex) ||
        !reader.readString(icon) ||
        !reader.readString(back))
    {
        fail("认证响应字段不完整");
        return;
    }

    LOG_INFO("TcpManager: 认证成功, uid=%u, name=%s", uid, name.c_str());

    /*  TODO:这里构造 AuthRsp 并投递到 UI 线程（TaskHandler）*/

}


