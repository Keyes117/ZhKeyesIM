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
    ParseResult result = m_HttpParser.feed(buffer);

    if (result == ParseResult::PARSE_RESULT_COMPLETE)
    {
        if (m_pHttpServer)
        {  // 服务端模式
            auto request = m_HttpParser.getRequest();
            if (request)
            {
                handleRequest(request);
            }
        }
        else
        {
            handleParseError();
        }

        // 重置解析器，准备处理下一个消息
        if (shouldKeepAlive()) {
            m_HttpParser.reset();

        }
        else
        {
            m_spConnection->shutdownAfterWrite();
        }
    }
    else if (result == ParseResult::PARSE_RESULT_ERROR)
    {
        handleParseError();
    }
}

void HttpSession::onWrite()
{

}
void HttpSession::onClose()
{

}

bool HttpSession::sendResponse(const HttpResponse& response)
{
    return m_spConnection->send(response.toString());
}

void HttpSession::handleRequest(std::shared_ptr<HttpRequest>& spRequest)
{
    if (m_pHttpServer)
    {
        HttpResponse response;
        m_pHttpServer->handleRequest(*spRequest, response);

        sendResponse(response);
    }
    else
    {
        m_spConnection->onClose();
    }
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