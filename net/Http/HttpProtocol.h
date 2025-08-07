/**
 * @desc:   Http常量设置，HttpCommon.h
 * @author: ZhKeyes
 * @date:   2025/8/5
 */
#ifndef NET_HTTP_HTTPPROTOCOL_H_
#define NET_HTTP_HTTPPROTOCOL_H_

#include <string>
#include <unordered_map>
#include <functional>
#include <system_error>
#include <cstdint>

namespace HttpConstants
{
    constexpr const char* CRLF = "\r\n";
    constexpr const char* HEARDER_SEPARATOR = ": ";
    constexpr const char* SPACE = " ";
    constexpr size_t MAX_HEADER_SIZE = 8192;
    constexpr size_t MAX_BODY_SIZE = 10485760;
    constexpr size_t MAX_URL_LENGTH = 2048;
    constexpr int DEFAULT_HTTP_PORT = 80;
    constexpr int DEFAULT_HTTPS_PORT = 443;
};

enum class HttpVersion
{
    HTTP_1_0,
    HTTP_1_1,
    HTTP_2_0,
    UNKNOWN
};

enum class HttpMethod
{
    GET,
    POST,
    PUT,
    DELETE,
    UNKNOWN
};

enum class HttpStatusCode : int 
{
   // 1xx Informational
   Continue = 100,
   SwitchingProtocols = 101,
   
   // 2xx Success
   OK = 200,
   Created = 201,
   Accepted = 202,
   NoContent = 204,
   PartialContent = 206,
   
   // 3xx Redirection
   MultipleChoices = 300,
   MovedPermanently = 301,
   Found = 302,
   SeeOther = 303,
   NotModified = 304,
   TemporaryRedirect = 307,
   PermanentRedirect = 308,
   
   // 4xx Client Error
   BadRequest = 400,
   Unauthorized = 401,
   PaymentRequired = 402,
   Forbidden = 403,
   NotFound = 404,
   MethodNotAllowed = 405,
   NotAcceptable = 406,
   RequestTimeout = 408,
   Conflict = 409,
   Gone = 410,
   LengthRequired = 411,
   PreconditionFailed = 412,
   PayloadTooLarge = 413,
   URITooLong = 414,
   UnsupportedMediaType = 415,
   RangeNotSatisfiable = 416,
   ExpectationFailed = 417,
   UnprocessableEntity = 422,
   TooManyRequests = 429,
   
   // 5xx Server Error
   InternalServerError = 500,
   NotImplemented = 501,
   BadGateway = 502,
   ServiceUnavailable = 503,
   GatewayTimeout = 504,
   HTTPVersionNotSupported = 505,
   InsufficientStorage = 507
};

enum class ParseState {
    PARSE_REQUEST_LINE,     // 解析请求行/状态行
    PARSE_HEADERS,          // 解析头部
    PARSE_BODY,             // 解析消息体
    PARSE_COMPLETE,         // 解析完成
    PARSE_ERROR             // 解析错误
};

// 解析结果枚举
enum class ParseResult {
    NEED_MORE_DATA,         // 需要更多数据
    PARSE_COMPLETE,         // 解析完成
    PARSE_ERROR             // 解析错误
};

// 会话模式枚举
enum class SessionMode {
    CLIENT,                 // 客户端模式
    SERVER,                 // 服务端模式
    UNKNOWN                 // 未知模式
};

// 会话状态枚举
enum class SessionState {
    IDLE,                   // 空闲状态
    CONNECTING,             // 正在连接
    CONNECTED,              // 已连接
    SENDING_REQUEST,        // 发送请求中
    WAITING_RESPONSE,       // 等待响应
    RECEIVING_RESPONSE,     // 接收响应中
    PROCESSING_REQUEST,     // 处理请求中
    SENDING_RESPONSE,       // 发送响应中
    KEEP_ALIVE,             // 保持连接
    CLOSING,                // 关闭连接中
    CLOSED,                 // 已关闭
    ERROR                   // 错误状态
};

