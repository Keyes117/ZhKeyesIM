/**
 * @desc:   Http请求类实现，HttpRequest.cpp
 * @author: ZhKeyes
 * @date:   2025/8/5
 */
#include "HttpRequest.h"

#include <algorithm>
#include <iterator>

#include "fmt/format.h"

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

void HttpRequest::setQuery(const std::string& query)
{
    m_query = query;
    parseQueryString(query);
}

// ============== 查询参数操作 ==============

void HttpRequest::setQueryParam(const std::string& name, const std::string& value) 
{
    m_queryParams[name] = value;
    updateQueryString();
}

std::string HttpRequest::getQueryParam(const std::string& name) const 
{
    auto it = m_queryParams.find(name);
    return (it != m_queryParams.end()) ? it->second : "";
}

bool HttpRequest::hasQueryParam(const std::string& name) const {
    return m_queryParams.find(name) != m_queryParams.end();
}

void HttpRequest::removeQueryParam(const std::string& name)
{
    m_queryParams.erase(name);
    updateQueryString();
}

// ============== 常用头部便捷方法 ==============

void HttpRequest::setHost(const std::string& host)
{
    setHeader("Host", host);
}

std::string HttpRequest::getHost() const
{
    return getHeader("Host");
}

void HttpRequest::setUserAgent(const std::string& userAgent)
{
    setHeader("User-Agent", userAgent);
}

std::string HttpRequest::getUserAgent() const
{
    return getHeader("User-Agent");
}

void HttpRequest::setReferer(const std::string& referer)
{
    setHeader("Referer", referer);
}

std::string HttpRequest::getReferer() const
{
    return getHeader("Referer");
}

void HttpRequest::setAccept(const std::string& accept)
{
    setHeader("Accept", accept);
}

std::string HttpRequest::getAccept() const
{
    return getHeader("Accept");
}

void HttpRequest::setAuthorization(const std::string& auth) 
{
    setHeader("Authorization", auth);
}

std::string HttpRequest::getAuthorization() const
{
    return getHeader("Authorization");
}

// ============== Cookie操作 ==============

void HttpRequest::setCookie(const std::string& name, const std::string& value) {
    auto cookies = parseCookies();
    cookies[name] = value;

    fmt::memory_buffer buf;
    bool first = true;
    for (const auto& cookie : cookies) 
    {
        if (!first) 
            fmt::format_to(std::back_inserter(buf), "; ");

        fmt::format_to(std::back_inserter(buf),"{}={}", cookie.first, cookie.second);
        first = false;
    }
    setHeader("Cookie", fmt::to_string(buf));
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
    fmt::memory_buffer buf;
    bool first = true;
    for (const auto& pair : m_formData)
    {
        if (!first)
            fmt::format_to(std::back_inserter(buf), "&");

        fmt::format_to(std::back_inserter(buf), "{}={}",
                     HttpUtils::urlEncode(pair.first),
                     HttpUtils::urlEncode(pair.second));
  
        first = false;
    }
    return fmt::to_string(buf);
    
}

// ============== JSON数据处理 ==============

void HttpRequest::setJsonBody(const std::string& json) {
    setBody(json);
    setContentType("application/json; charset=utf-8");
}

// ============== 序列化和解析 ==============

std::string HttpRequest::toString() const {
    fmt::memory_buffer buf;

    fmt::format_to(std::back_inserter(buf), "{} {}", getMethodString(), m_path);
    // 请求行

    if (!m_query.empty()) {
        fmt::format_to(std::back_inserter(buf), "?{}",m_query);
    }

    fmt::format_to(std::back_inserter(buf), "{}{}{}", HttpConstants::SPACE, getMethodString(), HttpConstants::CRLF);
    // 头部
    fmt::format_to(std::back_inserter(buf), "{}", headersToString());


    // 空行分隔头部和消息体
    fmt::format_to(std::back_inserter(buf), "{}", HttpConstants::CRLF);

    // 消息体
    fmt::format_to(std::back_inserter(buf), "{}", getBody());


    return fmt::to_string(buf);
}

