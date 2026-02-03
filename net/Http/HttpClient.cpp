#include "HttpClient.h"
#include "Logger.h"

using namespace ZhKeyesIM::Net::Http;


HttpClient::HttpClient(std::shared_ptr<EventLoop> eventLoop)
    : m_eventLoop(eventLoop)
{
}

HttpClient::~HttpClient()
{
}

void HttpClient::closeAll()
{
    std::lock_guard<std::mutex> lock(m_sessionsMutex);

    LOG_INFO("Closing all sessions (%zu active)", m_sessions.size());

    for (auto& [key, session] : m_sessions) {
        if (session) {
            session->close();
        }
    }
    m_sessions.clear();
}



bool HttpClient::get(const std::string& url,
    ResponseCallback responseCallback,
    ErrorCallback errorCallback)
{
    UrlInfo urlInfo = parseUrl(url);
    if (urlInfo.host.empty() || urlInfo.port == 0)
    {
        if (errorCallback)
            errorCallback("HttpClient: INVALID URL: " + url);

        return false;
    }

    std::string requesturl = buildRequestUrl(urlInfo);
    HttpRequest request(HttpMethod::GET, requesturl);
    request.setHeader("Host", urlInfo.host + ":" + std::to_string(urlInfo.port));
    request.setHeader("User-Agent", "ZhKeyes-HttpClient/1.0");
    request.setHeader("Accept", "*/*");

    std::shared_ptr<HttpClientSession> session = 
        getOrCreateSession(urlInfo.host, urlInfo.port);

    if (!session)
    {
        if (errorCallback)
            errorCallback("HttpClient: Failed to create http Client Session");

        return false;
    }

    session->sendRequest(request, responseCallback, errorCallback);
    return true;
}

bool HttpClient::post(const std::string& url,
    const std::string& body,
    ResponseCallback responseCallback,
    ErrorCallback errorCallback)
{
    UrlInfo urlInfo = parseUrl(url);
    if (urlInfo.host.empty() || urlInfo.port == 0) {
        if (errorCallback) {
            errorCallback("HttpClient: Invalid URL: " + url);
        }
        return false;
    }

    std::string requestUrl = buildRequestUrl(urlInfo);
    HttpRequest request(HttpMethod::POST, requestUrl);
    request.setHeader("Host", urlInfo.host + ":" + std::to_string(urlInfo.port));
    request.setHeader("User-Agent", "ZhKeyes-HttpClient/1.0");
    request.setHeader("Content-Type", "application/x-www-form-urlencoded");
    request.setHeader("Connection", "keep-alive");
    request.setContentLength(body.size());
    request.setBody(body);

    std::shared_ptr<HttpClientSession> session
             = getOrCreateSession(urlInfo.host, urlInfo.port);

    if (!session)
    {
        if (errorCallback)
            errorCallback("HttpClient: Failed to create http Client Session");
        return false;
    }

    session->sendRequest(request, responseCallback, errorCallback);
    return true;
}

bool HttpClient::postJson(const std::string& url,
    const std::string& jsonData,
    ResponseCallback responseCallback,
    ErrorCallback errorCallback)
{
    UrlInfo urlInfo = parseUrl(url);
    if (urlInfo.host.empty() || urlInfo.port == 0) {
        if (errorCallback) {
            errorCallback("HttpClient: Invalid URL: " + url);
        }
        return false;
    }

    std::string requestUrl = buildRequestUrl(urlInfo);
    HttpRequest request(HttpMethod::POST, requestUrl);
    request.setHeader("Host", urlInfo.host + ":" + std::to_string(urlInfo.port));
    request.setHeader("User-Agent", "ZhKeyes-HttpClient/1.0");
    request.setContentLength(jsonData.size());
    request.setJsonBody(jsonData);
    request.setHeader("Connection", "keep-alive");

    auto session = getOrCreateSession(urlInfo.host, urlInfo.port);

    if (!session)
    {
        if (errorCallback)
            errorCallback("HttpClient: Failed to create http Client Session");
        return false;
    }

    session->sendRequest(request, responseCallback, errorCallback);

    return true;
} 

