#include "IMSession.h"

#include "IMProtocol/IMProtocol.h"
#include "IMProtocol/IMMessage.h"
#include "IMProtocol/BinaryWriter.h"
#include "IMProtocol/BinaryReader.h"

#include "IMServer.h"
#include "Logger.h"

IMSession::IMSession(IMServer* server, std::shared_ptr<TCPConnection> spConn)
    :m_pServer(server),
    m_spConn(spConn)
{
    m_sessionId = generateID();

    spConn->setReadCallback(std::bind(&IMSession::onRead,this,std::placeholders::_1));
    spConn->setWriteCallback(std::bind(&IMSession::onWrite, this));

}

bool IMSession::sendMessage(const ZhKeyesIM::Protocol::IMMessage& msg)
{
    std::string msgData = msg.serialize();
    return m_spConn->send(msgData);
}

void IMSession::setUserInfo(uint32_t uid, const std::string& token)
{
    m_uid = uid;
    m_token = token;
    m_pServer->
}

uint32_t IMSession::generateID()
{
    static std::atomic<uint32_t> s_sessionID = 0;
    uint32_t result = ++s_sessionID;
    return result;
}

void IMSession::onRead(Buffer& buf)
{

    LOG_INFO("IMSession::onRead,接收导数据，开始解析....");
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
        m_pServer->handleMsg(msg,self);

        size_t msgLen = msg->getLength();
        buf.retrieve(msgLen);

    }
}

void IMSession::onWrite()
{
}
