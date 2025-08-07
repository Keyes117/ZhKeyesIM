#include "HttpClient.h"
#include "Logger.h"

HttpClient::HttpClient() {
    // 创建默认的事件循环
    m_eventLoop = std::make_shared<EventLoop>();
    m_tcpClient = std::make_unique<TCPClient>(m_eventLoop);

    // 设置 TCPClient 回调
    m_tcpClient->setConnectionCallback(
        std::bind(&HttpClient::onTcpConnected, this, std::placeholders::_1));
    m_tcpClient->setConnectionFailedCallback(
        std::bind(&HttpClient::onTcpConnectFailed, this));
    m_tcpClient->setDisconnectedCallback(
        std::bind(&HttpClient::onTcpDisconnected, this));
}

HttpClient::HttpClient(std::shared_ptr<EventLoop> eventLoop)
    : m_eventLoop(eventLoop) {
    m_tcpClient = std::make_unique<TCPClient>(m_eventLoop);

    // 设置 TCPClient 回调
    m_tcpClient->setConnectionCallback(
        std::bind(&HttpClient::onTcpConnected, this, std::placeholders::_1));
    m_tcpClient->setConnectionFailedCallback(
        std::bind(&HttpClient::onTcpConnectFailed, this));
    m_tcpClient->setDisconnectedCallback(
        std::bind(&HttpClient::onTcpDisconnected, this));
}

HttpClient::~HttpClient() {
    disconnect();
}

bool HttpClient::connect(const std::string& url, uint32_t timeoutMs) {
    if (m_isConnected) {
        return true;
    }

    // 解析 URL
    m_currentUrlInfo = parseUrl(url);
    if (m_currentUrlInfo.host.empty() || m_currentUrlInfo.port == 0) {
        if (m_errorHandler) {
            m_errorHandler("Invalid URL: " + url);
        }
        return false;
    }

    return connect(m_currentUrlInfo.host, m_currentUrlInfo.port, timeoutMs);
}

bool HttpClient::connect(const std::string& host, uint16_t port, uint32_t timeoutMs) {
    if (m_isConnected) {
        return true;
    }

    // 初始化并连接 TCPClient
    if (!m_tcpClient->init(host, port, timeoutMs)) {
        if (m_errorHandler) {
            m_errorHandler("Failed to initialize TCP client");
        }
        return false;
    }

    return m_tcpClient->connect();
}

void HttpClient::disconnect() {
    if (m_session) {
        m_session->close();
        m_session.reset();
    }

    if (m_tcpClient) {
        m_tcpClient->disconnect();
    }

    m_isConnected = false;
}

bool HttpClient::isConnected() const {
    return m_isConnected && m_tcpClient && m_tcpClient->isConnected();
}

bool HttpClient::sendRequest(const HttpRequest& request, ResponseCallback responseHandler,
    ErrorCallback errorHandler) {
    if (!isConnected()) {
        if (errorHandler) {
            errorHandler("Not connected");
        }
        return false;
    }

    m_responseHandler = responseHandler;
    m_errorHandler = errorHandler;

    return m_session->sendRequest(request);
}

bool HttpClient::get(const std::string& url, ResponseHandler responseHandler,
    ErrorHandler errorHandler) {
    // 解析 URL 获取路径
    UrlInfo urlInfo = parseUrl(url);
    std::string requestUrl = buildRequestUrl(urlInfo);

    HttpRequest request(HttpMethod::GET, requestUrl);
    request.setHeader("Host", urlInfo.host + ":" + std::to_string(urlInfo.port));
    request.setHeader("User-Agent", "ZhKeyes-HttpClient/1.0");
    request.setHeader("Accept", "*/*");
    request.setHeader("Connection", "keep-alive");

    return sendRequest(request, responseHandler, errorHandler);
}

bool HttpClient::post(const std::string& url, const std::string& body,
    ResponseHandler responseHandler, ErrorHandler errorHandler) {
    UrlInfo urlInfo = parseUrl(url);
    std::string requestUrl = buildRequestUrl(urlInfo);

    HttpRequest request(HttpMethod::POST, requestUrl);
    request.setHeader("Host", urlInfo.host + ":" + std::to_string(urlInfo.port));
    request.setHeader("User-Agent", "ZhKeyes-HttpClient/1.0");
    request.setHeader("Content-Type", "application/x-www-form-urlencoded");
    request.setHeader("Connection", "keep-alive");
    request.setBody(body);

    return sendRequest(request, responseHandler, errorHandler);
}

