#include "TcpManager.h"


#include "Base/UserSession.h"
#include "log/Logger.h"
#include "IMProtocol/IMProtocol.h"
#include "IMProtocol/IMMessage.h"
#include "IMProtocol/BinaryWriter.h"
#include "IMProtocol/BinaryReader.h"

TcpManager::TcpManager(std::shared_ptr<EventLoop> eventLoop):
    m_spEventLoop(eventLoop)
{
}

bool TcpManager::connect(const std::string& ip, uint16_t port)
{
    m_spTcpClient = std::make_unique<TCPClient>(m_spEventLoop);

    if (!m_spTcpClient->init(ip, port))
    {
        releaseConnectCallback();
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

    bool sent = sendMessage(authMsg);

    if (sent) {
        LOG_INFO("TcpManager: 认证消息已发送, uid=%d", uid);
    }
    else {
        LOG_ERROR("TcpManager: 发送认证消息失败");
        if (m_connectFailedCallback)
            m_connectFailedCallback("发送认证消息失败");
        releaseConnectCallback();
    }

    return sent;
}

bool TcpManager::sendMessage(const ZhKeyesIM::Protocol::IMMessage& msg)
{
    return m_spTcpClient->send(msg.serialize());

}

void TcpManager::releaseConnectCallback()
{
    m_connectFailedCallback = nullptr;
    m_connectionCallback = nullptr;
}

void TcpManager::registerHandler()
{
    m_dispatcher.registerHandler(ZhKeyesIM::Protocol::MessageType::AUTH_RESP,
        std::bind(&TcpManager::handleAuthResponse, this, std::placeholders::_1, std::placeholders::_2)
    );
}

void TcpManager::handleAuthResponse(std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg, std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender)
{
    auto fail = [this](const std::string& reason) {
        LOG_WARN("TcpManager: AUTH_RESP 失败: %s", reason.c_str());
        if (m_connectFailedCallback)
            m_connectFailedCallback(reason);
        if (m_spTcpClient)
            m_spTcpClient->disconnect();
        releaseConnectCallback();
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
        !reader.readString(back)) {
        fail("认证响应字段不完整");
        return;
    }

    LOG_INFO("TcpManager: 认证成功, uid=%u, name=%s", uid, name.c_str());

   /*  TODO: 如有需要，这里可以构造 AuthRsp 并投递到 UI 线程（TaskHandler）*/

    // 最终认为“连接（含认证）成功”
    if (m_connectionCallback)
        m_connectionCallback();
    releaseConnectCallback();
}

void TcpManager::onTcpResponse(Buffer& buf)
{
   while (true)
    {
        //如果数据不够一个头部大小，直接退出等待更多数据
        if (buf.readableBytes() < ZhKeyesIM::Protocol::HEADER_SIZE)
            break;

        const char* data = buf.peek();
        size_t      readable = buf.readableBytes();

        bool foundMagic = false;
        size_t magicOffset = 0;
        
        size_t searchLimit = std::min(readable - ZhKeyesIM::Protocol::HEADER_SIZE + 1, 
            ZhKeyesIM::Protocol::MAX_PACKET_SIZE);

          // 在可读数据中搜索 magic（最多搜索到 readable - HEADER_SIZE + 1 的位置）
        //magic 是 uint32_t，需要按字节对齐搜索
        for (size_t i = 0; i <= searchLimit; ++i)
        {
            // 检查当前位置是否可能是 magic（需要至少 4 字节）
            if (i + 4 <= readable)
            {
                const uint32_t* magicPtr = reinterpret_cast<const uint32_t*>(data + i);
                if (*magicPtr == ZhKeyesIM::Protocol::PROTOCOL_MAGIC)
                {
                    foundMagic = true;
                    magicOffset = i;
                    break;
                }
            }
        }
        
        // 如果没找到 magic
        if (!foundMagic)
        {
          // 理论上，如果搜索超过一个最大包的大小还没找到 magic，说明肯定有问题
            if (readable > ZhKeyesIM::Protocol::MAX_PACKET_SIZE)
            {                
                buf.retrieveAll();
                break;
            }
            // 如果数据还不够多，可能 magic 在下一批数据中，等待更多数据
            // 但为了安全，如果当前数据已经超过 HEADER_SIZE，应该丢弃一个字节继续查找
            if (readable >= ZhKeyesIM::Protocol::HEADER_SIZE)
            {
                // 丢弃一个字节，继续查找
                buf.retrieve(1);
                continue;  // 继续下一次循环
            }
            break;
        }
        
        // 找到了 magic，如果不在开头，丢弃前面的垃圾数据
        if (magicOffset > 0)
        {
            LOG_WARN("IMSession::onRead, 发现 %zu 字节垃圾数据，已丢弃", magicOffset);
            buf.retrieve(magicOffset);
            // 重新获取数据指针和长度
            data = buf.peek();
            readable = buf.readableBytes();
        }


        auto msg = ZhKeyesIM::Protocol::IMMessage::deserializeFromBuffer(data, readable);
        if (!msg)
            break;
         
        auto self = shared_from_this();
        m_dispatcher.dispatch(msg,self);

        size_t msgLen = msg->getLength();
        buf.retrieve(msgLen);

    }
}

void TcpManager::onConnected(std::shared_ptr<TCPConnection> spConn)
{

    spConn->setReadCallback(std::bind(&TcpManager::onTcpResponse, this, std::placeholders::_1));

    const std::string token = UserSession::getInstance().getToken();
    int64_t uid = UserSession::getInstance().getUid();
    if (!token.empty() && uid > 0) {
        authenticate(token, uid);
    }
    else {
        LOG_ERROR("TcpManager: Token 或 UID 未设置，无法认证");
        if(m_connectFailedCallback)
            m_connectFailedCallback("用户信息错误，认证失败");
        m_spTcpClient->disconnect();
        releaseConnectCallback();
        return;
    }

}

void TcpManager::onConnectFailed()
{
    if (m_connectFailedCallback)
    {
        m_connectFailedCallback("连接失败");
        releaseConnectCallback();
    }
   
}