// HTTP错误码枚举
enum class HttpError {
    NONE,                   // 无错误
    PARSE_ERROR,            // 解析错误
    CONNECTION_ERROR,       // 连接错误
    TIMEOUT_ERROR,          // 超时错误
    INVALID_REQUEST,        // 无效请求
    INVALID_RESPONSE,       // 无效响应
    HEADER_TOO_LARGE,       // 头部过大
    BODY_TOO_LARGE,         // 消息体过大
    UNSUPPORTED_VERSION,    // 不支持的版本
    UNKNOWN_ERROR           // 未知错误
};

class HttpRequest;
class HttpResponse;
class HttpMessage;

// 回调函数类型定义
// using RequestHandler = std::function<HttpResponse(const HttpRequest&)>;
// using ResponseCallback = std::function<void(const HttpResponse&, HttpError)>;
// using MessageCallback = std::function<void(const HttpMessage&)>;
// using ErrorCallback = std::function<void(HttpError, const std::string&)>;
// using CloseCallback = std::function<void()>;
// using ProgressCallback = std::function<void(size_t current, size_t total)>;

// HTTP方法字符串映射
const std::unordered_map<std::string, HttpMethod> METHOD_STRING_MAP = {
    {"GET", HttpMethod::GET},
    {"POST", HttpMethod::POST},
    {"PUT", HttpMethod::PUT},
    {"DELETE", HttpMethod::DELETE},
    // {"HEAD", HttpMethod::HEAD},
    // {"OPTIONS", HttpMethod::OPTIONS},
    // {"PATCH", HttpMethod::PATCH},
    // {"TRACE", HttpMethod::TRACE},
    // {"CONNECT", HttpMethod::CONNECT}
};

// HTTP方法到字符串映射
const std::unordered_map<HttpMethod, std::string> METHOD_TO_STRING_MAP = {
    {HttpMethod::GET, "GET"},
    {HttpMethod::POST, "POST"},
    {HttpMethod::PUT, "PUT"},
    {HttpMethod::DELETE, "DELETE"},
    // {HttpMethod::HEAD, "HEAD"},
    // {HttpMethod::OPTIONS, "OPTIONS"},
    // {HttpMethod::PATCH, "PATCH"},
    // {HttpMethod::TRACE, "TRACE"},
    // {HttpMethod::CONNECT, "CONNECT"},
    {HttpMethod::UNKNOWN, "UNKNOWN"}
};

// HTTP版本字符串映射
const std::unordered_map<std::string, HttpVersion> VERSION_STRING_MAP = {
    {"HTTP/1.0", HttpVersion::HTTP_1_0},
    {"HTTP/1.1", HttpVersion::HTTP_1_1},
    {"HTTP/2.0", HttpVersion::HTTP_2_0}
};

// HTTP版本到字符串映射
const std::unordered_map<HttpVersion, std::string> VERSION_TO_STRING_MAP = {
    {HttpVersion::HTTP_1_0, "HTTP/1.0"},
    {HttpVersion::HTTP_1_1, "HTTP/1.1"},
    {HttpVersion::HTTP_2_0, "HTTP/2.0"},
    {HttpVersion::UNKNOWN, "HTTP/1.1"}  // 默认版本
};

