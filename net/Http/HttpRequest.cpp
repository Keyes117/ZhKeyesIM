/**
 * @desc:   Http请求类实现，HttpRequest.cpp
 * @author: ZhKeyes
 * @date:   2025/8/5
 */
#include "HttpRequest.h"
#include <algorithm>
#include <sstream>

using namespace ZhKeyesIM::Net::Http;

HttpRequest::HttpRequest() : m_method(HttpMethod::GET) {
    // 默认构造函数
}

HttpRequest::HttpRequest(HttpMethod method, const std::string& url)
    : m_method(method) {
    setUrl(url);
}

// ============== 请求行相关 ==============

void HttpRequest::setUrl(const std::string& url) {
    m_url = url;
    parseUrl(url);
}

void HttpRequest::setQuery(const std::string& query) {
    m_query = query;
    parseQueryString(query);
}

// ============== 查询参数操作 ==============

void HttpRequest::setQueryParam(const std::string& name, const std::string& value) {
    m_queryParams[name] = value;
    updateQueryString();
}

std::string HttpRequest::getQueryParam(const std::string& name) const {
    auto it = m_queryParams.find(name);
    return (it != m_queryParams.end()) ? it->second : "";
}

bool HttpRequest::hasQueryParam(const std::string& name) const {
    return m_queryParams.find(name) != m_queryParams.end();
}

void HttpRequest::removeQueryParam(const std::string& name) {
    m_queryParams.erase(name);
    updateQueryString();
}

// ============== 常用头部便捷方法 ==============

void HttpRequest::setHost(const std::string& host) {
    setHeader("Host", host);
}

std::string HttpRequest::getHost() const {
    return getHeader("Host");
}

void HttpRequest::setUserAgent(const std::string& userAgent) {
    setHeader("User-Agent", userAgent);
}

std::string HttpRequest::getUserAgent() const {
    return getHeader("User-Agent");
}

void HttpRequest::setReferer(const std::string& referer) {
    setHeader("Referer", referer);
}

std::string HttpRequest::getReferer() const {
    return getHeader("Referer");
}

void HttpRequest::setAccept(const std::string& accept) {
    setHeader("Accept", accept);
}

std::string HttpRequest::getAccept() const {
    return getHeader("Accept");
}

void HttpRequest::setAuthorization(const std::string& auth) {
    setHeader("Authorization", auth);
}

std::string HttpRequest::getAuthorization() const {
    return getHeader("Authorization");
}

// ============== Cookie操作 ==============

void HttpRequest::setCookie(const std::string& name, const std::string& value) {
    auto cookies = parseCookies();
    cookies[name] = value;

    std::ostringstream oss;
    bool first = true;
    for (const auto& cookie : cookies) {
        if (!first) oss << "; ";
        oss << cookie.first << "=" << cookie.second;
        first = false;
    }
    setHeader("Cookie", oss.str());
}

std::string HttpRequest::getCookie(const std::string& name) const {
    auto cookies = parseCookies();
    auto it = cookies.find(name);
    return (it != cookies.end()) ? it->second : "";
}

bool HttpRequest::hasCookie(const std::string& name) const {
    auto cookies = parseCookies();
    return cookies.find(name) != cookies.end();
}

std::string HttpRequest::getCookieHeader() const {
    return getHeader("Cookie");
}

// ============== 表单数据处理 ==============

void HttpRequest::setFormData(const std::unordered_map<std::string, std::string>& formData) {
    m_formData = formData;
    setBody(getFormDataString());
    setContentType("application/x-www-form-urlencoded");
}

void HttpRequest::addFormData(const std::string& name, const std::string& value) {
    m_formData[name] = value;
    setBody(getFormDataString());
    setContentType("application/x-www-form-urlencoded");
}

std::string HttpRequest::getFormDataString() const {
    std::ostringstream oss;
    bool first = true;
    for (const auto& pair : m_formData) {
        if (!first) oss << "&";
        oss << HttpUtils::urlEncode(pair.first) << "=" << HttpUtils::urlEncode(pair.second);
        first = false;
    }
    return oss.str();
}

// ============== JSON数据处理 ==============

void HttpRequest::setJsonBody(const std::string& json) {
    setBody(json);
    setContentType("application/json; charset=utf-8");
}

// ============== 序列化和解析 ==============

std::string HttpRequest::toString() const {
    std::ostringstream oss;

    // 请求行
    oss << getMethodString() << HttpConstants::SPACE << m_path;
    if (!m_query.empty()) {
        oss << "?" << m_query;
    }
    oss << HttpConstants::SPACE << getVersionString() << HttpConstants::CRLF;

    // 头部
    oss << headersToString();

    // 空行分隔头部和消息体
    oss << HttpConstants::CRLF;

    // 消息体
    oss << getBody();

    return oss.str();
}

