/**
 * @desc:   Http协议，HttpProtocol.h
 * @author: ZhKeyes
 * @date:   2025/8/5
 */
#ifndef NET_HTTP_HTTPPROTOCOL_H_
#define NET_HTTP_HTTPPROTOCOL_H_

#include <cstdint>
#include <ctime>
#include <functional>
#include <string>
#include <system_error>
#include <unordered_map>

namespace ZhKeyesIM {
    namespace Net {
        namespace Http {


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

            enum class HttpVersion : int
            {
                HTTP_1_0 = 0,
                HTTP_1_1,
                HTTP_2_0,
                UNSPPORTED_VERSION
            };

            enum class HttpMethod : int
            {
                GET = 0,
                POST,
                PUT,
                DEL,
                UNKNOWN_METHOD
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

            enum class ParseState : int {
                PARSE_REQUEST_LINE = 0,     
                PARSE_HEADERS,         
                PARSE_BODY,            
                PARSE_COMPLETE,        
                PARSE_ERROR            
            };


            enum class ParseResult : int {
                PARSE_RESULT_NEED_MORE_DATA = 0,     
                PARSE_RESULT_PARSING,
                PARSE_RESULT_COMPLETE,       
                PARSE_RESULT_ERROR           
            };

   
            enum class SessionMode {
                SESSION_MODE_CLIENT = 0,          
                SESSION_MODE_SERVER,              
                SESSION_MODE_UNEXPECTED           
            };

    
            enum class SessionState : int {
                STATE_IDLE = 0,
                STATE_CONNECTING,
                STATE_CONNECTED,
                STATE_SENDING_REQUEST,
                STATE_WAITING_RESPONSE,
                STATE_RECEIVING_RESPONSE,
                STATE_PROCESSING_REQUEST,
                STATE_SENDING_RESPONSE,
                STATE_KEEP_ALIVE,
                STATE_CLOSING,
                STATE_CLOSED,
                STATE_ERROR
            };


            enum class HttpError : int {
                HTTPERROR_NONE = 0,             
                HTTPERROR_PARSE_ERROR,          
                HTTPERROR_CONNECTION_ERROR,     
                HTTPERROR_TIMEOUT_ERROR,        
                HTTPERROR_INVALID_REQUEST,      
                HTTPERROR_INVALID_RESPONSE,     
                HTTPERROR_HEADER_TOO_LARGE,     
                HTTPERROR_BODY_TOO_LARGE,       
                HTTPERROR_UNSUPPORTED_VERSION,  
                HTTPERROR_UNKNOWN_ERROR         
            };


            const std::unordered_map<std::string, HttpMethod> METHOD_STRING_MAP = {
                {"GET", HttpMethod::GET},
                {"POST", HttpMethod::POST},
                {"PUT", HttpMethod::PUT},
                {"DELETE", HttpMethod::DEL}
            };

            const std::unordered_map<HttpMethod, std::string> METHOD_TO_STRING_MAP = {
                {HttpMethod::GET, "GET"},
                {HttpMethod::POST, "POST"},
                {HttpMethod::PUT, "PUT"},
                {HttpMethod::DEL, "DELETE"},
                {HttpMethod::UNKNOWN_METHOD, "UNKNOWN"}
            };


            const std::unordered_map<std::string, HttpVersion> VERSION_STRING_MAP = {
                {"HTTP/1.0", HttpVersion::HTTP_1_0},
                {"HTTP/1.1", HttpVersion::HTTP_1_1},
                {"HTTP/2.0", HttpVersion::HTTP_2_0}
            };

    
            const std::unordered_map<HttpVersion, std::string> VERSION_TO_STRING_MAP = {
                {HttpVersion::HTTP_1_0, "HTTP/1.0"},
                {HttpVersion::HTTP_1_1, "HTTP/1.1"},
                {HttpVersion::HTTP_2_0, "HTTP/2.0"},
                {HttpVersion::UNSPPORTED_VERSION, "HTTP/1.1"}  // 默认版本
            };


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


            const std::unordered_map<std::string, std::string> MIME_TYPE_MAP = {
           
                {".html", "text/html; charset=utf-8"},
                {".htm", "text/html; charset=utf-8"},
                {".css", "text/css; charset=utf-8"},
                {".js", "application/javascript; charset=utf-8"},
                {".json", "application/json; charset=utf-8"},
                {".xml", "application/xml; charset=utf-8"},
                {".txt", "text/plain; charset=utf-8"},

        
                {".jpg", "image/jpeg"},
                {".jpeg", "image/jpeg"},
                {".png", "image/png"},
                {".gif", "image/gif"},
                {".bmp", "image/bmp"},
                {".ico", "image/x-icon"},
                {".svg", "image/svg+xml"},

         
                {".mp3", "audio/mpeg"},
                {".wav", "audio/wav"},
                {".mp4", "video/mp4"},
                {".avi", "video/x-msvideo"},

       
                {".pdf", "application/pdf"},
                {".doc", "application/msword"},
                {".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
                {".zip", "application/zip"},
                {".rar", "application/x-rar-compressed"}
            };


            namespace HttpUtils {
         
                std::string trimString(const std::string& str);
                std::string toLower(const std::string& str);
                std::string toUpper(const std::string& str);

       
                HttpMethod stringToMethod(const std::string& method);
                std::string methodToString(HttpMethod method);

           
                HttpVersion stringToVersion(const std::string& version);
                std::string versionToString(HttpVersion version);


                std::string getReasonPhrase(HttpStatusCode statusCode);
                bool isSuccessStatusCode(HttpStatusCode statusCode);
                bool isClientErrorStatusCode(HttpStatusCode statusCode);
                bool isServerErrorStatusCode(HttpStatusCode statusCode);


                std::string getMimeType(const std::string& extension);


                std::string urlEncode(const std::string& str);
                std::string urlDecode(const std::string& str);


                std::string formatHttpDate(time_t timestamp = 0); 
                time_t parseHttpDate(const std::string& dateStr);

     
                std::string normalizeHeaderName(const std::string& name);
            };

        }
    }
}
#endif //!NET_HTTP_HTTPPROTOCOL_H_