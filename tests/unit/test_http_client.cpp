#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "HttpClient.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "EventLoop.h"

using namespace ZhKeyesIM::Net::Http;
using namespace testing;

class HttpClientTest : public Test {
protected:
    void SetUp() override {
        m_eventLoop = std::make_shared<EventLoop>();
        m_client = std::make_unique<HttpClient>(m_eventLoop);
    }

    void TearDown() override {
        if (m_client && m_client->isConnected()) {
            m_client->disconnect();
        }
    }

    std::shared_ptr<EventLoop> m_eventLoop;
    std::unique_ptr<HttpClient> m_client;
};

TEST_F(HttpClientTest, InitialStateTest) {
    EXPECT_FALSE(m_client->isConnected());
}

TEST_F(HttpClientTest, RequestCreationTest) {
    HttpRequest request;
    request.setMethod(HttpMethod::GET);
    request.setPath("/test");
    request.setHeader("User-Agent", "TestClient");

    EXPECT_EQ(request.getMethod(), HttpMethod::GET);
    EXPECT_EQ(request.getPath(), "/test");
    EXPECT_EQ(request.getHeader("User-Agent"), "TestClient");
}

TEST_F(HttpClientTest, HeaderOperationsTest) {
    HttpRequest request;

    // 测试设置和获取头部
    request.setHeader("Content-Type", "application/json");
    EXPECT_EQ(request.getHeader("Content-Type"), "application/json");

    // 测试头部存在性
    EXPECT_TRUE(request.hasHeader("Content-Type"));
    EXPECT_FALSE(request.hasHeader("NonExistent"));

    // 测试删除头部
    request.removeHeader("Content-Type");
    EXPECT_FALSE(request.hasHeader("Content-Type"));
}

TEST_F(HttpClientTest, BodyOperationsTest) {
    HttpRequest request;

    // 测试设置和获取消息体
    request.setBody("Hello World");
    EXPECT_EQ(request.getBody(), "Hello World");
    EXPECT_EQ(request.getBodySize(), 11);

    // 测试追加消息体
    request.appendBody("!");
    EXPECT_EQ(request.getBody(), "Hello World!");

    // 测试清空消息体
    request.clearBody();
    EXPECT_TRUE(request.getBody().empty());
    EXPECT_EQ(request.getBodySize(), 0);
}

TEST_F(HttpClientTest, QueryParametersTest) {
    HttpRequest request;

    // 测试查询参数
    request.setQueryParam("page", "1");
    request.setQueryParam("size", "10");
    request.setQueryParam("sort", "name");

    EXPECT_EQ(request.getQueryParam("page"), "1");
    EXPECT_EQ(request.getQueryParam("size"), "10");
    EXPECT_EQ(request.getQueryParam("sort"), "name");

    EXPECT_TRUE(request.hasQueryParam("page"));
    EXPECT_FALSE(request.hasQueryParam("nonexistent"));

    request.removeQueryParam("page");
    EXPECT_FALSE(request.hasQueryParam("page"));

    request.clearQueryParams();
    EXPECT_EQ(request.getQueryParams().size(), 0);
}

TEST_F(HttpClientTest, FormDataTest) {
    HttpRequest request;

    // 测试表单数据
    std::unordered_map<std::string, std::string> formData;
    formData["username"] = "testuser";
    formData["password"] = "testpass";

    request.setFormData(formData);
    request.addFormData("email", "test@example.com");

    EXPECT_EQ(request.getFormDataString(), "username=testuser&password=testpass&email=test%40example.com");
}

TEST_F(HttpClientTest, CookieOperationsTest) {
    HttpRequest request;

    // 测试Cookie操作
    request.setCookie("sessionId", "abc123");
    request.setCookie("userId", "456");

    EXPECT_EQ(request.getCookie("sessionId"), "abc123");
    EXPECT_EQ(request.getCookie("userId"), "456");

    EXPECT_TRUE(request.hasCookie("sessionId"));
    EXPECT_FALSE(request.hasCookie("nonexistent"));

    std::string cookieHeader = request.getCookieHeader();
    EXPECT_TRUE(cookieHeader.find("sessionId=abc123") != std::string::npos);
    EXPECT_TRUE(cookieHeader.find("userId=456") != std::string::npos);
}

TEST_F(HttpClientTest, UrlOperationsTest) {
    HttpRequest request;

    // 测试URL操作
    request.setUrl("http://example.com/api/users?page=1&size=10");

    EXPECT_EQ(request.getUrl(), "http://example.com/api/users?page=1&size=10");
    EXPECT_EQ(request.getPath(), "/api/users");
    EXPECT_EQ(request.getHost(), "example.com");

    // 测试查询字符串解析
    EXPECT_EQ(request.getQueryParam("page"), "1");
    EXPECT_EQ(request.getQueryParam("size"), "10");
}

TEST_F(HttpClientTest, ContentTypeHelpersTest) {
    HttpRequest request;

    // 测试内容类型便捷方法
    request.setContentType("application/json");
    EXPECT_EQ(request.getContentType(), "application/json");

    request.setContentLength(1024);
    EXPECT_EQ(request.getContentLength(), 1024);

    request.setConnection("keep-alive");
    EXPECT_EQ(request.getConnection(), "keep-alive");
    EXPECT_TRUE(request.isKeepAlive());
}

TEST_F(HttpClientTest, JsonBodyTest) {
    HttpRequest request;

    // 测试JSON消息体
    std::string jsonData = "{\"name\": \"test\", \"value\": 123}";
    request.setJsonBody(jsonData);

    EXPECT_EQ(request.getBody(), jsonData);
    EXPECT_TRUE(request.isJsonRequest());
    EXPECT_FALSE(request.isFormRequest());
    EXPECT_FALSE(request.isMultipartRequest());
}