bool HttpRequest::fromString(const std::string& data) {
    clear();

    std::istringstream iss(data);
    std::string line;

    // 解析请求行
    if (!std::getline(iss, line)) {
        return false;
    }

    // 移除行尾的\r
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }

    std::istringstream lineStream(line);
    std::string methodStr, url, versionStr;
    if (!(lineStream >> methodStr >> url >> versionStr)) {
        return false;
    }

    m_method = HttpUtils::stringToMethod(methodStr);
    setUrl(url);
    setVersion(HttpUtils::stringToVersion(versionStr));

    // 读取剩余数据用于解析头部和消息体
    std::ostringstream remainingData;
    remainingData << iss.rdbuf();
    std::string remaining = remainingData.str();

    // 查找头部和消息体的分界线（空行）
    size_t headerEndPos = remaining.find("\r\n\r\n");
    if (headerEndPos == std::string::npos) {
        headerEndPos = remaining.find("\n\n");
        if (headerEndPos == std::string::npos) {
            // 没有找到分界线，假设只有头部没有消息体
            return parseHeaders(remaining);
        }
        headerEndPos += 2; // \n\n的长度
    }
    else {
        headerEndPos += 4; // \r\n\r\n的长度
    }

    // 解析头部
    std::string headerData = remaining.substr(0, headerEndPos - 4);
    if (!parseHeaders(headerData)) {
        return false;
    }

    // 解析消息体
    if (headerEndPos < remaining.length()) {
        std::string bodyData = remaining.substr(headerEndPos);
        setBody(bodyData);
    }

    return true;
}

// ============== 工具方法 ==============

void HttpRequest::clear() {
    HttpMessage::clear();
    m_method = HttpMethod::GET;
    m_url.clear();
    m_path.clear();
    m_query.clear();
    m_queryParams.clear();
    m_formData.clear();
}

bool HttpRequest::isFormRequest() const {
    std::string contentType = HttpUtils::toLower(getContentType());
    return contentType.find("application/x-www-form-urlencoded") != std::string::npos;
}

bool HttpRequest::isJsonRequest() const {
    std::string contentType = HttpUtils::toLower(getContentType());
    return contentType.find("application/json") != std::string::npos;
}

bool HttpRequest::isMultipartRequest() const {
    std::string contentType = HttpUtils::toLower(getContentType());
    return contentType.find("multipart/form-data") != std::string::npos;
}

// ============== 私有方法实现 ==============

void HttpRequest::parseUrl(const std::string& url) {
    // 查找问号分隔路径和查询字符串
    size_t queryPos = url.find('?');
    if (queryPos != std::string::npos) {
        m_path = url.substr(0, queryPos);
        m_query = url.substr(queryPos + 1);
        parseQueryString(m_query);
    }
    else {
        m_path = url;
        m_query.clear();
        m_queryParams.clear();
    }
}

void HttpRequest::updateQueryString() {
    if (m_queryParams.empty()) {
        m_query.clear();
        return;
    }

    std::ostringstream oss;
    bool first = true;
    for (const auto& param : m_queryParams) {
        if (!first) oss << "&";
        oss << HttpUtils::urlEncode(param.first) << "=" << HttpUtils::urlEncode(param.second);
        first = false;
    }
    m_query = oss.str();
}

void HttpRequest::parseQueryString(const std::string& queryString) {
    m_queryParams.clear();

    if (queryString.empty()) {
        return;
    }

    std::istringstream iss(queryString);
    std::string pair;

    while (std::getline(iss, pair, '&')) {
        size_t equalPos = pair.find('=');
        if (equalPos != std::string::npos) {
            std::string name = HttpUtils::urlDecode(pair.substr(0, equalPos));
            std::string value = HttpUtils::urlDecode(pair.substr(equalPos + 1));
            m_queryParams[name] = value;
        }
    }
}

std::unordered_map<std::string, std::string> HttpRequest::parseCookies() const {
    std::unordered_map<std::string, std::string> cookies;
    std::string cookieHeader = getHeader("Cookie");

    if (cookieHeader.empty()) {
        return cookies;
    }

    std::istringstream iss(cookieHeader);
    std::string pair;

    while (std::getline(iss, pair, ';')) {
        pair = HttpUtils::trimString(pair);
        size_t equalPos = pair.find('=');
        if (equalPos != std::string::npos) {
            std::string name = HttpUtils::trimString(pair.substr(0, equalPos));
            std::string value = HttpUtils::trimString(pair.substr(equalPos + 1));
            cookies[name] = value;
        }
    }

    return cookies;
}