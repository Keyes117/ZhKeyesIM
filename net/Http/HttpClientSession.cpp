/**
 * @desc:   HttpClientSession 实现
 * @author: ZhKeyes
 * @date:   2025/11/30
 */

#include "HttpClientSession.h"
#include "Logger.h"

using namespace ZhKeyesIM::Net::Http;

HttpClientSession::HttpClientSession(const std::string& host,
    uint16_t port,
    std::shared_ptr<EventLoop> eventLoop)
    : m_host(host)
    , m_port(port)
    , m_eventLoop(eventLoop)
    , m_HttpParser(SessionMode::SESSION_MODE_CLIENT)
{
    m_sessionID = generateID();
    m_lastActivityTime = std::chrono::steady_clock::now();  // 
    LOG_INFO("HttpClientSession created: %s:%u (ID: %u)",
        m_host.c_str(), m_port, m_sessionID);
}

HttpClientSession::~HttpClientSession()
{
    close();
    LOG_INFO("HttpClientSession destroyed (ID: %u)", m_sessionID);
}

bool HttpClientSession::sendRequest(const HttpRequest& request,
    ResponseCallback callback,
    ErrorCallback errorCallback)
{
    std::lock_guard<std::mutex> lock(m_queueMutex);

    if (m_closed.load()) {
        if (errorCallback) {
            errorCallback("Session is closed");
        }
        return false;
    }

    // 加入队列
    PendingRequest pending;
    pending.request = request;
    pending.responseCallback = callback;
    pending.errorCallback = errorCallback;
    pending.timestamp = std::chrono::steady_clock::now();

    m_pendingRequests.push(std::move(pending));

    //  更新Session 活跃时间
    m_lastActivityTime = std::chrono::steady_clock::now();

    LOG_DEBUG("Request queued to %s:%u, queue size: %zu",
        m_host.c_str(), m_port, m_pendingRequests.size());

    // 如果未连接，发起连接
    if (!m_spConnection && !m_isConnecting) {
        startConnect();
    }
    // 如果已连接且空闲，立即处理
    else if (m_spConnection && !m_isBusy) {
        processQueue();
    }
    return true;
}

void HttpClientSession::close()
{

    std::lock_guard<std::mutex> lock(m_queueMutex);

    if (m_connector) {
        m_connector.reset();
    }

    if (m_spConnection) {
        m_spConnection.reset();
    }

    m_isConnecting.store(false);
    m_isBusy.store(false);

    // 通知所有等待的请求
    while (!m_pendingRequests.empty()) {
        auto& pending = m_pendingRequests.front();
        if (pending.errorCallback) {
            pending.errorCallback("Session closed");
        }
        m_pendingRequests.pop();
    }
}

bool HttpClientSession::isConnected() const
{
    return m_spConnection != nullptr;
}

bool HttpClientSession::hasPendingRequests()  
{
    std::lock_guard<std::mutex> lock(m_queueMutex);
    return !m_pendingRequests.empty();
}

void ZhKeyesIM::Net::Http::HttpClientSession::checkTimeout()
{
    std::lock_guard<std::mutex> lock(m_queueMutex);

    if (m_closed.load() || !m_isBusy.load() || m_pendingRequests.empty()) {
        return;
    }

    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - m_pendingRequests.front().timestamp).count();

    if (elapsed > m_requestTimeoutMs) {
        LOG_WARN("Request timeout after %lld ms for %s:%u",
            elapsed, m_host.c_str(), m_port);
        handleTimeout();
    }
}

void HttpClientSession::startConnect()
{
    m_isConnecting = true;
    m_connector = std::make_unique<TCPConnector>(m_eventLoop);

    // 设置连接成功回调
    m_connector->setConnectCallback(
        std::bind(&HttpClientSession::onConnected,this,std::placeholders::_1));

    // 设置连接失败回调
    m_connector->setConnectFailedCallback(
        std::bind(&HttpClientSession::onConnectFailed, this));

    LOG_INFO("HttpClient: Connecting to %s:%u...", m_host.c_str(), m_port);

    // 发起连接
    if (!m_connector->startConnect(m_host, m_port, m_connectTimeoutMs)) {
        m_isConnecting = false;
        m_connector.reset();
        m_eventLoop->registerCustomTask(std::bind(&HttpClientSession::onConnectFailed, this));
    }
}

void HttpClientSession::onConnected(SOCKET socket)
{
    std::lock_guard<std::mutex> lock(m_queueMutex);

    m_isConnecting = false;

    // 创建 TCPConnection
    m_spConnection = std::make_shared<TCPConnection>(socket, m_eventLoop);

    // 设置回调
    m_spConnection->setReadCallback(
        std::bind(&HttpClientSession::onRead, this, std::placeholders::_1));

    m_spConnection->setCloseCallback([this]()
        {
            m_eventLoop->registerCustomTask(std::bind(&HttpClientSession::onDisconnected, this));
        }
    );

        
    // 开始读取
    m_spConnection->startRead();

    // 连接器已完成任务
    m_connector.reset();

    LOG_INFO("Connected to %s:%u", m_host.c_str(), m_port);

    //  更新Session 活跃时间
    m_lastActivityTime = std::chrono::steady_clock::now();

    // 处理队列中的请求
    processQueue();
}

void HttpClientSession::onConnectFailed()
{
    std::lock_guard<std::mutex> lock(m_queueMutex);

    m_isConnecting.store(false);
    m_connector.reset();

    LOG_ERROR("Failed to connect to %s:%u", m_host.c_str(), m_port);

    // 通知所有等待的请求
    while (!m_pendingRequests.empty()) {
        auto& pending = m_pendingRequests.front();
        if (pending.errorCallback) {
            pending.errorCallback("Connection failed");
        }
        m_pendingRequests.pop();
    }
}

