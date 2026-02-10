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

void TcpManager::disconnect()
{
    m_spTcpClient->disconnect();
}

bool TcpManager::authenticate(uint32_t uid, const std::string& token,
    TcpResponseHandler onResponse, ErrorCallback onError/* = nullptr*/)
{
    if (!m_spTcpClient || !m_spTcpClient->isConnected())
    {
        LOG_ERROR("TcpManager: 未连接，无法发送认证消息");
        if(onError)
            onError("未建立连接,无法发送认证消息");
        return false;
    }

    ZhKeyesIM::Protocol::BinaryWriter bodyWriter;
    bodyWriter.writeUInt32(uid);           // 写入 uid
    bodyWriter.writeString(token);        // 写入 token

    ZhKeyesIM::Protocol::IMMessage authMsg(
        ZhKeyesIM::Protocol::MessageType::AUTH_REQ,
        generateSeqId(),
        bodyWriter.getData()
    );

    bool sent = sendMessage(authMsg);

    if (sent)
    {
        addPendingRequest(authMsg.getSeqId(), std::move(onResponse));
        LOG_INFO("TcpManager: 认证消息已发送, uid=%d", uid);
    }
    else 
    {
        if (onError)
            onError("消息发送失败，请检查网络问题");
    }
    return sent;
}

bool TcpManager::applyFriend(uint32_t uid, TcpResponseHandler onResponse,ErrorCallback onError)
{
    return false;
}

bool TcpManager::searchUser(uint32_t uid, TcpResponseHandler onResponse, ErrorCallback onError)
{
    if (!m_spTcpClient || !m_spTcpClient->isConnected())
    {
        LOG_ERROR("TcpManager: 未连接，无法发送认证消息");
        if (onError)
            onError("网络未连接");
        return false;
    }

    ZhKeyesIM::Protocol::BinaryWriter bodyWriter;
    bodyWriter.writeUInt32(uid);

    ZhKeyesIM::Protocol::IMMessage authMsg(
        ZhKeyesIM::Protocol::MessageType::AUTH_REQ,
        generateSeqId(),
        bodyWriter.getData()
    );

    bool sent = sendMessage(authMsg);

    if (sent)
    {
        addPendingRequest(authMsg.getSeqId(), std::move(onResponse));
        LOG_INFO("TcpManager: 认证消息已发送, uid=%d", uid);
    }
    else
    {
        if (onError)
            onError("消息发送失败，请检查网络问题");
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

uint64_t TcpManager::generateSeqId()
{
    static std::atomic<uint64_t> counter{ 1 };
    return counter.fetch_add(1);
}

void TcpManager::registerHandler(ZhKeyesIM::Protocol::MessageType type, TcpResponseHandler&& handler)
{
    if (m_dispatcher.hasRegistered(type))
    {
        m_dispatcher.updateHandler(type, std::move(handler));
    }
    else
    {
        m_dispatcher.registerHandler(type, std::move(handler));
    }

}

void TcpManager::addPendingRequest(uint64_t seqId, TcpResponseHandler&& handler)
{
    std::lock_guard<std::mutex> lock(m_pendingMutex);
    m_pendingRequests[seqId] = PendingRequest{ std::move(handler) };
}

bool TcpManager::handleResponseBySeqId(std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg, std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender)
{
    if (!msg) return false;

    uint32_t seqId = msg->getSeqId();
    if (seqId == 0) {
        // 约定：0 表示无 seqId，交给 type-dispatcher 处理
        return false;
    }

    TcpResponseHandler handler;

    {
        std::lock_guard<std::mutex> lock(m_pendingMutex);
        auto it = m_pendingRequests.find(seqId);
        if (it == m_pendingRequests.end())
            return false;

        handler = std::move(it->second.handler);
        m_pendingRequests.erase(it);
    }

    if (handler)
    {
        handler(std::move(msg), std::move(sender));
        return true;
    }
    return false;
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

        // 如果没有人等待这个SeqId 的回复， 则统一进行Dispatch
        if (!handleResponseBySeqId(msg, self))
        {
            m_dispatcher.dispatch(msg, self);
        }
        

        size_t msgLen = msg->getLength();
        buf.retrieve(msgLen);

    }
}

void TcpManager::onConnected(std::shared_ptr<TCPConnection> spConn)
{
    spConn->setReadCallback(std::bind(&TcpManager::onTcpResponse, this, std::placeholders::_1)); 
  

    if (m_connectionCallback)
        m_connectionCallback();
    releaseConnectCallback();
    return;   

}

void TcpManager::onConnectFailed()
{
    if (m_connectFailedCallback)
    {
        m_connectFailedCallback("连接失败");
        releaseConnectCallback();
    }
   
}
