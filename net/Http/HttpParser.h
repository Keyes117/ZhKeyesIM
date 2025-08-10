/**
 * @desc:   Http解析器，HttpParser.h
 * @author: ZhKeyes
 * @date:   2025/8/5
 */

#ifndef NET_HTTP_HTTPPARSER_H_
#define NET_HTTP_HTTPPARSER_H_

#include "Buffer.h"
#include "HttpProtocol.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "net_export.h"
#include <memory>
#include <string>

class NET_API HttpParser
{
public:
    HttpParser();
    explicit HttpParser(SessionMode mode);
    ~HttpParser() = default;

    // ============== 配置方法 ==============
    void setSessionMode(SessionMode mode) { m_sessionMode = mode; }
    SessionMode getSessionMode() const { return m_sessionMode; }

    void setMaxHeaderSize(size_t maxSize) { m_maxHeaderSize = maxSize; }
    void setMaxBodySize(size_t maxSize) { m_maxBodySize = maxSize; }
    void setMaxUrlLength(size_t maxLength) { m_maxUrlLength = maxLength; }

    // ============== 解析方法 ==============
    ParseResult feed(Buffer& buffer);  // 唯一的解析入口

    // ============== 状态查询 ==============
    ParseState getState() const { return m_parseState; }
    bool isParseComplete() const { return m_parseState == ParseState::PARSE_COMPLETE; }
    bool hasError() const { return m_parseState == ParseState::PARSE_ERROR; }

    // ============== 结果获取 ==============
    std::shared_ptr<HttpRequest> getRequest() const { return m_request; }
    std::shared_ptr<HttpResponse> getResponse() const { return m_response; }
    HttpError getLastError() const { return m_lastError; }
    const std::string& getErrorMessage() const { return m_errorMessage; }

    // ============== 重置和清理 ==============
    void reset();
    void clear();

    // ============== 解析进度 ==============
    size_t getParsedBytes() const { return m_parsedBytes; }
    size_t getCurrentHeaderSize() const { return m_currentHeaderSize; }
    size_t getCurrentBodySize() const { return m_currentBodySize; }

    // ============== 工具方法 ==============
    static bool isValidHttpMethod(const std::string& method);
    static bool isValidHttpVersion(const std::string& version);
    static bool isValidStatusCode(int statusCode);

private:
    // ============== 状态机解析方法 ==============
    ParseResult parseRequestLine(Buffer& buffer);
    ParseResult parseStatusLine(Buffer& buffer);
    ParseResult parseHeaders(Buffer& buffer);
    ParseResult parseBody(Buffer& buffer);
    ParseResult parseChunkedBody(Buffer& buffer);

    // ============== 辅助解析方法 ==============
    bool findLine(Buffer& buffer, std::string& line);
    bool findHeaderEnd(Buffer& buffer);
    size_t getContentLength() const;
    bool isChunkedEncoding() const;
    bool needMoreData(Buffer& buffer) const;

    // ============== 错误处理 ==============
    void setError(HttpError error, const std::string& message);
    bool checkLimits();

private:
    // ============== 解析状态 ==============
    SessionMode m_sessionMode = SessionMode::UNEXPECTED_MODE;
    ParseState m_parseState = ParseState::PARSE_REQUEST_LINE;
    HttpError m_lastError = HttpError::NONE;
    std::string m_errorMessage;

    // ============== 解析结果 ==============
    std::shared_ptr<HttpRequest> m_request;
    std::shared_ptr<HttpResponse> m_response;

    // ============== 解析进度 ==============
    size_t m_parsedBytes = 0;       // 已解析字节数
    size_t m_currentHeaderSize = 0; // 当前头部大小
    size_t m_currentBodySize = 0;   // 当前消息体大小
    size_t m_expectedBodySize = 0;  // 期望的消息体大小

    // ============== 限制配置 ==============
    size_t m_maxHeaderSize = HttpConstants::MAX_HEADER_SIZE;
    size_t m_maxBodySize = HttpConstants::MAX_BODY_SIZE;
    size_t m_maxUrlLength = HttpConstants::MAX_URL_LENGTH;

    // ============== 分块传输状态 ==============
    bool m_isChunked = false;       // 是否分块传输
    size_t m_chunkSize = 0;         // 当前块大小
    bool m_chunkSizeRead = false;   // 是否已读取块大小
    bool m_finalChunk = false;      // 是否最后一块
};
#endif