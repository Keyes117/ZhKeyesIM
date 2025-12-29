/**
 * @desc:   HttpClientSession - 客户端会话（参考 POCO HTTPClientSession）
 * @author: ZhKeyes
 * @date:   2025/11/30
 */

#ifndef NET_HTTP_HTTPCLIENTSESSION_H_
#define NET_HTTP_HTTPCLIENTSESSION_H_

#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <string>

#include "Buffer.h"
#include "EventLoop.h"
#include "HttpParser.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "net_export.h"
#include "TCPConnection.h"
#include "TCPConnector.h"

namespace ZhKeyesIM {
    namespace Net {
        namespace Http {

            class NET_API HttpClientSession
            {
            public:
                using ResponseCallback = std::function<void(const HttpResponse&)>;
                using ErrorCallback = std::function<void(const std::string&)>;
                using SessionID = uint32_t;

                /**
                 * 构造函数
                 * @param host 服务器地址
                 * @param port 服务器端口
                 * @param eventLoop 事件循环
                 */
                HttpClientSession(const std::string& host,
                    uint16_t port,
                    std::shared_ptr<EventLoop> eventLoop);

                ~HttpClientSession();

                /**
                 * 发送请求（异步）
                 *
                 * 如果未连接，会自动发起连接
                 * 如果有请求正在处理，会加入队列
                 *
                 * @param request HTTP请求
                 * @param callback 响应回调
                 * @param errorCallback 错误回调
                 */
                bool sendRequest(const HttpRequest& request,
                    ResponseCallback callback,
                    ErrorCallback errorCallback = nullptr);

                /**
                 * 关闭会话
                 */
                void close();

      

                /**
                 * 是否已连接
                 */
                bool isConnected() const;

                /**
                 * 获取会话ID
                 */
                SessionID getID() const { return m_sessionID; }

                /**
                 * 获取目标地址
                 */
                const std::string& getHost() const { return m_host; }
                uint16_t getPort() const { return m_port; }

                bool hasPendingRequests();
                /**
                * 是否正在连接中
                */
                bool isConnecting() const { return m_isConnecting.load(); }

                bool isClosed() const { return m_closed.load(); }
                // ============== 配置方法 ==============

                /**
                 * 设置连接超时（毫秒）
                 */
                void setConnectTimeout(uint32_t timeoutMs) { m_connectTimeoutMs = timeoutMs; }

                /**
                 * 设置请求超时（毫秒）
                 */
                void setRequestTimeout(uint32_t timeoutMs) { m_requestTimeoutMs = timeoutMs; }

                /**
                 * 设置 Keep-Alive
                 */
                void setKeepAlive(bool enable) { m_keepAlive = enable; }

                /**
                 * 设置每个连接的最大请求数
                 */
                void setMaxRequestsPerConnection(size_t max) { m_maxRequestsPerConnection = max; }

                /**
                 * 检查超时（应该在 EventLoop 中定期调用）
                 */
                void checkTimeout();

                std::chrono::steady_clock::time_point getLastActivityTime() const {
                    return m_lastActivityTime;
                }

            private:
                // 连接管理
                void startConnect();
                void onConnected(SOCKET socket);
                void onConnectFailed();
                void onDisconnected();

                // 数据处理
                void onRead(Buffer& buffer);
                void handleResponse(std::shared_ptr<HttpResponse>& spResponse);
                void handleParseError();

                // 队列处理
                void processQueue();

                // 处理超时
                void handleTimeout();    
 

                static uint32_t generateID();

            private:
                // 请求队列项
                struct PendingRequest {
                    HttpRequest request;
                    ResponseCallback responseCallback;
                    ErrorCallback errorCallback;
                    std::chrono::steady_clock::time_point timestamp;
                };

                // 会话信息
                std::atomic<bool> m_closed{ false };  // 会话关闭标志
                SessionID m_sessionID;
                std::string m_host;
                uint16_t m_port;
                std::shared_ptr<EventLoop> m_eventLoop;

                // 连接管理
                std::shared_ptr<TCPConnector> m_connector;
                std::shared_ptr<TCPConnection> m_spConnection;
       
                // HTTP 协议
                HttpParser m_HttpParser;

                // 请求队列
                std::queue<PendingRequest> m_pendingRequests;
                std::mutex m_queueMutex;

                std::atomic<bool> m_isConnecting{ false };
                std::atomic<bool> m_isBusy{ false };

                uint32_t m_connectTimeoutMs = 5000;         // 连接超时
                uint32_t m_requestTimeoutMs = 30000;        // 请求超时
                bool m_keepAlive = true;                    // Keep-Alive 开关
                size_t m_maxRequestsPerConnection = 100;    // 每连接最大请求数
                size_t m_requestCount = 0;                  // 当前连接已处理的请求数
                std::chrono::steady_clock::time_point m_lastActivityTime;  // 新增：最后活动时间
            };

        } // namespace Http
    } // namespace Net
} // namespace ZhKeyesIM

#endif // NET_HTTP_HTTPCLIENTSESSION_H_