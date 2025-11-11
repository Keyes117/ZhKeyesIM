/**
 * @desc:   Http消息基类实现，HttpMessage.cpp
 * @author: ZhKeyes
 * @date:   2025/8/5
 */


#include "HttpMessage.h"

#include <algorithm>
#include <iterator>


#include "fmt/format.h"

#include "HttpProtocol.h"

using namespace ZhKeyesIM::Net::Http;

void HttpMessage::setHeader(const std::string& name, const std::string& value)
{
    std::string nomalizedName = normalizeHeaderName(name);
    m_headers[nomalizedName] = value;
}

std::string HttpMessage::getHeader(const std::string& name) const {
    std::string normalizedName = normalizeHeaderName(name);
    auto it = m_headers.find(normalizedName);
    return (it != m_headers.end()) ? it->second : "";
}

bool HttpMessage::hasHeader(const std::string& name) const {
    std::string normalizedName = normalizeHeaderName(name);
    return m_headers.find(normalizedName) != m_headers.end();
}

void HttpMessage::removeHeader(const std::string& name) {
    std::string normalizedName = normalizeHeaderName(name);
    m_headers.erase(normalizedName);
}

// ============== 常用头部的便捷方法 ==============

void HttpMessage::setContentType(const std::string& contentType) {
    setHeader("Content-Type", contentType);
}

std::string HttpMessage::getContentType() const {
    return getHeader("Content-Type");
}

void HttpMessage::setContentLength(size_t length) {
    setHeader("Content-Length", std::to_string(length));
}

size_t HttpMessage::getContentLength() const {
    std::string lengthStr = getHeader("Content-Length");
    if (lengthStr.empty()) {
        return m_body.size(); // 如果没有设置Content-Length，返回实际body大小
    }
    try {
        return std::stoull(lengthStr);
    }
    catch (...) {
        return 0;
    }
}

void HttpMessage::setConnection(const std::string& connection) {
    setHeader("Connection", connection);
}

std::string HttpMessage::getConnection() const {
    return getHeader("Connection");
}

bool HttpMessage::isKeepAlive() const {
    std::string connection = HttpUtils::toLower(getConnection());
    if (m_version == HttpVersion::HTTP_1_1) {
        // HTTP/1.1默认是keep-alive，除非明确指定close
        return connection != "close";
    }
    else {
        // HTTP/1.0默认是close，除非明确指定keep-alive
        return connection == "keep-alive";
    }
}

// ============== 工具方法 ==============

void HttpMessage::clear() {
    m_version = HttpVersion::HTTP_1_1;
    m_headers.clear();
    m_body.clear();
}

bool HttpMessage::empty() const {
    return m_headers.empty() && m_body.empty();
}

std::string HttpMessage::normalizeHeaderName(const std::string& name) const {
    return HttpUtils::normalizeHeaderName(name);
}

std::string HttpMessage::headersToString() const {
    fmt::memory_buffer buf;
    for (const auto& header : m_headers)
    {
        fmt::format_to(std::back_inserter(buf), "{}: {}\r\n", header.first, header.second);
    }
    return fmt::to_string(buf);
}

bool HttpMessage::parseHeaders(const std::string& headerData) {
    std::string_view sv(headerData);


    while (!sv.empty())
    {
        size_t posCRLF = sv.find("\r\n");
        if (posCRLF == std::string_view::npos)
        {
            posCRLF = sv.find('\n');
            if (posCRLF == std::string_view::npos)
                break;
        }

        std::string_view line = sv.substr(0, posCRLF);
        sv.remove_prefix(posCRLF + (sv[posCRLF] == '\r' ? 2 : 1));

        if (line.empty())
            break;


        // 查找冒号分隔符
        size_t colonPos = line.find(':');
        if (colonPos == std::string_view::npos) {
            return false; // 格式错误
        }

        std::string name = HttpUtils::trimString(std::string(line.substr(0, colonPos)));
        std::string value = HttpUtils::trimString(std::string(line.substr(colonPos + 1)));

        if (name.empty()) {
            return false; // 头部名称不能为空
        }

        setHeader(name, value);
    }

    return true;
}