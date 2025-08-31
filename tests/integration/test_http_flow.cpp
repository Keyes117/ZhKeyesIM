#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "HttpServer.h"
#include "HttpClient.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpParser.h"
#include "Buffer.h"

using namespace ZhKeyesIM::Net::Http;
using namespace testing;

class HttpFlowIntegrationTest : public Test {
protected:
    void SetUp() override {
        m_server = std::make_unique<HttpServer>();
        m_client = std::make_unique<HttpClient>();
        m_parser = std::make_unique<HttpParser>();

        ASSERT_TRUE(m_server->init(2, "127.0.0.1"));

        m_server->setRequestCallBack([this](const HttpRequest& request, HttpResponse& response) {
            this->handleServerRequest(request, response);
            });
    }

    void TearDown() override {
        if (m_server) {
            m_server->shutdown();
        }
    }

    void handleServerRequest(const HttpRequest& request, HttpResponse& response) {
        if (request.getPath() == "/test") {
            response.setStatusCode(HttpStatusCode::OK);
            response.setBody("Test Response");
            response.setHeader("Content-Type", "text/plain");
        }
        else if (request.getPath() == "/echo") {
            response.setStatusCode(HttpStatusCode::OK);
            response.setBody(request.getBody());
            response.setHeader("Content-Type", "text/plain");
        }
        else if (request.getPath() == "/api/users") {
            response.setStatusCode(HttpStatusCode::OK);
            response.setBody("[{\"id\": 1, \"name\": \"John\"}]");
            response.setHeader("Content-Type", "application/json");
        }
        else if (request.getPath() == "/redirect") {
            response.setRedirectResponse("/new-location", false);
        }
        else {
            response.setErrorResponse(HttpStatusCode::NotFound, "Not Found");
        }
    }

    std::unique_ptr<HttpServer> m_server;
    std::unique_ptr<HttpClient> m_client;
    std::unique_ptr<HttpParser> m_parser;
};

TEST_F(HttpFlowIntegrationTest, BasicGetRequestFlow) {
    HttpRequest request;
    request.setMethod(HttpMethod::GET);
    request.setPath("/test");
    request.setHeader("User-Agent", "TestClient");

    HttpResponse response;
    m_server->handleRequest(request, response);

    EXPECT_EQ(response.getStatusCode(), HttpStatusCode::OK);
    EXPECT_EQ(response.getBody(), "Test Response");
    EXPECT_EQ(response.getHeader("Content-Type"), "text/plain");
}

TEST_F(HttpFlowIntegrationTest, PostRequestFlow) {
    HttpRequest request;
    request.setMethod(HttpMethod::POST);
    request.setPath("/echo");
    request.setBody("Hello Server");
    request.setHeader("Content-Type", "text/plain");

    HttpResponse response;
    m_server->handleRequest(request, response);

    EXPECT_EQ(response.getStatusCode(), HttpStatusCode::OK);
    EXPECT_EQ(response.getBody(), "Hello Server");
}

TEST_F(HttpFlowIntegrationTest, JsonApiFlow) {
    HttpRequest request;
    request.setMethod(HttpMethod::GET);
    request.setPath("/api/users");
    request.setHeader("Accept", "application/json");

    HttpResponse response;
    m_server->handleRequest(request, response);

    EXPECT_EQ(response.getStatusCode(), HttpStatusCode::OK);
    EXPECT_EQ(response.getHeader("Content-Type"), "application/json");
    EXPECT_TRUE(response.getBody().find("John") != std::string::npos);
}

TEST_F(HttpFlowIntegrationTest, RedirectFlow) {
    HttpRequest request;
    request.setMethod(HttpMethod::GET);
    request.setPath("/redirect");

    HttpResponse response;
    m_server->handleRequest(request, response);

    EXPECT_EQ(response.getStatusCode(), HttpStatusCode::Found);
    EXPECT_EQ(response.getHeader("Location"), "/new-location");
}

TEST_F(HttpFlowIntegrationTest, NotFoundFlow) {
    HttpRequest request;
    request.setMethod(HttpMethod::GET);
    request.setPath("/nonexistent");

    HttpResponse response;
    m_server->handleRequest(request, response);

    EXPECT_EQ(response.getStatusCode(), HttpStatusCode::NotFound);
}

TEST_F(HttpFlowIntegrationTest, ParserIntegrationTest) {
    // 测试解析器与HTTP消息的集成
    std::string rawRequest =
        "POST /api/data HTTP/1.1\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 25\r\n"
        "\r\n"
        "{\"key\": \"value\"}";

    Buffer buffer;
    buffer.append(rawRequest);

    m_parser->setSessionMode(SessionMode::SESSION_MODE_SERVER);
    ParseResult result = m_parser->feed(buffer);

    EXPECT_EQ(result, ParseResult::PARSE_RESULT_COMPLETE);

    auto request = m_parser->getRequest();
    EXPECT_NE(request, nullptr);
    EXPECT_EQ(request->getMethod(), HttpMethod::POST);
    EXPECT_EQ(request->getPath(), "/api/data");
    EXPECT_EQ(request->getHeader("Content-Type"), "application/json");
    EXPECT_EQ(request->getBody(), "{\"key\": \"value\"}");

    // 将解析的请求传递给服务器处理
    HttpResponse response;
    m_server->handleRequest(*request, response);

    // 验证响应
    EXPECT_EQ(response.getStatusCode(), HttpStatusCode::NotFound); // 因为/api/data路径没有定义
}

TEST_F(HttpFlowIntegrationTest, ComplexRequestFlow) {
    // 测试复杂的请求流程
    HttpRequest request;
    request.setMethod(HttpMethod::POST);
    request.setPath("/echo");
    request.setUrl("http://example.com/echo?param1=value1&param2=value2");
    request.setHeader("Content-Type", "application/x-www-form-urlencoded");
    request.setHeader("User-Agent", "TestClient/1.0");
    request.setCookie("sessionId", "abc123");

    // 设置表单数据
    request.addFormData("username", "testuser");
    request.addFormData("email", "test@example.com");

    HttpResponse response;
    m_server->handleRequest(request, response);

    EXPECT_EQ(response.getStatusCode(), HttpStatusCode::OK);
    EXPECT_EQ(response.getBody(), "username=testuser&email=test@example.com");

    // 验证请求的各个部分
    EXPECT_EQ(request.getHost(), "example.com");
    EXPECT_EQ(request.getQueryParam("param1"), "value1");
    EXPECT_EQ(request.getQueryParam("param2"), "value2");
    EXPECT_EQ(request.getCookie("sessionId"), "abc123");
    EXPECT_TRUE(request.isFormRequest());
}