void HttpClientSession::onDisconnected()
{
    std::lock_guard<std::mutex> lock(m_queueMutex);

    LOG_WARN("Disconnected from %s:%u", m_host.c_str(), m_port);

    m_spConnection.reset();
    m_isBusy = false;

    // 如果还有等待的请求，尝试重连
    if (!m_pendingRequests.empty() && !m_isConnecting && !m_isConnecting.load()) {
        LOG_INFO("Reconnecting for pending requests...");
        startConnect();
    }
}

void HttpClientSession::onRead(Buffer& buffer)
{
    //  更新Session 活跃时间
    m_lastActivityTime = std::chrono::steady_clock::now();

    ParseResult result = m_HttpParser.feed(buffer);

    if (result == ParseResult::PARSE_RESULT_COMPLETE) {
        auto response = m_HttpParser.getResponse();
        if (response) {
            handleResponse(response);
        }
        m_HttpParser.reset();
    }
    else if (result == ParseResult::PARSE_RESULT_ERROR) {
        handleParseError();
    }
}

void HttpClientSession::handleResponse(std::shared_ptr<HttpResponse>& spResponse)
{
    ResponseCallback callback;
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);

        if (m_pendingRequests.empty()) {
            LOG_WARN("Received response but no pending request");
            return;
        }

        // 取出对应的请求
        PendingRequest pending = std::move(m_pendingRequests.front());
        m_pendingRequests.pop();
        m_isBusy = false;

        LOG_DEBUG("Response received for %s:%u, remaining queue: %zu",
            m_host.c_str(), m_port, m_pendingRequests.size());

        // 释放锁后再调用回调（避免死锁）
        callback = pending.responseCallback;

        //  更新Session 活跃时间
        m_lastActivityTime = std::chrono::steady_clock::now();
        //  检查是否需要关闭连接
        if (m_requestCount >= m_maxRequestsPerConnection) {
            LOG_INFO("Max requests per connection (%zu) reached for %s:%u, closing",
                m_maxRequestsPerConnection, m_host.c_str(), m_port);
            if (m_spConnection) {
                m_spConnection->shutdownAfterWrite();
            }
        }
        // 检查响应头是否要求关闭连接
        else if (spResponse->getConnection() == "close") {
            LOG_INFO("Server requested connection close for %s:%u",
                m_host.c_str(), m_port);
            if (m_spConnection) {
                m_spConnection->shutdownAfterWrite();
            }
        }
        else {
            processQueue();
        }
    }
    
    // 然后调用回调
    if (callback) {
        callback(*spResponse);
    }
}

void HttpClientSession::handleParseError()
{
    ErrorCallback errorCallback; 

    { 
        std::lock_guard<std::mutex> lock(m_queueMutex);

        if (m_pendingRequests.empty()) {
            return;
        }

        auto& pending = m_pendingRequests.front();
        errorCallback = std::move(pending.errorCallback); 
        m_pendingRequests.pop();
        m_isBusy = false;

        
    } // 释放锁


    if (errorCallback) {
        errorCallback("HTTP parse error");
    }
}

void HttpClientSession::processQueue()
{
    // 调用前必须已持有 m_queueMutex

    if (m_pendingRequests.empty() || m_isBusy || !m_spConnection) {
        return;
    }

    m_isBusy = true;

    // 取出第一个请求（但不从队列移除，等响应到达后再移除）
    const PendingRequest& pending = m_pendingRequests.front();

    LOG_DEBUG("Sending request to %s:%u", m_host.c_str(), m_port);

    // 发送请求
    if (!m_spConnection->send(pending.request.toString())) {
        // 发送失败
        LOG_ERROR("Failed to send request to %s:%u", m_host.c_str(), m_port);

        ErrorCallback errorCallback = pending.errorCallback;  // 提取回调
        m_pendingRequests.pop();
        m_isBusy = false;

        // 关闭连接
        if (m_spConnection) {
            m_spConnection.reset();
        }

        // 在锁外调用回调
        m_queueMutex.unlock();
        if (errorCallback) {
            errorCallback("Failed to send request");
        }
        m_queueMutex.lock();

        // 如果还有待处理请求，尝试重连
        if (!m_pendingRequests.empty() && !m_closed.load() && !m_isConnecting.load()) {
            startConnect();
        }
    }
    // 否则等待响应（在 handleResponse 中处理）
}

void ZhKeyesIM::Net::Http::HttpClientSession::handleTimeout()
{
    // 注意：调用此方法前必须已持有 m_queueMutex

    if (m_pendingRequests.empty()) {
        return;
    }

    PendingRequest& pending = m_pendingRequests.front();
    ErrorCallback errorCallback = pending.errorCallback;
    m_pendingRequests.pop();
    m_isBusy = false;

    LOG_ERROR("Request timeout for %s:%u", m_host.c_str(), m_port);

    // 关闭连接
    if (m_spConnection) {
        m_spConnection.reset();
    }

    // 先解锁再调用回调
    m_queueMutex.unlock();
    if (errorCallback) {
        errorCallback("Request timeout");
    }
    m_queueMutex.lock();

    // 如果还有待处理请求，尝试重连
    if (!m_pendingRequests.empty() && !m_closed.load() && !m_isConnecting.load()) {
        startConnect();
    }
}

uint32_t HttpClientSession::generateID()
{
    static std::atomic<uint32_t> s_sessionID{ 0 };
    return ++s_sessionID;
}