bool HttpClient::postForm(const std::string& url,
    const std::unordered_map<std::string, std::string>& formData,
    ResponseCallback responseCallback,
    ErrorCallback errorCallback)
{
    UrlInfo urlInfo = parseUrl(url);
    if (urlInfo.host.empty() || urlInfo.port == 0) {
        if (errorCallback) {
            errorCallback("HttpClient: Invalid URL: " + url);
        }
        return false;
    }

    std::string requestUrl = buildRequestUrl(urlInfo);
    HttpRequest request(HttpMethod::POST, requestUrl);
    request.setHeader("Host", urlInfo.host + ":" + std::to_string(urlInfo.port));
    request.setHeader("User-Agent", "ZhKeyes-HttpClient/1.0");
    request.setHeader("Connection", "keep-alive");
    request.setFormData(formData);

    auto session = getOrCreateSession(urlInfo.host, urlInfo.port);

    if (!session)
    {
        if (errorCallback)
            errorCallback("HttpClient: Failed to create http Client Session");
        return false;
    }

    session->sendRequest(request, responseCallback, errorCallback);

    return true;
}


void ZhKeyesIM::Net::Http::HttpClient::cleanupIdleSessions(std::chrono::seconds idleTimeout)
{
    std::lock_guard<std::mutex> lock(m_sessionsMutex);
    auto now = std::chrono::steady_clock::now();

    for (auto it = m_sessions.begin(); it != m_sessions.end();) {
        auto& session = it->second;
        auto lastActivity = session->getLastActivityTime();
        auto idleTime = std::chrono::duration_cast<std::chrono::seconds>(
            now - lastActivity);

        if (idleTime > idleTimeout && !session->hasPendingRequests()) {
            LOG_INFO("HttpClient: Closing idle session: %s (idle for %lld seconds)",
                it->first.c_str(), idleTime.count());
            session->close();
            it = m_sessions.erase(it);
        }
        else {
            ++it;
        }
    }
}

HttpClient::UrlInfo HttpClient::parseUrl(const std::string& url)
{
    UrlInfo info;

    // URL 解析正则表达式
    std::regex urlRegex(R"(^(http?)://([^:/\?#]+)(?::(\d+))?([^?\#]*)(?:\?([^#]*))?(?:#.*)?$)");
    std::smatch match;

    if (std::regex_match(url, match, urlRegex))
    {
        info.scheme = match[1].str();
        info.host = match[2].str();

        // 解析端口
        if (match[3].matched)
        {
            info.port = static_cast<uint16_t>(std::stoi(match[3].str()));
        }
        else
        {
            info.port = (info.scheme == "https") ? 443 : 80;
        }

        // 解析路径
        info.path = match[4].matched ? match[4].str() : "/";
        if (info.path.empty())
        {
            info.path = "/";
        }

        // 解析查询字符串
        info.query = match[5].matched ? match[5].str() : "";
    }
    else
    {
        LOG_ERROR("HttpClient: failed to parse URL: {}", url);
    }

    return info;
}

std::string HttpClient::buildRequestUrl(const UrlInfo& urlInfo)
{
    std::string requestUrl = urlInfo.path;
    if (!urlInfo.query.empty())
    {
        requestUrl += "?" + urlInfo.query;
    }
    return requestUrl;
}

std::shared_ptr<HttpClientSession> HttpClient::getOrCreateSession(const std::string& host, uint16_t port)
{
    std::string key = host + ":" + std::to_string(port);

    std::lock_guard<std::mutex> lock(m_sessionsMutex);

    auto it = m_sessions.find(key);
    if (it != m_sessions.end()) {
        auto& session = it->second;
        if (session->isConnecting() || session->isConnected() || session->hasPendingRequests()) {
            return session;
        }

        // Session 已失效，移除
        LOG_DEBUG("HttpClient: Removing invalid session for %s", key.c_str());
        m_sessions.erase(it);
    }

    if (m_sessions.size() >= m_maxSessions) {
        LOG_ERROR("HttpClient: Too many active sessions (%zu >= %zu)",
            m_sessions.size(), m_maxSessions);
        return nullptr;
    }

    // 创建新会话
    auto session = std::make_shared<HttpClientSession>(host, port, m_eventLoop);
    session->setConnectTimeout(m_connectTimeoutMs);
    session->setRequestTimeout(m_requestTimeoutMs);
    session->setMaxRequestsPerConnection(m_maxRequestsPerConnection);
    m_sessions[key] = session;

    LOG_INFO("HttpClient: Created new session for %s:%u", host.c_str(), port);

    return session;
}
