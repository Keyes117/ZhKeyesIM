/**
 * @desc:   Http响应类实现，HttpResponse.cpp
 * @author: ZhKeyes
 * @date:   2025/8/5
 */
#include "HttpResponse.h"
#include <ctime>
#include <fstream>
#include <iterator>

#include "fmt/format.h"


using namespace ZhKeyesIM::Net::Http;

 // ============== Cookie结构体实现 ==============

std::string HttpResponse::Cookie::toString() const 
{
    fmt::memory_buffer buf;
    fmt::format_to(std::back_inserter(buf), "{}={}", name, value);

    if (!path.empty()) {
        fmt::format_to(std::back_inserter(buf), "; Path={}", path);
    }

    if (!domain.empty()) {
        fmt::format_to(std::back_inserter(buf), "; Domain={}", domain);
    }

    if (!expires.empty()) {
        fmt::format_to(std::back_inserter(buf), "; Expires={}", expires);
    }

    if (maxAge >= 0) {
        fmt::format_to(std::back_inserter(buf), "; Max-Age={}", maxAge);
    }

    if (secure) {
        fmt::format_to(std::back_inserter(buf), "; Secure");
    }

    if (httpOnly) {
        fmt::format_to(std::back_inserter(buf), "; HttpOnly");
    }

    if (!sameSite.empty()) {
        fmt::format_to(std::back_inserter(buf), "; SameSite={}", sameSite);
    }

    return fmt::to_string(buf);
}

// ============== 构造函数 ==============

HttpResponse::HttpResponse() : m_statusCode(HttpStatusCode::OK) {
    setCurrentDate();
}

HttpResponse::HttpResponse(HttpStatusCode statusCode) : m_statusCode(statusCode) {
    setCurrentDate();
}

HttpResponse::HttpResponse(HttpStatusCode statusCode, const std::string& body)
    : m_statusCode(statusCode) {
    setBody(body);
    setContentLength(body.length());
    setCurrentDate();
}

// ============== 状态行相关 ==============

std::string HttpResponse::getReasonPhrase() const {
    if (!m_reasonPhrase.empty()) {
        return m_reasonPhrase;
    }
    return HttpUtils::getReasonPhrase(m_statusCode);
}

// ============== 常用响应头便捷方法 ==============

void HttpResponse::setServer(const std::string& server) {
    setHeader("Server", server);
}

std::string HttpResponse::getServer() const {
    return getHeader("Server");
}

void HttpResponse::setDate(const std::string& date) {
    setHeader("Date", date);
}

std::string HttpResponse::getDate() const {
    return getHeader("Date");
}

void HttpResponse::setCurrentDate() {
    setHeader("Date", HttpUtils::formatHttpDate());
}

void HttpResponse::setLastModified(const std::string& lastModified) {
    setHeader("Last-Modified", lastModified);
}

std::string HttpResponse::getLastModified() const {
    return getHeader("Last-Modified");
}

void HttpResponse::setLastModified(time_t timestamp) {
    setHeader("Last-Modified", HttpUtils::formatHttpDate(timestamp));
}

void HttpResponse::setETag(const std::string& etag) {
    setHeader("ETag", etag);
}

std::string HttpResponse::getETag() const {
    return getHeader("ETag");
}

void HttpResponse::setLocation(const std::string& location) {
    setHeader("Location", location);
}

std::string HttpResponse::getLocation() const {
    return getHeader("Location");
}

void HttpResponse::setCacheControl(const std::string& cacheControl) {
    setHeader("Cache-Control", cacheControl);
}

std::string HttpResponse::getCacheControl() const {
    return getHeader("Cache-Control");
}

// ============== Cookie操作 ==============

void HttpResponse::setCookie(const Cookie& cookie) {
    std::string cookieHeader = cookie.toString();

    // 获取现有的Set-Cookie头部
    auto& headers = getHeaders();
    auto it = headers.find("Set-Cookie");
    if (it != headers.end()) {
        it->second += "\r\n" + cookieHeader;
    }
    else {
        setHeader("Set-Cookie", cookieHeader);
    }
}

