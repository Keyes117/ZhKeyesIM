/**
 * @desc:   HttpClient类，HttpClient.h
 * @author: ZhKeyes
 * @date:   2025/8/6
 */

#ifndef NET_HTTP_HTTPCLIENT_H_
#define NET_HTTP_HTTPCLIENT_H_


#include <memory>
#include <functional>
#include <string>
#include <cstdint>
#include <regex>


#include "HttpSession.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "TCPClient.h"
#include "EventLoop.h"
#include "net_export.h"

class NET_API HttpClient
{
public:
    using ResponseCallback = std::function<void(const HttpResponse&)>;
    using ErrorCallback = std::function<void(const std::string&)>;

    HttpClient();
    explicit HttpClient(std::shared_ptr<EventLoop> spEventLoop);
    ~HttpClient();

    bool connect(const std::string& url, uint32_t timeoutMs = 5000);
    bool connect(const std::string& host, uint16_t port, uint32_t timeoutMs = 5000);
    void disconnect();
    bool isConnected() const;

    bool sendRequest(const HttpRequest& request, ResponseCallback callback, ErrorCallback errorCallback = nullptr);


    bool get(const std::string& url, ResponseCallback callback, ErrorCallback errorCallback = nullptr);
    bool post(const std::string& url, const std::string& body, ResponseCallback callback, ErrorCallback errorCallback = nullptr);
    bool postJson(const std::string& url, const std::string& jsonData, ResponseCallback responseHandler, ErrorCallback errorHandler = nullptr);
    bool postForm(const std::string& url, const std::unordered_map<std::string, std::string>& formData,
        ResponseCallback responseHandler, ErrorCallback errorHandler = nullptr);

    void handleResponse(const HttpResponse& response);
    void onSessionClosed(HttpSession::SessionID sessionId);

private:
    // TCPClient 回调
    void onTcpConnected(std::shared_ptr<TCPConnection>& spConn);
    void onTcpConnectFailed();
    void onTcpDisconnected();

    // URL 解析
    struct UrlInfo {
        std::string scheme;     // http/https
        std::string host;       // 主机名
        uint16_t port;          // 端口
        std::string path;       // 路径
        std::string query;      // 查询字符串
    };

    UrlInfo parseUrl(const std::string& url);
    std::string buildRequestUrl(const UrlInfo& urlInfo);

private:
    std::shared_ptr<EventLoop>      m_eventLoop;
    std::unique_ptr<TCPClient>      m_tcpClient;
    std::shared_ptr<HttpSession>    m_session;

    ResponseCallback                m_responseHandler;
    ErrorCallback                   m_errorHandler;

    UrlInfo                        m_currentUrlInfo;  // 当前连接的URL信息
    bool                           m_isConnected = false;

}



#endif //!NET_HTTP_HTTPCLIENT_H_