// HTTP状态码原因短语映射
const std::unordered_map<HttpStatusCode, std::string> STATUS_REASON_MAP = {
    // 1xx
    {HttpStatusCode::Continue, "Continue"},
    {HttpStatusCode::SwitchingProtocols, "Switching Protocols"},
    
    // 2xx
    {HttpStatusCode::OK, "OK"},
    {HttpStatusCode::Created, "Created"},
    {HttpStatusCode::Accepted, "Accepted"},
    {HttpStatusCode::NoContent, "No Content"},
    {HttpStatusCode::PartialContent, "Partial Content"},
    
    // 3xx
    {HttpStatusCode::MultipleChoices, "Multiple Choices"},
    {HttpStatusCode::MovedPermanently, "Moved Permanently"},
    {HttpStatusCode::Found, "Found"},
    {HttpStatusCode::SeeOther, "See Other"},
    {HttpStatusCode::NotModified, "Not Modified"},
    {HttpStatusCode::TemporaryRedirect, "Temporary Redirect"},
    {HttpStatusCode::PermanentRedirect, "Permanent Redirect"},
    
    // 4xx
    {HttpStatusCode::BadRequest, "Bad Request"},
    {HttpStatusCode::Unauthorized, "Unauthorized"},
    {HttpStatusCode::PaymentRequired, "Payment Required"},
    {HttpStatusCode::Forbidden, "Forbidden"},
    {HttpStatusCode::NotFound, "Not Found"},
    {HttpStatusCode::MethodNotAllowed, "Method Not Allowed"},
    {HttpStatusCode::NotAcceptable, "Not Acceptable"},
    {HttpStatusCode::RequestTimeout, "Request Timeout"},
    {HttpStatusCode::Conflict, "Conflict"},
    {HttpStatusCode::Gone, "Gone"},
    {HttpStatusCode::LengthRequired, "Length Required"},
    {HttpStatusCode::PreconditionFailed, "Precondition Failed"},
    {HttpStatusCode::PayloadTooLarge, "Payload Too Large"},
    {HttpStatusCode::URITooLong, "URI Too Long"},
    {HttpStatusCode::UnsupportedMediaType, "Unsupported Media Type"},
    {HttpStatusCode::RangeNotSatisfiable, "Range Not Satisfiable"},
    {HttpStatusCode::ExpectationFailed, "Expectation Failed"},
    {HttpStatusCode::UnprocessableEntity, "Unprocessable Entity"},
    {HttpStatusCode::TooManyRequests, "Too Many Requests"},
    
    // 5xx
    {HttpStatusCode::InternalServerError, "Internal Server Error"},
    {HttpStatusCode::NotImplemented, "Not Implemented"},
    {HttpStatusCode::BadGateway, "Bad Gateway"},
    {HttpStatusCode::ServiceUnavailable, "Service Unavailable"},
    {HttpStatusCode::GatewayTimeout, "Gateway Timeout"},
    {HttpStatusCode::HTTPVersionNotSupported, "HTTP Version Not Supported"},
    {HttpStatusCode::InsufficientStorage, "Insufficient Storage"}
};

// 常用MIME类型映射
const std::unordered_map<std::string, std::string> MIME_TYPE_MAP = {
    // 文本类型
    {".html", "text/html; charset=utf-8"},
    {".htm", "text/html; charset=utf-8"},
    {".css", "text/css; charset=utf-8"},
    {".js", "application/javascript; charset=utf-8"},
    {".json", "application/json; charset=utf-8"},
    {".xml", "application/xml; charset=utf-8"},
    {".txt", "text/plain; charset=utf-8"},
    
    // 图片类型
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".png", "image/png"},
    {".gif", "image/gif"},
    {".bmp", "image/bmp"},
    {".ico", "image/x-icon"},
    {".svg", "image/svg+xml"},
    
    // 音视频类型
    {".mp3", "audio/mpeg"},
    {".wav", "audio/wav"},
    {".mp4", "video/mp4"},
    {".avi", "video/x-msvideo"},
    
    // 文档类型
    {".pdf", "application/pdf"},
    {".doc", "application/msword"},
    {".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
    {".zip", "application/zip"},
    {".rar", "application/x-rar-compressed"}
};

// 工具函数声明
namespace HttpUtils {
    // 字符串工具
    std::string trim(const std::string& str);
    std::string toLower(const std::string& str);
    std::string toUpper(const std::string& str);
    
    // HTTP方法转换
    HttpMethod stringToMethod(const std::string& method);
    std::string methodToString(HttpMethod method);
    
    // HTTP版本转换
    HttpVersion stringToVersion(const std::string& version);
    std::string versionToString(HttpVersion version);
    
    // HTTP状态码转换
    std::string getReasonPhrase(HttpStatusCode statusCode);
    bool isSuccessStatusCode(HttpStatusCode statusCode);
    bool isClientErrorStatusCode(HttpStatusCode statusCode);
    bool isServerErrorStatusCode(HttpStatusCode statusCode);
    
    // MIME类型获取
    std::string getMimeType(const std::string& extension);
    
    // URL编码/解码
    std::string urlEncode(const std::string& str);
    std::string urlDecode(const std::string& str);
    
    // 时间格式化
    std::string formatHttpDate(time_t timestamp = 0);  // 0表示当前时间
    time_t parseHttpDate(const std::string& dateStr);
    
    // 头部名称规范化
    std::string normalizeHeaderName(const std::string& name);
}

 #endif //!NET_HTTP_HTTPPROTOCOL_H_