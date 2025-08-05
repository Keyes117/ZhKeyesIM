/**
 * @desc:   Http解析器实现，HttpParser.cpp
 * @author: ZhKeyes
 * @date:   2025/8/5
 */
#include "HttpParser.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <set>
HttpParser::HttpParser() {
    reset();
}

HttpParser::HttpParser(SessionMode mode) : m_sessionMode(mode) {
    reset();
}

// ============== 解析方法 ==============

ParseResult HttpParser::feed(const char* data, size_t length) {
    if (!data || length == 0) {
        return ParseResult::NEED_MORE_DATA;
    }
    
    // 检查是否已经完成或出错
    if (m_parseState == ParseState::PARSE_COMPLETE || 
        m_parseState == ParseState::PARSE_ERROR) {
        return static_cast<ParseResult>(m_parseState);
    }
    
    // 添加数据到缓冲区
    appendBuffer(data, length);
    
    // 检查限制
    if (!checkLimits()) {
        return ParseResult::PARSE_ERROR;
    }
    
    // 状态机解析
    while (m_parseState != ParseState::PARSE_COMPLETE && 
           m_parseState != ParseState::PARSE_ERROR && 
           !needMoreData()) {
        
        ParseResult result = ParseResult::PARSE_ERROR;
        
        switch (m_parseState) {
            case ParseState::PARSE_REQUEST_LINE:
                result = parseRequestLine();
                break;
                
            case ParseState::PARSE_HEADERS:
                result = parseHeaders();
                break;
                
            case ParseState::PARSE_BODY:
                if (m_isChunked) {
                    result = parseChunkedBody();
                } else {
                    result = parseBody();
                }
                break;
                
            default:
                setError(HttpError::PARSE_ERROR, "Unknown parse state");
                return ParseResult::PARSE_ERROR;
        }
        
        if (result == ParseResult::PARSE_ERROR) {
            return ParseResult::PARSE_ERROR;
        } else if (result == ParseResult::NEED_MORE_DATA) {
            break;
        }
    }
    
    return static_cast<ParseResult>(m_parseState);
}

ParseResult HttpParser::feed(const std::string& data) {
    return feed(data.c_str(), data.length());
}

ParseResult HttpParser::parseRequest(const std::string& data, HttpRequest& request) {
    reset();
    m_sessionMode = SessionMode::CLIENT;
    m_parseState = ParseState::PARSE_REQUEST_LINE;
    
    ParseResult result = feed(data);
    if (result == ParseResult::PARSE_COMPLETE && m_request) {
        request = *m_request;
        return ParseResult::PARSE_COMPLETE;
    }
    
    return result;
}

ParseResult HttpParser::parseResponse(const std::string& data, HttpResponse& response) {
    reset();
    m_sessionMode = SessionMode::SERVER;
    m_parseState = ParseState::PARSE_REQUEST_LINE; // 实际会解析状态行
    
    ParseResult result = feed(data);
    if (result == ParseResult::PARSE_COMPLETE && m_response) {
        response = *m_response;
        return ParseResult::PARSE_COMPLETE;
    }
    
    return result;
}

// ============== 状态机解析方法 ==============

ParseResult HttpParser::parseRequestLine() {
    // 根据会话模式决定解析请求行还是状态行
    if (m_sessionMode == SessionMode::CLIENT || m_sessionMode == SessionMode::UNKNOWN) {
        return parseStatusLine(); // 客户端接收响应
    } else {
        // 服务端接收请求
        std::string line;
        if (!findLine(line)) {
            return ParseResult::NEED_MORE_DATA;
        }
        
        // 检查URL长度限制
        if (line.length() > m_maxUrlLength) {
            setError(HttpError::INVALID_REQUEST, "Request line too long");
            return ParseResult::PARSE_ERROR;
        }
        
        // 解析请求行: METHOD URL HTTP/VERSION
        std::istringstream iss(line);
        std::string method, url, version;
        
        if (!(iss >> method >> url >> version)) {
            setError(HttpError::INVALID_REQUEST, "Invalid request line format");
            return ParseResult::PARSE_ERROR;
        }
        
        // 验证HTTP方法
        if (!isValidHttpMethod(method)) {
            setError(HttpError::INVALID_REQUEST, "Invalid HTTP method: " + method);
            return ParseResult::PARSE_ERROR;
        }
        
        // 验证HTTP版本
        if (!isValidHttpVersion(version)) {
            setError(HttpError::UNSUPPORTED_VERSION, "Unsupported HTTP version: " + version);
            return ParseResult::PARSE_ERROR;
        }
        
        // 创建请求对象
        if (!m_request) {
            m_request = std::make_shared<HttpRequest>();
        }
        
        m_request->setMethod(HttpUtils::stringToMethod(method));
        m_request->setUrl(url);
        m_request->setVersion(HttpUtils::stringToVersion(version));
        
        // 转到头部解析状态
        m_parseState = ParseState::PARSE_HEADERS;
        return ParseResult::NEED_MORE_DATA;
    }
}

