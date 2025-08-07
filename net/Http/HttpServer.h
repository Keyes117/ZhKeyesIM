/**
 * @desc:   HttpServer类，HttpServer.h
 * @author: ZhKeyes
 * @date:   2025/8/6
 */

 #ifndef NET_HTTP_HTTPSERVER_H_
 #define NET_HTTP_HTTPSERVER_H_

#include <memory>

#include "HttpSession.h"
#include "TCPServer.h"
#include "net_export.h"

class NET_API HttpServer final
{
public:

    using RequestCallBack = std::function<void(const HttpRequest&, HttpResponse&)>;

    HttpServer() = default;
    ~HttpServer() = default;

    bool init(uint32_t threadNum, const std::string& ip = "");
    void shutdown();

    //Session管理
    size_t getActiveSessionCount() const; {return m_sessions.size();}
    void cleanupSessions();

    // 内部回调
    void handleRequest(const HttpRequest& request, HttpResponse& response);
    //请求回调
    void setRequestCallBack(RequestCallBack&& callback)
    {
        m_requestCallBack = std::move(callback); 
    }
private:
    void onConnected(std::shared_ptr<TCPConnection>& spConn);
    void onDisConnected(HttpSession::SessionID sessionID);

private:
    std::unique_ptr<TCPServer>                         m_spTcpServer;
    std::unordered_map<HttpSession::SessionID, std::shared_ptr<HttpSession>>    m_sessions;
    std::vector<std::shared_ptr<HttpSession>> m_pendingToDeleteSessions;
    
    RequestCallBack m_requestCallBack; 
};


 #endif //!NET_HTTP_HTTPSERVER_H_