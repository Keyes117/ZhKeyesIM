#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "HttpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

#ifdef _WIN32
#include <stdexcept>
#include <Windows.h>
#include <iostream>
#endif

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


using namespace ZhKeyesIM::Net::Http;
using namespace testing;

class HttpServerTest : public Test {
protected:
    void SetUp() override 
    {  // 修正：Setup -> SetUp

#ifdef _DEBUG
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
        m_spServer = std::make_unique<HttpServer>();
    }

    void TearDown() override
    {   
         if (m_spServer)
            m_spServer->shutdown();   
#ifdef _DEBUG
            _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif  
    }

    std::unique_ptr<HttpServer> m_spServer;
    std::string m_strIp = "127.0.0.1";
};

TEST_F(HttpServerTest, InitTest) {

    EXPECT_TRUE(m_spServer->init(0, m_strIp));
    EXPECT_EQ(m_spServer->getActiveSessionCount(), 0);

    std::this_thread::sleep_for(std::chrono::seconds(10));

}

TEST_F(HttpServerTest, RequestCallbackTest) {
    bool callbackCalled = false;
    HttpRequest receivedRequest;  

    m_spServer->setRequestCallBack([&](const HttpRequest& request, HttpResponse& response) {
        callbackCalled = true;
        receivedRequest = request;
        response.setStatusCode(HttpStatusCode::OK);  // 使用枚举值
        response.setBody("Hello, World!");
    });

    HttpRequest request;
    request.setMethod(HttpMethod::GET);  // 使用枚举值
    request.setPath("/test");

    HttpResponse response;
    m_spServer->handleRequest(request, response);  // 修正：server -> m_spServer

    EXPECT_TRUE(callbackCalled);
    EXPECT_EQ(response.getStatusCode(), HttpStatusCode::OK);  // 使用枚举值
    EXPECT_EQ(response.getBody(), "Hello, World!");
}

TEST_F(HttpServerTest, DefaultErrorResponseTest) {
    HttpRequest request;
    request.setMethod(HttpMethod::GET);  // 使用枚举值
    request.setPath("/nonexistent");

    HttpResponse response;
    m_spServer->handleRequest(request, response);  // 修正：server -> m_spServer

    EXPECT_EQ(response.getStatusCode(), HttpStatusCode::NotFound);  // 使用枚举值
}

TEST_F(HttpServerTest, SessionManagerTest) {
    EXPECT_TRUE(m_spServer->init(4, m_strIp));
    EXPECT_EQ(m_spServer->getActiveSessionCount(), 0);
    
    m_spServer->cleanupSessions();  // 修正：m_cleanupSessions -> cleanupSessions
    EXPECT_EQ(m_spServer->getActiveSessionCount(), 0);
}

TEST_F(HttpServerTest, MultipleRequestsTest) {
    int callbackCount = 0;

    m_spServer->setRequestCallBack([&](const HttpRequest& request, HttpResponse& response) {
        callbackCount++;
        if (request.getPath() == "/api/users") {
            response.setStatusCode(HttpStatusCode::OK);
            response.setBody("{\"users\": []}");
            response.setHeader("Content-Type", "application/json");
        }
        else if (request.getPath() == "/api/status") {
            response.setStatusCode(HttpStatusCode::OK);
            response.setBody("{\"status\": \"running\"}");
            response.setHeader("Content-Type", "application/json");
        }
        else {
            response.setStatusCode(HttpStatusCode::NotFound);
            response.setBody("Not Found");
        }
        });

    // 测试多个请求
    HttpRequest request1, request2, request3;
    HttpResponse response1, response2, response3;

    request1.setMethod(HttpMethod::GET);
    request1.setPath("/api/users");

    request2.setMethod(HttpMethod::GET);
    request2.setPath("/api/status");

    request3.setMethod(HttpMethod::GET);
    request3.setPath("/invalid");

    m_spServer->handleRequest(request1, response1);
    m_spServer->handleRequest(request2, response2);
    m_spServer->handleRequest(request3, response3);

    EXPECT_EQ(callbackCount, 3);
    EXPECT_EQ(response1.getStatusCode(), HttpStatusCode::OK);
    EXPECT_EQ(response2.getStatusCode(), HttpStatusCode::OK);
    EXPECT_EQ(response3.getStatusCode(), HttpStatusCode::NotFound);
}