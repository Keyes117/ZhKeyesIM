#include "TcpManager.h"

#include "IMProtocol/IMProtocol.h"
#include "IMProtocol/IMMessage.h"
#include "IMProtocol/BinaryWriter.h"
#include "IMProtocol/BinaryReader.h"
#include "Base/UserSession.h"
#include "Logger.h"
TcpManager::TcpManager(std::shared_ptr<EventLoop> eventLoop):
    m_spEventLoop(eventLoop)
{
}

bool TcpManager::connect(const std::string& ip, uint16_t port)
{
    m_spTcpClient = std::make_unique<TCPClient>(m_spEventLoop);

    if (!m_spTcpClient->init(ip, port))
    {
        LOG_ERROR("TcpManager: TcpClient初始化失败");
        return false;
    }

    m_spTcpClient->setConnectionCallback(std::bind(&TcpManager::onConnected, this, std::placeholders::_1));
    m_spTcpClient->setConnectionFailedCallback(std::bind(&TcpManager::onConnectFailed, this));

    return m_spTcpClient->connect();
}

bool TcpManager::authenticate(const std::string& token, uint32_t uid)
{
    if (!m_spTcpClient || !m_spTcpClient->isConnected())
    {
        LOG_ERROR("TcpManager: 未连接，无法发送认证消息");
        return false;
    }

    ZhKeyesIM::Protocol::BinaryWriter bodyWriter;
    bodyWriter.writeUInt32(uid);           // 写入 uid
    bodyWriter.writeString(token);        // 写入 token

    ZhKeyesIM::Protocol::IMMessage authMsg(
        ZhKeyesIM::Protocol::MessageType::AUTH_REQ,
        0,
        bodyWriter.getData()
    );

    std::string msg = authMsg.serialize();

    bool sent = m_spTcpClient->send(msg);
    if (sent) {
        LOG_INFO("TcpManager: 认证消息已发送, uid=%d", uid);
    }
    else {
        LOG_ERROR("TcpManager: 发送认证消息失败");
    }

    return sent;
}

void TcpManager::onTcpResponse(Buffer& recvBuf)
{
    while (true)
    {

        // 检查是否够一个头部
        if (recvBuf.readableBytes() < ZhKeyesIM::Protocol::HEADER_SIZE)
            break;

        // 只peek buf,不移动指针
        const char* data = recvBuf.peek();
        size_t      len = recvBuf.readableBytes();

        ZhKeyesIM::Protocol::IMMessage msg;
        if (!ZhKeyesIM::Protocol::IMMessage::deserializeFromBuffer(data, len, msg))
        {
            //这里两种情况
            // 1. 半包 
            // 2. 格式错误
            break;
        }

        //处理业务
        auto msgType = msg.getType();
        switch (msgType)
        {
        default:
        }

        size_t msgLen = msg.getLength();
        recvBuf.retrieve(msgLen);
    }
}

void TcpManager::onConnected(std::shared_ptr<TCPConnection> spConn)
{

    spConn->setReadCallback(std::bind(&TcpManager::onTcpResponse, this, std::placeholders::_1));

    const std::string token = UserSession::getInstance().getToken();
    int64_t uid = UserSession::getInstance().getUid();
    if (!token.empty() || uid < 0) {
        authenticate(token, uid);
    }
    else {
        LOG_ERROR("TcpManager: Token 或 UID 未设置，无法认证");
    }

    // 通知外部连接成功
    if (m_connectionCallback) {
        m_connectionCallback();
    }
}

void TcpManager::onConnectFailed()
{
}
