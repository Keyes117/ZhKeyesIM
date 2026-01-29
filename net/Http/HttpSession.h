/**
 * @desc:   HttpSession类，HttpSession.h
 * @author: ZhKeyes
 * @date:   2025/8/6
*/

#ifndef NET_HTTP_HTTPSESSION_H_
#define NET_HTTP_HTTPSESSION_H_

#include <atomic>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

#include "Buffer.h"
#include "HttpParser.h"
#include "HttpProtocol.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "net_export.h"
#include "TCPConnection.h"

namespace ZhKeyesIM {
    namespace Net {
        namespace Http {

            class HttpServer;
            class NET_API HttpSession : public std::enable_shared_from_this<HttpSession>
            {
            public:
                using SessionID = uint32_t;

                HttpSession(HttpServer* pServer, std::shared_ptr<TCPConnection>& spConn);
                ~HttpSession();

                void onRead(Buffer& byteBuffer);
                void onWrite();

                bool sendResponse(const HttpResponse& response);

                HttpSession::SessionID getID() { return m_sessionID; }

            private:
                void handleRequest(std::shared_ptr<HttpRequest> spRequest);
                void handleParseError();
                bool shouldKeepAlive() const;
                static uint32_t generateID();

            private:

                HttpServer* m_pHttpServer = nullptr;
                HttpParser                      m_HttpParser;
                std::shared_ptr<TCPConnection>  m_spConnection;
                SessionID                       m_sessionID;

                bool                            m_waitingResponse = false;
                bool                            m_keepAlive = false;
            };

        }
    }
}
#endif //!NET_HTTP_HTTPSESSION_H_