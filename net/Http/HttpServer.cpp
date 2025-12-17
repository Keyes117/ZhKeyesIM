#include "HttpServer.h"

#include "Logger.h"


using namespace ZhKeyesIM::Net::Http;

ZhKeyesIM::Net::Http::HttpServer::~HttpServer()
{
    shutdown();
}

bool HttpServer::init(uint32_t threadNum, const std::string& ip/*=""*/, uint16_t port /*= 80*/,
    IOMultiplexType type)
{
    m_spTcpServer = std::make_unique<TCPServer>();
    if (!m_spTcpServer->init(threadNum, ip, port, type))
        return false;

    m_spTcpServer->setConnectionCallback(std::bind(&HttpServer::onConnected, this, std::placeholders::_1));
    m_spTcpServer->setDisConnectionCallback(std::bind(&HttpServer::onDisConnected, this, std::placeholders::_1));

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
    auto spHttpSession = std::make_shared<HttpSession>(this, spConn);
    HttpSession::SessionID sessionId = spHttpSession->getID();
    {
        std::lock_guard<std::mutex> lock(m_sessionMutex);
        m_sessions.insert(std::make_pair(sessionId, spHttpSession));
        m_socketToSession.insert(std::make_pair(spConn->getSocket(), sessionId));
    }
}

void HttpServer::onDisConnected(SOCKET clientSocket)
{

    std::lock_guard<std::mutex> lock(m_sessionMutex);
    auto socketIter = m_socketToSession.find(clientSocket);

    if (socketIter != m_socketToSession.end())
    {
        HttpSession::SessionID sessionID = socketIter->second;

        auto iter = m_sessions.find(sessionID);
        if (iter != m_sessions.end())
        {
            //这里不能直接删除
            std::shared_ptr<HttpSession> spHttpSession = iter->second;
            m_pendingToDeleteSessions.emplace_back(spHttpSession);
            m_sessions.erase(iter);
        }

        m_socketToSession.erase(socketIter);
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

void ZhKeyesIM::Net::Http::HttpServer::handleRequestAsync(const HttpRequest& request, AsyncDone done)
{
    if (!done)
        return;

    if (m_asyncRequestCallBack)
    {
        m_asyncRequestCallBack(request, std::move(done));
        return;
    }

    HttpResponse response;
    response.setErrorResponse(HttpStatusCode::NotFound, "NotFound");
    done(std::move(response));
}

void HttpServer::cleanupSessions()
{
    m_pendingToDeleteSessions.clear();
}