void HttpResponse::setCookie(const std::string& name, const std::string& value) {
    Cookie cookie;
    cookie.name = name;
    cookie.value = value;
    setCookie(cookie);
}

void HttpResponse::setCookie(const std::string& name, const std::string& value,
    const std::string& path, const std::string& domain,
    int maxAge, bool secure, bool httpOnly) {
    Cookie cookie;
    cookie.name = name;
    cookie.value = value;
    cookie.path = path;
    cookie.domain = domain;
    cookie.maxAge = maxAge;
    cookie.secure = secure;
    cookie.httpOnly = httpOnly;
    setCookie(cookie);
}

std::vector<HttpResponse::Cookie> HttpResponse::getCookies() const {
    return parseCookiesFromHeaders();
}

void HttpResponse::clearCookies() {
    removeHeader("Set-Cookie");
}

// ============== 状态判断 ==============

bool HttpResponse::isRedirection() const {
    int code = static_cast<int>(m_statusCode);
    return code >= 300 && code < 400;
}

// ============== 便捷响应构建 ==============

void HttpResponse::setTextResponse(const std::string& text) {
    setBody(text);
    setContentType("text/plain; charset=utf-8");
    setContentLength(text.length());
}

void HttpResponse::setHtmlResponse(const std::string& html) {
    setBody(html);
    setContentType("text/html; charset=utf-8");
    setContentLength(html.length());
}

void HttpResponse::setJsonResponse(const std::string& json) {
    setBody(json);
    setContentType("application/json; charset=utf-8");
    setContentLength(json.length());
}

void HttpResponse::setFileResponse(const std::string& filePath, const std::string& fileName) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        setErrorResponse(HttpStatusCode::NotFound, "File not found");
        return;
    }

    // 读取文件内容
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string content(fileSize, '\0');
    file.read(&content[0], fileSize);
    file.close();

    // 设置响应
    setBody(content);
    setContentLength(fileSize);

    // 获取文件扩展名并设置MIME类型
    size_t dotPos = filePath.find_last_of('.');
    if (dotPos != std::string::npos) {
        std::string extension = filePath.substr(dotPos);
        setContentType(HttpUtils::getMimeType(extension));
    }

    // 设置下载文件名
    if (!fileName.empty()) {
        setHeader("Content-Disposition", "attachment; filename=\"" + fileName + "\"");
    }
}

void HttpResponse::setRedirectResponse(const std::string& location, bool permanent) {
    setStatusCode(permanent ? HttpStatusCode::MovedPermanently : HttpStatusCode::Found);
    setLocation(location);
    setBody("");
}

void HttpResponse::setErrorResponse(HttpStatusCode statusCode, const std::string& message) {
    setStatusCode(statusCode);

    std::string errorMessage = message;
    if (errorMessage.empty()) {
        errorMessage = getReasonPhrase();
    }

    std::string html = "<!DOCTYPE html>\n"
        "<html><head><title>" + std::to_string(static_cast<int>(statusCode)) + " " + getReasonPhrase() + "</title></head>\n"
        "<body><h1>" + std::to_string(static_cast<int>(statusCode)) + " " + getReasonPhrase() + "</h1>\n"
        "<p>" + errorMessage + "</p></body></html>";

    setHtmlResponse(html);
}

// ============== 序列化和解析 ==============

std::string HttpResponse::toString() const
{
    fmt::memory_buffer buf;

    // 状态行
    fmt::format_to(std::back_inserter(buf), "{} {} {}\r\n",
        getVersionString(),
        static_cast<int>(m_statusCode),
        getReasonPhrase());

    // 头部
    fmt::format_to(std::back_inserter(buf), "{}", headersToString());

    // 空行分隔头部和消息体
    fmt::format_to(std::back_inserter(buf), "\r\n");

    // 消息体
    fmt::format_to(std::back_inserter(buf), "{}", getBody());

    return fmt::to_string(buf);
}

