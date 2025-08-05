/**
 * @desc:   Http消息基类，HttpMessage.h
 * @author: ZhKeyes
 * @date:   2025/8/5
 */
#ifndef NET_HTTP_HTTPMESSAGE_H_
#define NET_HTTP_HTTPMESSAGE_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "HttpProtocol.h"
#include "net_export.h"

class NET_API HttpMessage
{
public:
    using HeaderMap = std::unordered_map<std::string, std::string>;
    HttpMessage();
    virtual ~HttpMessage() = default;

    // ============== Version ===============
    void setVersion(HttpVersion version){m_version = version;}
    HttpVersion getVersion() {return m_version;}
    std::string getVersionString() const {return HttpUtils::versionToString(m_version);}

    // ============== Header ===============
    void setHeader(const std::string&name, const std::string value);
    std::string getHeader(const std::string& name ) const;
    bool hasHeader(const std::string& name ) const;
    void removeHeader(const std::string& name);
    void clearHeaders() { m_headers.clear();}

    const HeaderMap& getHeaders() const {return m_headers;}
    HeaderMap& getHeaders() {return m_headers;}

    // 常用头部的便捷方法
    void setContentType(const std::string& contentType);
    std::string getContentType() const;
    
    void setContentLength(size_t length);
    size_t getContentLength() const;
    
    void setConnection(const std::string& connection);
    std::string getConnection() const;
    bool isKeepAlive() const;
    
    // ============== 消息体操作 ==============
    void setBody(const std::string& body) { m_body = body; }
    void setBody(std::string&& body) { m_body = std::move(body); }
    void appendBody(const std::string& data) { m_body += data; }
    void appendBody(const char* data, size_t length) { m_body.append(data, length); }
    
    const std::string& getBody() const { return m_body; }
    std::string& getBody() { return m_body; }
    size_t getBodySize() const { return m_body.size(); }
    void clearBody() { m_body.clear(); }
    
    // ============== 序列化和解析 ==============
    virtual std::string toString() const = 0;
    virtual bool fromString(const std::string& data) = 0;
    
    // ============== 工具方法 ==============
    virtual void clear();
    bool empty() const;
    
protected:
    // 获取规范化的头部名称
    std::string normalizeHeaderName(const std::string& name) const;
    
    // 序列化头部为字符串
    std::string headersToString() const;
    
    // 从字符串解析头部
    bool parseHeaders(const std::string& headerData);

private:
    HttpVersion m_version = HttpVersion::HTTP_1_1;
    HeaderMap m_headers;
    std::string m_body;  
};

#endif