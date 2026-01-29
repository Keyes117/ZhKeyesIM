#include "HttpSession.h"

#include <functional>

#include "HttpClient.h"
#include "HttpServer.h"
#include "Logger.h"

using namespace ZhKeyesIM::Net::Http;

HttpSession::HttpSession(HttpServer* pServer, std::shared_ptr<TCPConnection>& spConn) :
    m_pHttpServer(pServer),
    m_HttpParser(SessionMode::SESSION_MODE_SERVER),
    m_spConnection(spConn)
{
    m_sessionID = HttpSession::generateID();

    m_spConnection->setReadCallback(std::bind(&HttpSession::onRead, this, std::placeholders::_1));
    m_spConnection->setWriteCallback(std::bind(&HttpSession::onWrite, this));

}

ZhKeyesIM::Net::Http::HttpSession::~HttpSession()
{
}

void HttpSession::onRead(Buffer& buffer)
{

    //如果上一条请求还没有完成相应， 直接忽略（正常情况下也不应该，外部会pauseRead）
    if (m_waitingResponse)
        return;

    ParseResult result = m_HttpParser.feed(buffer);

    if (result == ParseResult::PARSE_RESULT_COMPLETE)
    {
        if (!m_pHttpServer)
        {
            handleParseError();
            return;
        }

        // 服务端模式
        auto request = m_HttpParser.getRequest();
        if (request)
        {
            handleRequest(request);
            return;
        }
    }

    if (result == ParseResult::PARSE_RESULT_ERROR)
    {
        handleParseError();
        return;
    }
}

void HttpSession::onWrite()
{
  
}

bool HttpSession::sendResponse(const HttpResponse& response)
{
    return m_spConnection->send(response.toString());
}

void HttpSession::handleRequest(std::shared_ptr<HttpRequest> spRequest)
{
    if (! m_pHttpServer  || !spRequest)
    {
        m_spConnection->onClose();
        return;
    }


    m_keepAlive = spRequest->isKeepAlive();
    m_waitingResponse = true;

    m_spConnection->pauseRead();

    std::weak_ptr<HttpSession> weakSelf = shared_from_this();

    m_pHttpServer->handleRequestAsync(*spRequest, [weakSelf](HttpResponse&& response) mutable {
        auto self = weakSelf.lock();

        if (!self)
            return;

        if (!self->m_keepAlive)
            response.setHeader("Connection", "close");
        else
            response.setHeader("Connection", "keep-alive");

        self->sendResponse(response);

        if (!self->m_keepAlive)
        {
            self->m_spConnection->shutdownAfterWrite();
        }
        else
        {
            self->m_waitingResponse = false;
            self->m_HttpParser.reset();
            self->m_spConnection->resumeRead();
        }
    });
}

void ZhKeyesIM::Net::Http::HttpSession::handleParseError()
{
}

bool ZhKeyesIM::Net::Http::HttpSession::shouldKeepAlive() const
{
    return false;
}

uint32_t HttpSession::generateID()
{
    static std::atomic<uint32_t> s_sessionID = 0;
    uint32_t result = ++s_sessionID;
    return result;
}