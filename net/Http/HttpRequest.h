/**
 * @desc:   Http请求类，HttpRequest.h
 * @author: ZhKeyes
 * @date:   2025/8/5
 */

#ifndef NET_HTTP_HTTPREQUEST_H_
#define NET_HTTP_HTTPREQUEST_H_

#include "HttpMessage.h"
namespace ZhKeyesIM {
    namespace Net {
    namespace Http {

class NET_API HttpRequest : public HttpMessage
{
public:
    using QueryParams = std::unordered_map<std::string, std::string>;
    using formData = std::unordered_map<std::string, std::string>;

    HttpRequest();
    HttpRequest(HttpMethod method, const std::string& url);
    virtual ~HttpRequest() = default;

    void setMethod(HttpMethod method) { m_method = method; }
    HttpMethod getMethod() const {return m_method;}
    std::string getMethodString() const {return HttpUtils::methodToString(m_method);}

    void setUrl(const std::string& url);
    const std::string& getUrl() const {return m_url;}

    void setPath(const std::string& path){m_path = path;}
    const std::string& getPath() const {return m_path;}

    void setQuery(const std::string& query);
    const std::string& getQuery() const {return  m_query;}

    void setQueryParam(const std::string& name, const std::string& value);
    std::string getQueryParam(const std::string&name ) const;
    bool hasQueryParam(const std::string& name) const;
    void removeQueryParam(const std::string& name);
      void clearQueryParams() { m_queryParams.clear(); updateQueryString(); }
    
    const QueryParams& getQueryParams() const { return m_queryParams; }
    
    // ============== 常用头部便捷方法 ==============
    void setHost(const std::string& host);
    std::string getHost() const;
    
    void setUserAgent(const std::string& userAgent);
    std::string getUserAgent() const;
    
    void setReferer(const std::string& referer);
    std::string getReferer() const;
    
    void setAccept(const std::string& accept);
    std::string getAccept() const;
    
    void setAuthorization(const std::string& auth);
    std::string getAuthorization() const;
    
    // ============== Cookie操作 ==============
    void setCookie(const std::string& name, const std::string& value);
    std::string getCookie(const std::string& name) const;
    bool hasCookie(const std::string& name) const;
    std::string getCookieHeader() const;
    
    // ============== 表单数据处理 ==============
    void setFormData(const std::unordered_map<std::string, std::string>& formData);
    void addFormData(const std::string& name, const std::string& value);
    std::string getFormDataString() const;
    
    // ============== JSON数据处理 ==============
    void setJsonBody(const std::string& json);
    
    // ============== 序列化和解析 ==============
    std::string toString() const override;
    bool fromString(const std::string& data) override;
    
    // ============== 工具方法 ==============
    virtual void clear() override;
    bool isFormRequest() const;
    bool isJsonRequest() const;
    bool isMultipartRequest() const;
    
private:
    void parseUrl(const std::string& url);
    void updateQueryString();
    void parseQueryString(const std::string& queryString);
    std::unordered_map<std::string, std::string> parseCookies() const;
    

private:
    HttpMethod m_method = HttpMethod::GET;
    std::string m_url;
    std::string m_path;
    std::string m_query;
    QueryParams m_queryParams;
    formData   m_formData;
    

};
    }}}


#endif