bool HttpClient::postJson(const std::string& url, const std::string& jsonData,
    ResponseHandler responseHandler, ErrorHandler errorHandler) {
    UrlInfo urlInfo = parseUrl(url);
    std::string requestUrl = buildRequestUrl(urlInfo);

    HttpRequest request(HttpMethod::POST, requestUrl);
    request.setHeader("Host", urlInfo.host + ":" + std::to_string(urlInfo.port));
    request.setHeader("User-Agent", "ZhKeyes-HttpClient/1.0");
    request.setJsonBody(jsonData);
    request.setHeader("Connection", "keep-alive");

    return sendRequest(request, responseHandler, errorHandler);
}

bool HttpClient::postForm(const std::string& url,
    const std::unordered_map<std::string, std::string>& formData,
    ResponseHandler responseHandler, ErrorHandler errorHandler) {
    UrlInfo urlInfo = parseUrl(url);
    std::string requestUrl = buildRequestUrl(urlInfo);

    HttpRequest request(HttpMethod::POST, requestUrl);
    request.setHeader("Host", urlInfo.host + ":" + std::to_string(urlInfo.port));
    request.setHeader("User-Agent", "ZhKeyes-HttpClient/1.0");
    request.setHeader("Connection", "keep-alive");
    request.setFormData(formData);

    return sendRequest(request, responseHandler, errorHandler);
}

void HttpClient::handleResponse(const HttpResponse& response) {
    if (m_responseHandler) {
        m_responseHandler(response);
    }
}

void HttpClient::onSessionClosed(HttpSession::SessionID sessionId) {
    m_isConnected = false;
    m_session.reset();

    if (m_errorHandler) {
        m_errorHandler("Session closed");
    }

    LOG_INFO("HTTP session closed, ID: " + std::to_string(sessionId));
}

void HttpClient::onTcpConnected(std::shared_ptr<TCPConnection>& spConn) {
    // 创建 HttpSession
    m_session = std::make_shared<HttpSession>(this, std::move(spConn));
    m_isConnected = true;

    LOG_INFO("HTTP client connected successfully");
}

void HttpClient::onTcpConnectFailed() {
    m_isConnected = false;

    if (m_errorHandler) {
        m_errorHandler("TCP connection failed");
    }

    LOG_ERROR("HTTP client TCP connection failed");
}

void HttpClient::onTcpDisconnected() {
    m_isConnected = false;
    m_session.reset();

    if (m_errorHandler) {
        m_errorHandler("TCP connection disconnected");
    }

    LOG_INFO("HTTP client TCP connection disconnected");
}

HttpClient::UrlInfo HttpClient::parseUrl(const std::string& url) {
    UrlInfo info;

    // 简单的URL解析正则表达式
    std::regex urlRegex(R"(^(https?)://([^:/\?#]+)(?::(\d+))?([^?\#]*)(?:\?([^#]*))?(?:#.*)?$)");
    std::smatch match;

    if (std::regex_match(url, match, urlRegex)) {
        info.scheme = match[1].str();
        info.host = match[2].str();

        // 解析端口
        if (match[3].matched) {
            info.port = static_cast<uint16_t>(std::stoi(match[3].str()));
        }
        else {
            info.port = (info.scheme == "https") ? 443 : 80;
        }

        // 解析路径
        info.path = match[4].matched ? match[4].str() : "/";
        if (info.path.empty()) {
            info.path = "/";
        }

        // 解析查询字符串
        info.query = match[5].matched ? match[5].str() : "";
    }

    return info;
}

std::string HttpClient::buildRequestUrl(const UrlInfo& urlInfo) {
    std::string requestUrl = urlInfo.path;
    if (!urlInfo.query.empty()) {
        requestUrl += "?" + urlInfo.query;
    }
    return requestUrl;
}