bool HttpResponse::fromString(const std::string& data)
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

    std::string_view statusLine = sv.substr(0, lineEnd);
    sv.remove_prefix(lineEnd + (sv[lineEnd] == '\r' ? 2 : 1));

    // 解析状态行: HTTP/VERSION SP STATUS_CODE SP REASON_PHRASE
    size_t sp1 = statusLine.find(' ');
    if (sp1 == std::string_view::npos) return false;

    size_t sp2 = statusLine.find(' ', sp1 + 1);
    if (sp2 == std::string_view::npos) return false;

    std::string versionStr(statusLine.substr(0, sp1));
    std::string statusCodeStr(statusLine.substr(sp1 + 1, sp2 - sp1 - 1));
    std::string reasonPhrase(statusLine.substr(sp2 + 1));

    setVersion(HttpUtils::stringToVersion(versionStr));

    try {
        m_statusCode = static_cast<HttpStatusCode>(std::stoi(statusCodeStr));
    }
    catch (...) {
        return false;
    }

    m_reasonPhrase = HttpUtils::trimString(reasonPhrase);

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

void HttpResponse::clear() {
    HttpMessage::clear();
    m_statusCode = HttpStatusCode::OK;
    m_reasonPhrase.clear();
}

std::vector<HttpResponse::Cookie> HttpResponse::parseCookiesFromHeaders() const {
    std::vector<Cookie> cookies;
    std::string setCookieHeader = getHeader("Set-Cookie");

    if (setCookieHeader.empty()) {
        return cookies;
    }

    // 解析Set-Cookie头部（可能有多个，用\r\n分隔）
    std::string_view sv(setCookieHeader);

    while (!sv.empty()) {
        size_t lineEnd = sv.find("\r\n");
        if (lineEnd == std::string_view::npos) {
            lineEnd = sv.find('\n');
        }

        std::string_view cookieLine = sv.substr(0, lineEnd);
        if (cookieLine.empty()) {
            if (lineEnd == std::string_view::npos) break;
            sv.remove_prefix(lineEnd + 1);
            continue;
        }

        Cookie cookie;
        bool isFirst = true;

        while (!cookieLine.empty()) {
            size_t semicolonPos = cookieLine.find(';');
            std::string_view attribute = cookieLine.substr(0, semicolonPos);
            attribute = std::string_view(HttpUtils::trimString(std::string(attribute)));

            if (isFirst) {
                // 第一个部分是name=value
                size_t equalPos = attribute.find('=');
                if (equalPos != std::string_view::npos) {
                    cookie.name = HttpUtils::trimString(std::string(attribute.substr(0, equalPos)));
                    cookie.value = HttpUtils::trimString(std::string(attribute.substr(equalPos + 1)));
                }
                isFirst = false;
            }
            else {
                // 解析其他属性
                size_t equalPos = attribute.find('=');
                if (equalPos != std::string_view::npos) {
                    std::string attrName = HttpUtils::toLower(HttpUtils::trimString(std::string(attribute.substr(0, equalPos))));
                    std::string attrValue = HttpUtils::trimString(std::string(attribute.substr(equalPos + 1)));

                    if (attrName == "path") {
                        cookie.path = attrValue;
                    }
                    else if (attrName == "domain") {
                        cookie.domain = attrValue;
                    }
                    else if (attrName == "expires") {
                        cookie.expires = attrValue;
                    }
                    else if (attrName == "max-age") {
                        try {
                            cookie.maxAge = std::stoi(attrValue);
                        }
                        catch (...) {
                            cookie.maxAge = -1;
                        }
                    }
                    else if (attrName == "samesite") {
                        cookie.sameSite = attrValue;
                    }
                }
                else {
                    // 布尔属性
                    std::string attrName = HttpUtils::toLower(std::string(attribute));
                    if (attrName == "secure") {
                        cookie.secure = true;
                    }
                    else if (attrName == "httponly") {
                        cookie.httpOnly = true;
                    }
                }
            }

            if (semicolonPos == std::string_view::npos) break;
            cookieLine.remove_prefix(semicolonPos + 1);
        }

        if (!cookie.name.empty()) {
            cookies.push_back(cookie);
        }

        if (lineEnd == std::string_view::npos) break;
        sv.remove_prefix(lineEnd + (sv[lineEnd] == '\r' ? 2 : 1));
    }

    return cookies;
}