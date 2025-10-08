#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "HttpParser.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Buffer.h"

using namespace ZhKeyesIM::Net::Http;
using namespace testing;

class HttpParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        parser = std::make_unique<HttpParser>();
        parser->setSessionMode(SessionMode::SESSION_MODE_SERVER);
    }

    std::unique_ptr<HttpParser> parser;
};

TEST_F(HttpParserTest, InitialStateTest) {
    EXPECT_EQ(parser->getSessionMode(), SessionMode::SESSION_MODE_SERVER);
    EXPECT_EQ(parser->getState(), ParseState::PARSE_REQUEST_LINE);
    EXPECT_FALSE(parser->isParseComplete());
    EXPECT_FALSE(parser->hasError());
    EXPECT_EQ(parser->getParsedBytes(), 0);
}

TEST_F(HttpParserTest, ParseValidHttpRequest) {
    std::string rawRequest =
        "GET /test HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "User-Agent: TestAgent\r\n"
        "Content-Length: 11\r\n"
        "\r\n"
        "Hello World";

    Buffer buffer;
    buffer.append(rawRequest);

    ParseResult result = parser->feed(buffer);

    EXPECT_EQ(result, ParseResult::PARSE_RESULT_COMPLETE);
    EXPECT_TRUE(parser->isParseComplete());
    EXPECT_FALSE(parser->hasError());

    auto request = parser->getRequest();
    EXPECT_NE(request, nullptr);
    EXPECT_EQ(request->getMethod(), HttpMethod::GET);
    EXPECT_EQ(request->getPath(), "/test");
    EXPECT_EQ(request->getVersion(), HttpVersion::HTTP_1_1);
    EXPECT_EQ(request->getHeader("Host"), "example.com");
    EXPECT_EQ(request->getHeader("User-Agent"), "TestAgent");
    EXPECT_EQ(request->getHeader("Content-Length"), "11");
    EXPECT_EQ(request->getBody(), "Hello World");
}

TEST_F(HttpParserTest, ParseInvalidHttpRequest) {
    std::string invalidRequest = "INVALID HTTP REQUEST";

    Buffer buffer;
    buffer.append(invalidRequest);

    ParseResult result = parser->feed(buffer);

    EXPECT_EQ(result, ParseResult::PARSE_RESULT_ERROR);
    EXPECT_TRUE(parser->hasError());
    EXPECT_FALSE(parser->isParseComplete());
}

TEST_F(HttpParserTest, ParseHttpResponse) {
    // 切换到客户端模式
    parser->setSessionMode(SessionMode::SESSION_MODE_CLIENT);

    std::string rawResponse =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 12\r\n"
        "\r\n"
        "Hello World!";

    Buffer buffer;
    buffer.append(rawResponse);

    ParseResult result = parser->feed(buffer);

    EXPECT_EQ(result, ParseResult::PARSE_RESULT_COMPLETE);
    EXPECT_TRUE(parser->isParseComplete());
    EXPECT_FALSE(parser->hasError());

    auto response = parser->getResponse();
    EXPECT_NE(response, nullptr);
    EXPECT_EQ(response->getStatusCode(), HttpStatusCode::OK);
    EXPECT_EQ(response->getReasonPhrase(), "OK");
    EXPECT_EQ(response->getVersion(), HttpVersion::HTTP_1_1);
    EXPECT_EQ(response->getHeader("Content-Type"), "text/html");
    EXPECT_EQ(response->getHeader("Content-Length"), "12");
    EXPECT_EQ(response->getBody(), "Hello World!");
}

TEST_F(HttpParserTest, ParseIncompleteRequest) {
    std::string incompleteRequest =
        "GET /test HTTP/1.1\r\n"
        "Host: example.com\r\n";

    Buffer buffer;
    buffer.append(incompleteRequest);

    ParseResult result = parser->feed(buffer);

    EXPECT_EQ(result, ParseResult::PARSE_RESULT_NEED_MORE_DATA);
    EXPECT_FALSE(parser->isParseComplete());
    EXPECT_FALSE(parser->hasError());
}

