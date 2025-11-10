#include "HttpServer.h"

#include "Logger.h"


using namespace ZhKeyesIM::Net::Http;

ZhKeyesIM::Net::Http::HttpServer::~HttpServer()
{
    shutdown();
}

bool HttpServer::init(uint32_t threadNum, const std::string& ip/*=""*/, uint16_t port /*= 80*/)
{
    m_spTcpServer = std::make_unique<TCPServer>();
    if (!m_spTcpServer->init(threadNum, ip, port))
        return false;

    m_spTcpServer->setConnectionCallback(std::bind(&HttpServer::onConnected, this, std::placeholders::_1));


    return true;
}

void HttpServer::start()
{
    m_spTcpServer->start();
}

void HttpServer::shutdown()
{
    m_sessions.clear();
    m_pendingToDeleteSessions.clear();
}


void HttpServer::onConnected(std::shared_ptr<TCPConnection>& spConn)
{
    auto spHttpSession = std::make_shared<HttpSession>(this,std::move(spConn));
    m_sessions.insert(std::make_pair(spHttpSession->getID(), spHttpSession));

}

void HttpServer::onDisConnected(HttpSession::SessionID sessionID)
{
    auto iter = m_sessions.find(sessionID);
    if (iter != m_sessions.end())
    {
        //这里不能直接删除
        std::shared_ptr<HttpSession> spHttpSession = iter->second;
        m_pendingToDeleteSessions.emplace_back(spHttpSession);
        m_sessions.erase(iter);


    }
}

void HttpServer::handleRequest(const HttpRequest& request, HttpResponse& response)
{
    if (m_requestCallBack)
    {
        m_requestCallBack(request, response);
    }
    else {
        response.setErrorResponse(HttpStatusCode::NotFound, "Not Found");
    }
}

void HttpServer::cleanupSessions()
{
    m_pendingToDeleteSessions.clear();
}

