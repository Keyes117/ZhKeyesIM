/**
 * @desc:   Http解析器，HttpParser.h
 * @author: ZhKeyes
 * @date:   2025/8/5
 */

#ifndef NET_HTTP_HTTPPARSER_H_
#define NET_HTTP_HTTPPARSER_H_

#include "HttpProtocol.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "net_export.h"
#include <string>
#include <memory>

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
    ParseResult feed(const char* data, size_t length);
    ParseResult feed(const std::string& data);
    
    // 解析完整消息（一次性解析）
    ParseResult parseRequest(const std::string& data, HttpRequest& request);
    ParseResult parseResponse(const std::string& data, HttpResponse& response);
    
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
    ParseResult parseRequestLine();
    ParseResult parseStatusLine();
    ParseResult parseHeaders();
    ParseResult parseBody();
    ParseResult parseChunkedBody();
    
    // ============== 辅助解析方法 ==============
    bool findLine(std::string& line);
    bool findHeaderEnd();
    size_t getContentLength() const;
    bool isChunkedEncoding() const;
    bool needMoreData() const;
    
    // ============== 错误处理 ==============
    void setError(HttpError error, const std::string& message);
    bool checkLimits();
    
    // ============== 数据处理 ==============
    void appendBuffer(const char* data, size_t length);
    void consumeBuffer(size_t length);
    std::string extractLine();
    
private:
    // ============== 解析状态 ==============
    SessionMode m_sessionMode = SessionMode::UNKNOWN;
    ParseState m_parseState = ParseState::PARSE_REQUEST_LINE;
    HttpError m_lastError = HttpError::NONE;
    std::string m_errorMessage;
    
    // ============== 解析结果 ==============
    std::shared_ptr<HttpRequest> m_request;
    std::shared_ptr<HttpResponse> m_response;
    
    // ============== 数据缓冲区 ==============
    std::string m_buffer;           // 输入数据缓冲区
    size_t m_bufferOffset = 0;      // 当前解析位置
    
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