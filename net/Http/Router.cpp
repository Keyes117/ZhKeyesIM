

#include "Router.h"
#include "Logger.h"

using namespace ZhKeyesIM::Net::Http;

void Router::addRoute(Http::HttpMethod method,
    const std::string& pattern,
    HandlerFunc handler)
{
    RouteEntry  entry;
    entry.method = method;
    entry.pattern = pattern;

    auto [regex, paramNames] = compilePattern(pattern);
    entry.regex = regex;
    entry.paramNames = paramNames;
    entry.handler = handler;

    m_routes.push_back(std::move(entry));
}

bool Router::dispatch(const HttpRequest& request, HttpResponse& response)
{
    //TODO:1. 执行全局中间件

    // 2. 查找匹配的路由
    std::map<std::string, std::string> params;
    for (const auto& route : m_routes)
    {
        if (route.method == request.getMethod() &&
            matchRoute(route, request.getPath(), params))
        {
            //TODO: 执行路由特定的中间件

            route.handler(request, response, params);
        }

        return true;
    }
}

std::pair<std::regex, std::vector<std::string>> Router::compilePattern(const std::string& pattern)
{
    std::vector<std::string> paramNames;
    std::string regexPattern = "^";

    size_t pos = 0;
    while (pos < pattern.length())
    {
        if (pattern[pos] == ':')
        {
            // 找到参数，如 :id
            size_t end = pattern.find('/', pos);
            if (end == std::string::npos)
                end = pattern.length();

            std::string paramName = pattern.substr(pos + 1, end - pos - 1);
            paramNames.push_back(paramName);

            // 匹配任意非斜杠字符
            regexPattern += "([^/]+)";
            pos = end;
        }
        else if (pattern[pos] == '*')
        {
            // 通配符，匹配任意字符
            regexPattern += "(.*)";
            pos++;
        }
        else
        {
            // 普通字符，需要转义正则特殊字符
            char c = pattern[pos];
            if (c == '.' || c == '+' || c == '?' || c == '^' ||
                c == '$' || c == '(' || c == ')' || c == '[' ||
                c == ']' || c == '{' || c == '}' || c == '|')
            {
                regexPattern += '\\';
            }
            regexPattern += c;
            pos++;
        }
    }

    regexPattern += "$";
    return { std::regex(regexPattern), paramNames };
}

bool Router::matchRoute(const RouteEntry& entry, const std::string& path, std::map<std::string, std::string>& params)
{
    std::smatch matches;
    if (std::regex_match(path, matches, entry.regex))
    {
        // 提取参数
        for (size_t i = 0; i < entry.paramNames.size() && i + 1 < matches.size(); ++i)
        {
            params[entry.paramNames[i]] = matches[i + 1].str();
        }
        return true;
    }
    return false;
}


