/**
 * @desc:   HttpServer类，HttpServer.h
 * @author: ZhKeyes
 * @date:   2025/8/6
 */

#ifndef NET_HTTP_HTTPSERVER_H_
#define NET_HTTP_HTTPSERVER_H_

#include <memory>

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpSession.h"
#include "net_export.h"
#include "TCPServer.h"

namespace ZhKeyesIM {
    namespace Net {
        namespace Http {

            class NET_API HttpServer final
            {
            public:

                using RequestCallBack = std::function<void(const HttpRequest&, HttpResponse&)>;

                using AsyncDone = std::function<void(HttpResponse&&)>;
                using AsyncRequestCallBack = std::function<void(const HttpRequest&, AsyncDone)>;

                HttpServer() = default;
                ~HttpServer();

                bool init(uint32_t threadNum, const std::string& ip = "",
                    uint16_t port = 80, IOMultiplexType type = IOMultiplexType::Epoll);
                void start();
                void shutdown();

                //Session管理
                size_t getActiveSessionCount() const { return m_sessions.size(); }
                void cleanupSessions();

                // 内部回调
                void handleRequest(const HttpRequest& request, HttpResponse& response);

                void handleRequestAsync(const HttpRequest& requst, AsyncDone done);

                //请求回调
                void setRequestCallBack(RequestCallBack&& callback)
                {
                    m_requestCallBack = std::move(callback);
                }

                void setAsyncRequestCallBack(AsyncRequestCallBack&& callback)
                {
                    m_asyncRequestCallBack = std::move(callback);
                }

                void onDisConnected(SOCKET clientSocket);
            private:
                void onConnected(std::shared_ptr<TCPConnection>& spConn);


            private:
                std::unique_ptr<TCPServer>                         m_spTcpServer;
                std::unordered_map<HttpSession::SessionID, std::shared_ptr<HttpSession>>    m_sessions;
                std::unordered_map<SOCKET, HttpSession::SessionID> m_socketToSession;
                std::vector<std::shared_ptr<HttpSession>> m_pendingToDeleteSessions;

                mutable std::mutex      m_sessionMutex;
                RequestCallBack m_requestCallBack;
                AsyncRequestCallBack m_asyncRequestCallBack;
            };

        }
    }
}
#endif //!NET_HTTP_HTTPSERVER_H_