bool HttpRequest::fromString(const std::string& data)
{
    clear();

    std::string_view sv(data);

    // 查找第一行结束符
    size_t lineEnd = sv.find("\r\n");
    if (lineEnd == std::string_view::npos) {
        lineEnd = sv.find('\n');
        if (lineEnd == std::string_view::npos) {
            return false;
        }
    }

    std::string_view requestLine = sv.substr(0, lineEnd);
    sv.remove_prefix(lineEnd + (sv[lineEnd] == '\r' ? 2 : 1));

    // 解析请求行: METHOD SP URL SP HTTP/VERSION
    size_t sp1 = requestLine.find(' ');
    if (sp1 == std::string_view::npos) return false;

    size_t sp2 = requestLine.find(' ', sp1 + 1);
    if (sp2 == std::string_view::npos) return false;

    std::string methodStr(requestLine.substr(0, sp1));
    std::string url(requestLine.substr(sp1 + 1, sp2 - sp1 - 1));
    std::string versionStr(requestLine.substr(sp2 + 1));

    m_method = HttpUtils::stringToMethod(methodStr);
    setUrl(url);
    setVersion(HttpUtils::stringToVersion(versionStr));

    // 查找头部和消息体的分界线（空行）
    size_t headerEndPos = sv.find("\r\n\r\n");
    size_t headerLen = 4;
    if (headerEndPos == std::string_view::npos) {
        headerEndPos = sv.find("\n\n");
        headerLen = 2;
        if (headerEndPos == std::string_view::npos) {
            // 没有找到分界线，假设只有头部没有消息体
            return parseHeaders(std::string(sv));
        }
    }

    // 解析头部
    std::string headerData(sv.substr(0, headerEndPos));
    if (!parseHeaders(headerData)) {
        return false;
    }

    // 解析消息体
    sv.remove_prefix(headerEndPos + headerLen);
    if (!sv.empty()) {
        setBody(std::string(sv));
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

    fmt::memory_buffer buf;
    bool first = true;
    for (const auto& param : m_queryParams) {
        if (!first) fmt::format_to(std::back_inserter(buf), "&");
        fmt::format_to(std::back_inserter(buf), "{}={}",
            HttpUtils::urlEncode(param.first),
            HttpUtils::urlEncode(param.second));
        first = false;
    }
    m_query = fmt::to_string(buf);
}

void HttpRequest::parseQueryString(const std::string& queryString) {
    m_queryParams.clear();

    if (queryString.empty()) 
    {
        return;
    }

    std::string_view sv(queryString);
    while (!sv.empty()) 
    {
        size_t ampPos = sv.find('&');
        std::string_view pair = sv.substr(0, ampPos);

        size_t equalPos = pair.find('=');
        if (equalPos != std::string_view::npos)
        {
            std::string name = HttpUtils::urlDecode(std::string(pair.substr(0, equalPos)));
            std::string value = HttpUtils::urlDecode(std::string(pair.substr(equalPos + 1)));
            m_queryParams[name] = value;
        }

        if (ampPos == std::string_view::npos)
            break;

        sv.remove_prefix(ampPos + 1);
    }
}

std::unordered_map<std::string, std::string> HttpRequest::parseCookies() const {
    std::unordered_map<std::string, std::string> cookies;
    std::string cookieHeader = getHeader("Cookie");

    if (cookieHeader.empty()) {
        return cookies;
    }

    std::string_view sv(cookieHeader);
    while (!sv.empty())
    {
        size_t semicolonPos = sv.find(';');
        std::string_view pair = HttpUtils::trimString(std::string(sv.substr(0, semicolonPos)));

        size_t equalPos = pair.find('=');
        if (equalPos != std::string_view::npos) 
        {
            std::string name = HttpUtils::trimString(std::string(pair.substr(0, equalPos)));
            std::string value = HttpUtils::trimString(std::string(pair.substr(equalPos + 1)));
            cookies[name] = value;
        }


        if (semicolonPos == std::string_view::npos)
            break;
        sv.remove_prefix(semicolonPos + 1);
    }

    return cookies;
}