ParseResult HttpParser::parseStatusLine() {
    std::string line;
    if (!findLine(line)) {
        return ParseResult::NEED_MORE_DATA;
    }
    
    // 解析状态行: HTTP/VERSION STATUS_CODE REASON_PHRASE
    std::istringstream iss(line);
    std::string version;
    int statusCode;
    std::string reasonPhrase;
    
    if (!(iss >> version >> statusCode)) {
        setError(HttpError::INVALID_RESPONSE, "Invalid status line format");
        return ParseResult::PARSE_ERROR;
    }
    
    // 读取原因短语（可能包含空格）
    std::getline(iss, reasonPhrase);
    reasonPhrase = HttpUtils::trim(reasonPhrase);
    
    // 验证HTTP版本
    if (!isValidHttpVersion(version)) {
        setError(HttpError::UNSUPPORTED_VERSION, "Unsupported HTTP version: " + version);
        return ParseResult::PARSE_ERROR;
    }
    
    // 验证状态码
    if (!isValidStatusCode(statusCode)) {
        setError(HttpError::INVALID_RESPONSE, "Invalid status code: " + std::to_string(statusCode));
        return ParseResult::PARSE_ERROR;
    }
    
    // 创建响应对象
    if (!m_response) {
        m_response = std::make_shared<HttpResponse>();
    }
    
    m_response->setVersion(HttpUtils::stringToVersion(version));
    m_response->setStatusCode(static_cast<HttpStatusCode>(statusCode));
    if (!reasonPhrase.empty()) {
        m_response->setReasonPhrase(reasonPhrase);
    }
    
    // 转到头部解析状态
    m_parseState = ParseState::PARSE_HEADERS;
    return ParseResult::NEED_MORE_DATA;
}

ParseResult HttpParser::parseHeaders() {
    while (true) {
        std::string line;
        if (!findLine(line)) {
            return ParseResult::NEED_MORE_DATA;
        }
        
        // 空行表示头部结束
        if (line.empty()) {
            // 检查是否需要解析消息体
            size_t contentLength = getContentLength();
            bool hasBody = contentLength > 0 || isChunkedEncoding();
            
            if (hasBody) {
                m_expectedBodySize = contentLength;
                m_isChunked = isChunkedEncoding();
                m_parseState = ParseState::PARSE_BODY;
            } else {
                m_parseState = ParseState::PARSE_COMPLETE;
            }
            
            return ParseResult::NEED_MORE_DATA;
        }
        
        // 解析头部字段: name: value
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) {
            setError(HttpError::PARSE_ERROR, "Invalid header format: " + line);
            return ParseResult::PARSE_ERROR;
        }
        
        std::string name = HttpUtils::trim(line.substr(0, colonPos));
        std::string value = HttpUtils::trim(line.substr(colonPos + 1));
        
        if (name.empty()) {
            setError(HttpError::PARSE_ERROR, "Empty header name");
            return ParseResult::PARSE_ERROR;
        }
        
        // 添加头部到相应的对象
        if (m_request) {
            m_request->setHeader(name, value);
        } else if (m_response) {
            m_response->setHeader(name, value);
        }
        
        m_currentHeaderSize += line.length() + 2; // +2 for CRLF
    }
}

ParseResult HttpParser::parseBody() {
    if (m_expectedBodySize == 0) {
        m_parseState = ParseState::PARSE_COMPLETE;
        return ParseResult::PARSE_COMPLETE;
    }
    
    size_t availableData = m_buffer.length() - m_bufferOffset;
    size_t needData = m_expectedBodySize - m_currentBodySize;
    
    if (availableData < needData) {
        // 读取可用数据
        std::string bodyPart = m_buffer.substr(m_bufferOffset, availableData);
        if (m_request) {
            m_request->appendBody(bodyPart);
        } else if (m_response) {
            m_response->appendBody(bodyPart);
        }
        
        m_currentBodySize += availableData;
        consumeBuffer(availableData);
        
        return ParseResult::NEED_MORE_DATA;
    } else {
        // 读取剩余数据
        std::string bodyPart = m_buffer.substr(m_bufferOffset, needData);
        if (m_request) {
            m_request->appendBody(bodyPart);
        } else if (m_response) {
            m_response->appendBody(bodyPart);
        }
        
        m_currentBodySize += needData;
        consumeBuffer(needData);
        
        m_parseState = ParseState::PARSE_COMPLETE;
        return ParseResult::PARSE_COMPLETE;
    }
}

