/**
 * @desc:   Http响应类，HttpResponse.h
 * @author: ZhKeyes
 * @date:   2025/8/5
 */

#ifndef NET_HTTP_HTTPRESPONSE_H_
#define NET_HTTP_HTTPRESPONSE_H_

#include "HttpMessage.h"
#include <vector>

namespace ZhKeyesIM {
    namespace Net {
    namespace Http {

class NET_API HttpResponse : public HttpMessage
{
public:
    struct Cookie {
        std::string name;
        std::string value;
        std::string domain;
        std::string path;
        std::string expires;
        int maxAge = -1;
        bool secure = false;
        bool httpOnly = false;
        std::string sameSite;
        
        std::string toString() const;
    };
    
    HttpResponse();
    HttpResponse(HttpStatusCode statusCode);
    HttpResponse(HttpStatusCode statusCode, const std::string& body);
    virtual ~HttpResponse() = default;
    

    void setStatusCode(HttpStatusCode statusCode) { m_statusCode = statusCode; }
    HttpStatusCode getStatusCode() const { return m_statusCode; }
    int getStatusCodeValue() const { return static_cast<int>(m_statusCode); }
    
    void setReasonPhrase(const std::string& reasonPhrase) { m_reasonPhrase = reasonPhrase; }
    std::string getReasonPhrase() const;
    

    void setServer(const std::string& server);
    std::string getServer() const;
    
    void setDate(const std::string& date);
    std::string getDate() const;
    void setCurrentDate();
    
    void setLastModified(const std::string& lastModified);
    std::string getLastModified() const;
    void setLastModified(time_t timestamp);
    
    void setETag(const std::string& etag);
    std::string getETag() const;
    
    void setLocation(const std::string& location);
    std::string getLocation() const;
    
    void setCacheControl(const std::string& cacheControl);
    std::string getCacheControl() const;
    

    void setCookie(const Cookie& cookie);
    void setCookie(const std::string& name, const std::string& value);
    void setCookie(const std::string& name, const std::string& value, 
                   const std::string& path, const std::string& domain = "", 
                   int maxAge = -1, bool secure = false, bool httpOnly = false);
    
    std::vector<Cookie> getCookies() const;
    void clearCookies();
    

    bool isSuccessful() const { return HttpUtils::isSuccessStatusCode(m_statusCode); }
    bool isRedirection() const;
    bool isClientError() const { return HttpUtils::isClientErrorStatusCode(m_statusCode); }
    bool isServerError() const { return HttpUtils::isServerErrorStatusCode(m_statusCode); }
    

    void setTextResponse(const std::string& text);
    void setHtmlResponse(const std::string& html);
    void setJsonResponse(const std::string& json);
    void setFileResponse(const std::string& filePath, const std::string& fileName = "");
    void setRedirectResponse(const std::string& location, bool permanent = false);
    void setErrorResponse(HttpStatusCode statusCode, const std::string& message = "");
    

    std::string toString() const override;
    bool fromString(const std::string& data) override;
    

    void clear() override;
    
private:
    std::vector<Cookie> parseCookiesFromHeaders() const;
    
private:
    HttpStatusCode m_statusCode = HttpStatusCode::OK;
    std::string m_reasonPhrase;
};

    }}}

#endif //!NET_HTTP_HTTPRESPONSE_H_