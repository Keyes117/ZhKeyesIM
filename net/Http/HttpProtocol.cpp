/**
 * @desc:   Http辅助工具实现，HttpProtocol.cpp
 * @author: ZhKeyes
 * @date:   2025/8/5
 */
#include "HttpProtocol.h"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iterator>

#include "fmt/chrono.h"
#include "fmt/format.h"

using namespace ZhKeyesIM::Net::Http;


std::string HttpUtils::trimString(const std::string& str) {
    if (str.empty()) return str;

    size_t start = 0;
    size_t end = str.length() - 1;

    while (start <= end && std::isspace(str[start])) {
        start++;
    }


    while (end >= start && std::isspace(str[end])) {
        end--;
    }

    return str.substr(start, end - start + 1);
}

std::string HttpUtils::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string HttpUtils::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::toupper(c); });
    return result;
}



HttpMethod HttpUtils::stringToMethod(const std::string& method) {
    std::string upperMethod = HttpUtils::toUpper(method);
    auto it = METHOD_STRING_MAP.find(upperMethod);
    if (it != METHOD_STRING_MAP.end()) {
        return it->second;
    }
    return HttpMethod::UNKNOWN_METHOD;
}

std::string HttpUtils::methodToString(HttpMethod method) {
    auto it = METHOD_TO_STRING_MAP.find(method);
    if (it != METHOD_TO_STRING_MAP.end()) {
        return it->second;
    }
    return "UNKNOWN";
}



HttpVersion HttpUtils::stringToVersion(const std::string& version) {
    auto it = VERSION_STRING_MAP.find(version);
    if (it != VERSION_STRING_MAP.end()) {
        return it->second;
    }
    return HttpVersion::UNSPPORTED_VERSION;
}

std::string HttpUtils::versionToString(HttpVersion version) {
    auto it = VERSION_TO_STRING_MAP.find(version);
    if (it != VERSION_TO_STRING_MAP.end()) {
        return it->second;
    }
    return "HTTP/1.1";
}



std::string HttpUtils::getReasonPhrase(HttpStatusCode statusCode) {
    auto it = STATUS_REASON_MAP.find(statusCode);
    if (it != STATUS_REASON_MAP.end()) {
        return it->second;
    }
    return "Unknown Status";
}

bool HttpUtils::isSuccessStatusCode(HttpStatusCode statusCode) {
    int code = static_cast<int>(statusCode);
    return code >= 200 && code < 300;
}

bool HttpUtils::isClientErrorStatusCode(HttpStatusCode statusCode) {
    int code = static_cast<int>(statusCode);
    return code >= 400 && code < 500;
}

bool HttpUtils::isServerErrorStatusCode(HttpStatusCode statusCode) {
    int code = static_cast<int>(statusCode);
    return code >= 500 && code < 600;
}



std::string HttpUtils::getMimeType(const std::string& extension) {
    std::string lowerExt = HttpUtils::toLower(extension);


    if (!lowerExt.empty() && lowerExt[0] != '.') {
        lowerExt = "." + lowerExt;
    }

    auto it = MIME_TYPE_MAP.find(lowerExt);
    if (it != MIME_TYPE_MAP.end()) {
        return it->second;
    }
    return "application/octet-stream";
}



std::string HttpUtils::urlEncode(const std::string& str) {

    static constexpr char hex[] = "0123456789ABCDEF";

    fmt::memory_buffer buf;
    buf.reserve(str.size() * 3); // 预留足够空间

    for (unsigned char c : str) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            buf.push_back(static_cast<char>(c));
        }
        else {
            buf.push_back('%');
            buf.push_back(hex[c >> 4]);
            buf.push_back(hex[c & 0x0F]);
        }
    }

    return fmt::to_string(buf);
}

std::string HttpUtils::urlDecode(const std::string& str) {
    std::string decoded;
    decoded.reserve(str.length());

    for (size_t i = 0; i < str.length(); ++i)
    {
        if (str[i] == '%' && i + 2 < str.length())
        {
            std::string hex = str.substr(i + 1, 2);
            char* end;
            long value = std::strtol(hex.c_str(), &end, 16);
            if (end == hex.c_str() + 2) {
                decoded += static_cast<char>(value);
                i += 2;
            }
            else {
                decoded += str[i];
            }
        }
        else if (str[i] == '+') {
            decoded += ' ';
        }
        else {
            decoded += str[i];
        }
    }

    return decoded;
}

// ============== 时间格式化函数 ==============

std::string HttpUtils::formatHttpDate(time_t timestamp)
{
    // 使用 chrono 获取时间点
    std::chrono::system_clock::time_point tp;

    if (timestamp == 0) {
        tp = std::chrono::system_clock::now();
    }
    else {
        tp = std::chrono::system_clock::from_time_t(timestamp);
    }

    // 转换为 time_t
    auto time = std::chrono::system_clock::to_time_t(tp);
    std::tm* gmt = std::gmtime(&time);
    if (!gmt) {
        return "";
    }

    // 使用 put_time 格式化
    std::ostringstream oss;
    oss << std::put_time(gmt, "%a, %d %b %Y %H:%M:%S GMT");
    return oss.str();
}

time_t HttpUtils::parseHttpDate(const std::string& dateStr)
{
    std::tm tm = {};

    // 尝试几种常见的HTTP日期格式
    // RFC 822格式: Sun, 06 Nov 1994 08:49:37 GMT
    std::string_view sv(dateStr);

    // 这里可以使用更高效的手动解析，但为了简化先保留原逻辑
    // 实际项目中建议用专门的日期解析库如 date/chrono
    std::istringstream ss(dateStr);
    ss >> std::get_time(&tm, "%a, %d %b %Y %H:%M:%S");

    if (ss.fail()) {
        ss.clear();
        ss.str(dateStr);
        // RFC 850格式: Sunday, 06-Nov-94 08:49:37 GMT
        ss >> std::get_time(&tm, "%A, %d-%b-%y %H:%M:%S");
    }

    if (ss.fail()) {
        ss.clear();
        ss.str(dateStr);
        // ANSI C asctime()格式: Sun Nov  6 08:49:37 1994
        ss >> std::get_time(&tm, "%a %b %d %H:%M:%S %Y");
    }

    if (ss.fail()) {
        return 0; // 解析失败
    }

    return std::mktime(&tm);
}

// ============== 头部名称规范化函数 ==============

std::string HttpUtils::normalizeHeaderName(const std::string& name) {
    if (name.empty()) {
        return name;
    }

    std::string normalized;
    normalized.reserve(name.length());

    bool capitalizeNext = true;
    for (char c : name) {
        if (c == '-') {
            normalized += c;
            capitalizeNext = true;
        }
        else if (capitalizeNext) {
            normalized += std::toupper(c);
            capitalizeNext = false;
        }
        else {
            normalized += std::tolower(c);
        }
    }

    return normalized;
}

// namespace HttpUtils