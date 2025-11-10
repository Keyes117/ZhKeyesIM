#ifndef GATESERVER_ROUTER_H_
#define GATESERVER_ROUTER_H_

#include <functional>
#include <map>
#include <regex>
#include <string>
#include <vector>

#include "net_export.h"
#include "Http/HttpRequest.h"
#include "Http/HttpResponse.h"

namespace ZhKeyesIM {
    namespace Net {
        namespace Http {

//TODO: 未来增加中间件
class NET_API Router final
{
public:
    using HandlerFunc = std::function<void(const HttpRequest&,
        HttpResponse&, const std::map<std::string, std::string>&)>;


    Router() = default;
    ~Router() = default;

    void addRoute(HttpMethod method, const std::string& pattern,
        HandlerFunc handler);

    bool dispatch(const HttpRequest& request, HttpResponse& response);

private:
    struct RouteEntry
    {
        Http::HttpMethod method;
        std::string pattern;         //原始模式 /user/:id
        std::regex regex;           //编译之后的正则表达式
        std::vector<std::string> paramNames;  // 参数名列表
        HandlerFunc handler;
    };

    std::vector<RouteEntry> m_routes;
    // 将路由模式转换为正则表达式
    std::pair<std::regex, std::vector<std::string>> compilePattern(const std::string& pattern);

    // 匹配路由并提取参数
    bool matchRoute(const RouteEntry& entry,
        const std::string& path,
        std::map<std::string, std::string>& params);
};
        }
    }
}
#endif