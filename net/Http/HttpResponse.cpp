/**
 * @desc:   Http响应类实现，HttpResponse.cpp
 * @author: ZhKeyes
 * @date:   2025/8/5
 */
#include "HttpResponse.h"
#include <ctime>
#include <fstream>
#include <sstream>

 // ============== Cookie结构体实现 ==============

std::string HttpResponse::Cookie::toString() const {
    std::ostringstream oss;
    oss << name << "=" << value;

    if (!path.empty()) {
        oss << "; Path=" << path;
    }

    if (!domain.empty()) {
        oss << "; Domain=" << domain;
    }

    if (!expires.empty()) {
        oss << "; Expires=" << expires;
    }

    if (maxAge >= 0) {
        oss << "; Max-Age=" << maxAge;
    }

    if (secure) {
        oss << "; Secure";
    }

    if (httpOnly) {
        oss << "; HttpOnly";
    }

    if (!sameSite.empty()) {
        oss << "; SameSite=" << sameSite;
    }

    return oss.str();
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

std::string HttpResponse::toString() const {
    std::ostringstream oss;

    // 状态行
    oss << getVersionString() << HttpConstants::SPACE
        << static_cast<int>(m_statusCode) << HttpConstants::SPACE
        << getReasonPhrase() << HttpConstants::CRLF;

    // 头部
    oss << headersToString();

    // 空行分隔头部和消息体
    oss << HttpConstants::CRLF;

    // 消息体
    oss << getBody();

    return oss.str();
}

bool HttpResponse::fromString(const std::string& data) {
    clear();

    std::istringstream iss(data);
    std::string line;

    // 解析状态行
    if (!std::getline(iss, line)) {
        return false;
    }

    // 移除行尾的\r
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }

    std::istringstream lineStream(line);
    std::string versionStr;
    int statusCode;
    std::string reasonPhrase;

    if (!(lineStream >> versionStr >> statusCode)) {
        return false;
    }

    // 读取原因短语（可能包含空格）
    std::getline(lineStream, reasonPhrase);
    reasonPhrase = HttpUtils::trimString(reasonPhrase);

    setVersion(HttpUtils::stringToVersion(versionStr));
    m_statusCode = static_cast<HttpStatusCode>(statusCode);
    m_reasonPhrase = reasonPhrase;

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
    std::istringstream iss(setCookieHeader);
    std::string cookieLine;

    while (std::getline(iss, cookieLine)) {
        if (cookieLine.empty()) continue;

        Cookie cookie;
        std::istringstream cookieStream(cookieLine);
        std::string attribute;

        bool isFirst = true;
        while (std::getline(cookieStream, attribute, ';')) {
            attribute = HttpUtils::trimString(attribute);

            if (isFirst) {
                // 第一个部分是name=value
                size_t equalPos = attribute.find('=');
                if (equalPos != std::string::npos) {
                    cookie.name = HttpUtils::trimString(attribute.substr(0, equalPos));
                    cookie.value = HttpUtils::trimString(attribute.substr(equalPos + 1));
                }
                isFirst = false;
            }
            else {
                // 解析其他属性
                size_t equalPos = attribute.find('=');
                if (equalPos != std::string::npos) {
                    std::string attrName = HttpUtils::toLower(HttpUtils::trimString(attribute.substr(0, equalPos)));
                    std::string attrValue = HttpUtils::trimString(attribute.substr(equalPos + 1));

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
                    std::string attrName = HttpUtils::toLower(attribute);
                    if (attrName == "secure") {
                        cookie.secure = true;
                    }
                    else if (attrName == "httponly") {
                        cookie.httpOnly = true;
                    }
                }
            }
        }

        if (!cookie.name.empty()) {
            cookies.push_back(cookie);
        }
    }

    return cookies;
}