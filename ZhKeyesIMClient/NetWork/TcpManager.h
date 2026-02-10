#ifndef ZHKEYESIMCLIENT_NETWORK_TCPMANAGER_H_
#define ZHKEYESIMCLIENT_NETWORK_TCPMANAGER_H_


#include <memory>

#include "net/Buffer.h"
#include "net/EventLoop.h"
#include "net/TCPClient.h"
#include "net/TCPConnection.h"

#include "IMProtocol/IMMessageDispatcher.h"
#include "IMProtocol/IMMessageSender.h"

class TcpManager : public ZhKeyesIM::Protocol::IMMessageSender
{
public:

    using TcpResponseHandler = std::function<void(std::shared_ptr<ZhKeyesIM::Protocol::IMMessage>,
        std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender>)>;
    using ErrorCallback = std::function<void(const std::string& errorMsg)>;

    using ConnectionCallback = std::function<void()>;
    using ConnectionFailedCallback = std::function<void(const std::string&)>;

    TcpManager(std::shared_ptr<EventLoop> eventLoop);
    ~TcpManager() = default;

    bool connect(const std::string& ip, uint16_t port);

    void disconnect();

    bool authenticate(uint32_t uid, const std::string& token,
        TcpManager::TcpResponseHandler onResponse, ErrorCallback onError = nullptr);

    bool applyFriend(uint32_t uid, TcpResponseHandler onResponse, ErrorCallback onError = nullptr);

    bool searchUser(uint32_t uid, TcpResponseHandler onResponse, ErrorCallback onError = nullptr);

    void setConnectCallback(ConnectionCallback&& onSuccess)
    {
        m_connectionCallback = std::move(onSuccess);
    }
    void setConnectFailedCallback(ConnectionFailedCallback&& onFailed)
    {
        m_connectFailedCallback = std::move(onFailed);
    }

    virtual bool sendMessage(const ZhKeyesIM::Protocol::IMMessage& msg);


private:
    void releaseConnectCallback();

    uint64_t generateSeqId();

    //注册到dispatcher 
    void registerHandler(ZhKeyesIM::Protocol::MessageType type,
        TcpResponseHandler&& handler);

    //注册到 pendingResponse
    void addPendingRequest(uint64_t seqId, TcpResponseHandler&& handler);

    bool handleResponseBySeqId(
        std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg,
        std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender
    );

private:

    void onTcpResponse(Buffer& recvBuf);

    void onConnected(std::shared_ptr<TCPConnection> spConn);
    void onConnectFailed();
private:

    struct PendingRequest
    {
        TcpResponseHandler handler;
        
    };

    std::shared_ptr<EventLoop>  m_spEventLoop;
    std::unique_ptr<TCPClient>  m_spTcpClient;

    ZhKeyesIM::Protocol::IMMessageDispatcher    m_dispatcher;

    ConnectionCallback m_connectionCallback;
    ConnectionFailedCallback m_connectFailedCallback;

    std::mutex m_pendingMutex;
    std::unordered_map<uint32_t, PendingRequest> m_pendingRequests;
};


#endif