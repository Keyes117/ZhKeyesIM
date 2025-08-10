#include "HttpSession.h"

#include <functional>

#include "HttpClient.h"
#include "HttpServer.h"
#include "Logger.h"

HttpSession::HttpSession(HttpServer* pServer, std::shared_ptr<TCPConnection>&& spConn) :
    m_pHttpServer(pServer),
    m_HttpParser(SessionMode::SERVER),
    m_spConnection(std::move(spConn))
{
    m_sessionID = HttpSession::generateID();


    m_spConnection->setReadCallback(std::bind(&HttpSession::onRead, this, std::placeholders::_1));
    m_spConnection->setWriteCallback(std::bind(&HttpSession::onWrite, this));
    m_spConnection->setCloseCallback(std::bind(&HttpSession::onClose, this));
}

HttpSession::HttpSession(HttpClient* pClient, std::shared_ptr<TCPConnection>&& spConn) :
    m_pHttpClient(pClient),
    m_HttpParser(SessionMode::CLIENT),
    m_spConnection(std::move(spConn))
{
    m_sessionID = HttpSession::generateID();

    m_spConnection->setReadCallback(std::bind(&HttpSession::onRead, this, std::placeholders::_1));
    m_spConnection->setWriteCallback(std::bind(&HttpSession::onWrite, this));
    m_spConnection->setCloseCallback(std::bind(&HttpSession::onClose, this));
}

void HttpSession::onRead(Buffer& buffer)
{
    ParseResult result = m_HttpParser.feed(buffer);

    if (result == ParseResult::PARSE_COMPLETE)
    {
        if (m_pHttpServer)
        {  // 服务端模式
            auto request = m_HttpParser.getRequest();
            if (request)
            {
                handleRequest(request);
            }
        }
        else if (m_pHttpServer)
        {  // 客户端模式
            auto response = m_HttpParser.getResponse();
            if (response)
            {
                handleResponse(response);
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
    }
    else if (result == ParseResult::PARSE_ERROR)
    {
        handleParseError();
    }
}
void HttpSession::onWrite()
{

}
void HttpSession::onClose()
{
    if (m_pHttpServer)
    {
        m_pHttpServer->onDisConnected(m_sessionID);
    }
    else if (m_pHttpClient)
    {
        m_pHttpClient->onDisconnected();
    }
}

bool HttpSession::sendRequest(const HttpRequest& request)
{
    return false;
}

bool HttpSession::sendResponse(const HttpResponse& response)
{
    return false;
}

void HttpSession::handleResponse(std::shared_ptr<HttpResponse>& spResponse)
{
    if (m_pHttpClient)
    {
        m_pHttpClient->handleResponse(*spResponse);
    }
    else
    {
        onClose();
    }
}

uint32_t HttpSession::generateID()
{
    static std::atomic<uint32_t> s_sessionID = 0;
    uint32_t result = ++s_sessionID;
    return result;
}