TEST_F(HttpParserTest, ParseChunkedRequest) {
    std::string chunkedRequest =
        "POST /upload HTTP/1.1\r\n"
        "Transfer-Encoding: chunked\r\n"
        "\r\n"
        "5\r\n"
        "Hello\r\n"
        "6\r\n"
        " World\r\n"
        "0\r\n"
        "\r\n";

    Buffer buffer;
    buffer.append(chunkedRequest);

    ParseResult result = parser->feed(buffer);

    EXPECT_EQ(result, ParseResult::PARSE_RESULT_COMPLETE);
    EXPECT_TRUE(parser->isParseComplete());

    auto request = parser->getRequest();
    EXPECT_NE(request, nullptr);
    EXPECT_EQ(request->getMethod(), HttpMethod::POST);
    EXPECT_EQ(request->getPath(), "/upload");
    EXPECT_EQ(request->getBody(), "Hello World");
}

TEST_F(HttpParserTest, ParserLimitsTest) {
    // 测试头部大小限制
    parser->setMaxHeaderSize(100);

    std::string largeHeaderRequest =
        "GET /test HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "Very-Long-Header: " + std::string(200, 'A') + "\r\n"
        "\r\n";

    Buffer buffer;
    buffer.append(largeHeaderRequest);

    ParseResult result = parser->feed(buffer);

    EXPECT_EQ(result, ParseResult::PARSE_RESULT_ERROR);
    EXPECT_TRUE(parser->hasError());
}

TEST_F(HttpParserTest, ParserResetTest) {
    // 先解析一个请求
    std::string request = "GET /test HTTP/1.1\r\n\r\n";
    Buffer buffer;
    buffer.append(request);

    parser->feed(buffer);
    EXPECT_TRUE(parser->isParseComplete());

    // 重置解析器
    parser->reset();
    EXPECT_EQ(parser->getState(), ParseState::PARSE_REQUEST_LINE);
    EXPECT_FALSE(parser->isParseComplete());
    EXPECT_FALSE(parser->hasError());
    EXPECT_EQ(parser->getParsedBytes(), 0);
}

TEST_F(HttpParserTest, StaticValidationTest) {
    // 测试静态验证方法
    EXPECT_TRUE(HttpParser::isValidHttpMethod("GET"));
    EXPECT_TRUE(HttpParser::isValidHttpMethod("POST"));
    EXPECT_TRUE(HttpParser::isValidHttpMethod("PUT"));
    EXPECT_TRUE(HttpParser::isValidHttpMethod("DELETE"));
    EXPECT_FALSE(HttpParser::isValidHttpMethod("INVALID"));

    EXPECT_TRUE(HttpParser::isValidHttpVersion("HTTP/1.0"));
    EXPECT_TRUE(HttpParser::isValidHttpVersion("HTTP/1.1"));
    EXPECT_FALSE(HttpParser::isValidHttpVersion("HTTP/2.0"));

    EXPECT_TRUE(HttpParser::isValidStatusCode(200));
    EXPECT_TRUE(HttpParser::isValidStatusCode(404));
    EXPECT_TRUE(HttpParser::isValidStatusCode(500));
    EXPECT_FALSE(HttpParser::isValidStatusCode(999));
}

TEST_F(HttpParserTest, MultipleRequestsTest) {
    std::string multipleRequests =
        "GET /first HTTP/1.1\r\n\r\n"
        "POST /second HTTP/1.1\r\nContent-Length: 0\r\n\r\n";

    Buffer buffer;
    buffer.append(multipleRequests);

    // 解析第一个请求
    ParseResult result1 = parser->feed(buffer);
    EXPECT_EQ(result1, ParseResult::PARSE_RESULT_COMPLETE);

    auto request1 = parser->getRequest();
    EXPECT_EQ(request1->getMethod(), HttpMethod::GET);
    EXPECT_EQ(request1->getPath(), "/first");

    // 重置解析器
    parser->reset();

    // 解析第二个请求
    ParseResult result2 = parser->feed(buffer);
    EXPECT_EQ(result2, ParseResult::PARSE_RESULT_COMPLETE);

    auto request2 = parser->getRequest();
    EXPECT_EQ(request2->getMethod(), HttpMethod::POST);
    EXPECT_EQ(request2->getPath(), "/second");
}