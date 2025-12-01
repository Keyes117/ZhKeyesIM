/**
 * @desc:   HttpClient类，HttpClient.h
 * @author: ZhKeyes
 * @date:   2025/8/6
 */

#ifndef NET_HTTP_HTTPCLIENT_H_
#define NET_HTTP_HTTPCLIENT_H_


#include <cstdint>
#include <functional>
#include <memory>
#include <regex>
#include <string>


#include "EventLoop.h"
#include "HttpResponse.h"
#include "HttpClientSession.h"

#include "net_export.h"
#include "TCPClient.h"

namespace ZhKeyesIM {
    namespace Net {
    namespace Http {

class NET_API HttpClient
{
public:
    using ResponseCallback = std::function<void(const HttpResponse&)>;
    using ErrorCallback = std::function<void(const std::string&)>;


    explicit HttpClient(std::shared_ptr<EventLoop> eventLoop);

    ~HttpClient();

    void closeAll();

    // ============== HTTP 请求方法 ==============

    bool get(const std::string& url,
        ResponseCallback callback,
        ErrorCallback errorCallback = nullptr);

    bool post(const std::string& url,
        const std::string& body,
        ResponseCallback callback,
        ErrorCallback errorCallback = nullptr);

    bool postJson(const std::string& url,
        const std::string& jsonData,
        ResponseCallback callback,
        ErrorCallback errorCallback = nullptr);

    bool postForm(const std::string& url,
        const std::unordered_map<std::string, std::string>& formData,
        ResponseCallback callback,
        ErrorCallback errorCallback = nullptr);

    // ============== 配置 ==============

    /**
     * 设置连接超时（毫秒）
     */
    void setConnectionTimeout(uint32_t timeoutMs) { m_connectTimeoutMs = timeoutMs; }

    /**
     * 设置请求超时（毫秒）
     */
    void setRequestTimeout(uint32_t timeoutMs) { m_requestTimeoutMs = timeoutMs; }

    /**
    * 设置最大会话数
    */
    void setMaxSessions(size_t maxSessions) { m_maxSessions = maxSessions; }

    /**
     * 设置每个会话的最大请求数
     */
    void setMaxRequestsPerConnection(size_t max) { m_maxRequestsPerConnection = max; }

    /**
     * 清理空闲会话
     */
    void cleanupIdleSessions(std::chrono::seconds idleTimeout);

private:
    // URL 解析
    struct UrlInfo {
        std::string scheme;
        std::string host;
        uint16_t port;
        std::string path;
        std::string query;

        std::string getKey() const {
            return host + ":" + std::to_string(port);
        }
    };

    UrlInfo parseUrl(const std::string& url);
    std::string buildRequestUrl(const UrlInfo& urlInfo);

    // 会话管理
    std::shared_ptr<HttpClientSession> getOrCreateSession(const std::string& host, uint16_t port);

private:
    std::shared_ptr<EventLoop> m_eventLoop;

    // 会话池：key = "host:port", value = session
    std::unordered_map<std::string, std::shared_ptr<HttpClientSession>> m_sessions;
    std::mutex m_sessionsMutex;

    uint32_t m_connectTimeoutMs = 5000;
    uint32_t m_requestTimeoutMs = 30000;

 
    size_t m_maxSessions = 100;
    size_t m_maxRequestsPerConnection = 100;

};

    }}}

#endif //!NET_HTTP_HTTPCLIENT_H_