#include "HttpServer.h"

#include "Logger.h"

bool HttpServer::init(uint32_t threadNum, const std::string& ip/*=""*/)
{
    m_spTcpServer = std::make_unique<TCPServer>();
    if (!m_spTcpServer->init(threadNum, ip, 80))
        return false;

    m_spTcpServer->setConnectionCallback(std::bind(&HttpServer::onConnected, this, std::placeholders::_1));


    return true;
}

void HttpServer::shutdown()
{
    if (m_spTcpServer)
        m_spTcpServer->shutdown();

    m_sessions.clear();
    m_pendingToDeleteSessions.clear();
}


void HttpServer::onConnected(std::shared_ptr<TCPConnection>& spConn)
{
    auto spHttpSession = std::make_shared<HttpSession>(spConn);
    m_sessions.insert(std::make_pair(spHttpSession->getID(), spHttpSession));

    LOG_INFO("New Http Session from %s", spConn->getRemoteAddress().c_str());
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

        LOG_INFO("HTTP session disconnected, ID: " + std::to_string(sessionID));
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

