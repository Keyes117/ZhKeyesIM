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
    std::ostringstream encoded;
    encoded.fill('0');
    encoded << std::hex;

    for (char c : str) {
        
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded << c;
        }
        else {
           
            encoded << std::uppercase;
            encoded << '%' << std::setw(2) << static_cast<int>(static_cast<unsigned char>(c));
            encoded << std::nouppercase;
        }
    }

    return encoded.str();
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

std::string HttpUtils::formatHttpDate(time_t timestamp) {
    if (timestamp == 0) {
        timestamp = std::time(nullptr);
    }

    std::tm* gmt = std::gmtime(&timestamp);
    if (!gmt) {
        return "";
    }

    std::ostringstream oss;
    oss << std::put_time(gmt, "%a, %d %b %Y %H:%M:%S GMT");
    return oss.str();
}

time_t HttpUtils::parseHttpDate(const std::string& dateStr) {
    std::tm tm = {};
    std::istringstream ss(dateStr);

    // 尝试几种常见的HTTP日期格式
    // RFC 822格式: Sun, 06 Nov 1994 08:49:37 GMT
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