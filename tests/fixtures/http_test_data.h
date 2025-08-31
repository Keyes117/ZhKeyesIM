#ifndef HTTP_TEST_DATA_H
#define HTTP_TEST_DATA_H

#include <string>
#include <vector>
#include <tuple>

namespace HttpTestData {

// 有效的HTTP请求示例
struct ValidHttpRequest {
    std::string method;
    std::string path;
    std::string version;
    std::vector<std::pair<std::string, std::string>> headers;
    std::string body;
    std::string rawData;
};

// 有效的HTTP响应示例
struct ValidHttpResponse {
    std::string version;
    int statusCode;
    std::string statusText;
    std::vector<std::pair<std::string, std::string>> headers;
    std::string body;
    std::string rawData;
};

// 预定义的测试数据
extern const std::vector<ValidHttpRequest> VALID_REQUESTS;
extern const std::vector<ValidHttpResponse> VALID_RESPONSES;
extern const std::vector<std::string> INVALID_HTTP_DATA;

// 测试数据生成函数
std::string generateHttpRequest(const ValidHttpRequest& request);
std::string generateHttpResponse(const ValidHttpResponse& response);

} // namespace HttpTestData

#endif // HTTP_TEST_DATA_H