ParseResult HttpParser::parseChunkedBody() {
    while (true) {
        if (!m_chunkSizeRead) {
            // 读取块大小行
            std::string line;
            if (!findLine(line)) {
                return ParseResult::NEED_MORE_DATA;
            }
            
            // 解析块大小（十六进制）
            size_t semicolonPos = line.find(';');
            std::string sizeStr = (semicolonPos != std::string::npos) ? 
                                  line.substr(0, semicolonPos) : line;
            
            try {
                m_chunkSize = std::stoull(sizeStr, nullptr, 16);
            } catch (...) {
                setError(HttpError::PARSE_ERROR, "Invalid chunk size: " + sizeStr);
                return ParseResult::PARSE_ERROR;
            }
            
            m_chunkSizeRead = true;
            
            // 如果块大小为0，表示最后一块
            if (m_chunkSize == 0) {
                m_finalChunk = true;
                // 读取可能的尾随头部（暂时忽略）
                m_parseState = ParseState::PARSE_COMPLETE;
                return ParseResult::PARSE_COMPLETE;
            }
        } else {
            // 读取块数据
            size_t availableData = m_buffer.length() - m_bufferOffset;
            if (availableData < m_chunkSize + 2) { // +2 for trailing CRLF
                return ParseResult::NEED_MORE_DATA;
            }
            
            // 读取块数据
            std::string chunkData = m_buffer.substr(m_bufferOffset, m_chunkSize);
            if (m_request) {
                m_request->appendBody(chunkData);
            } else if (m_response) {
                m_response->appendBody(chunkData);
            }
            
            m_currentBodySize += m_chunkSize;
            consumeBuffer(m_chunkSize + 2); // +2 for CRLF
            
            // 重置块状态
            m_chunkSizeRead = false;
            m_chunkSize = 0;
        }
    }
}

// ============== 辅助方法 ==============

bool HttpParser::findLine(std::string& line) {
    size_t start = m_bufferOffset;
    size_t crlfPos = m_buffer.find("\r\n", start);
    
    if (crlfPos == std::string::npos) {
        // 尝试查找单独的\n
        size_t lfPos = m_buffer.find('\n', start);
        if (lfPos == std::string::npos) {
            return false;
        }
        
        line = m_buffer.substr(start, lfPos - start);
        m_bufferOffset = lfPos + 1;
    } else {
        line = m_buffer.substr(start, crlfPos - start);
        m_bufferOffset = crlfPos + 2;
    }
    
    return true;
}

size_t HttpParser::getContentLength() const {
    std::string lengthStr;
    if (m_request) {
        lengthStr = m_request->getHeader("Content-Length");
    } else if (m_response) {
        lengthStr = m_response->getHeader("Content-Length");
    }
    
    if (lengthStr.empty()) {
        return 0;
    }
    
    try {
        return std::stoull(lengthStr);
    } catch (...) {
        return 0;
    }
}

bool HttpParser::isChunkedEncoding() const {
    std::string encoding;
    if (m_request) {
        encoding = m_request->getHeader("Transfer-Encoding");
    } else if (m_response) {
        encoding = m_response->getHeader("Transfer-Encoding");
    }
    
    return HttpUtils::toLower(encoding).find("chunked") != std::string::npos;
}

bool HttpParser::needMoreData() const {
    return m_buffer.length() <= m_bufferOffset;
}

// ============== 数据处理 ==============

void HttpParser::appendBuffer(const char* data, size_t length) {
    m_buffer.append(data, length);
    m_parsedBytes += length;
}

void HttpParser::consumeBuffer(size_t length) {
    m_bufferOffset += length;
    
    // 如果缓冲区偏移量过大，压缩缓冲区
    if (m_bufferOffset > m_buffer.length() / 2) {
        m_buffer = m_buffer.substr(m_bufferOffset);
        m_bufferOffset = 0;
    }
}

// ============== 错误处理和验证 ==============

void HttpParser::setError(HttpError error, const std::string& message) {
    m_lastError = error;
    m_errorMessage = message;
    m_parseState = ParseState::PARSE_ERROR;
}

bool HttpParser::checkLimits() {
    if (m_currentHeaderSize > m_maxHeaderSize) {
        setError(HttpError::HEADER_TOO_LARGE, "Header size exceeds limit");
        return false;
    }
    
    if (m_currentBodySize > m_maxBodySize) {
        setError(HttpError::BODY_TOO_LARGE, "Body size exceeds limit");
        return false;
    }
    
    return true;
}

// ============== 重置和清理 ==============

void HttpParser::reset() {
    m_parseState = (m_sessionMode == SessionMode::SERVER) ? 
                   ParseState::PARSE_REQUEST_LINE : ParseState::PARSE_REQUEST_LINE;
    m_lastError = HttpError::NONE;
    m_errorMessage.clear();
    
    m_request.reset();
    m_response.reset();
    
    m_buffer.clear();
    m_bufferOffset = 0;
    m_parsedBytes = 0;
    m_currentHeaderSize = 0;
    m_currentBodySize = 0;
    m_expectedBodySize = 0;
    
    m_isChunked = false;
    m_chunkSize = 0;
    m_chunkSizeRead = false;
    m_finalChunk = false;
}

void HttpParser::clear() {
    reset();
    m_sessionMode = SessionMode::UNKNOWN;
}

// ============== 工具方法 ==============

bool HttpParser::isValidHttpMethod(const std::string& method) {
    static const std::set<std::string> validMethods = {
        "GET", "POST", "PUT", "DELETE"
    };
    return validMethods.find(HttpUtils::toUpper(method)) != validMethods.end();
}

bool HttpParser::isValidHttpVersion(const std::string& version) {
    return version == "HTTP/1.0" || version == "HTTP/1.1" || version == "HTTP/2.0";
}

bool HttpParser::isValidStatusCode(int statusCode) {
    return statusCode >= 100 && statusCode < 600;
}