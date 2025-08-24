/**
 * @desc:   Http解析器，HttpParser.h
 * @author: ZhKeyes
 * @date:   2025/8/5
 */

#ifndef NET_HTTP_HTTPPARSER_H_
#define NET_HTTP_HTTPPARSER_H_

#include "Buffer.h"
#include "HttpProtocol.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "net_export.h"
#include <memory>
#include <string>

namespace ZhKeyesIM {
    namespace Net {
        namespace Http {

            class NET_API HttpParser
            {
            public:
                HttpParser();
                explicit HttpParser(SessionMode mode);
                ~HttpParser() = default;

       
                void setSessionMode(SessionMode mode) { m_sessionMode = mode; }
                SessionMode getSessionMode() const { return m_sessionMode; }

                void setMaxHeaderSize(size_t maxSize) { m_maxHeaderSize = maxSize; }
                void setMaxBodySize(size_t maxSize) { m_maxBodySize = maxSize; }
                void setMaxUrlLength(size_t maxLength) { m_maxUrlLength = maxLength; }

   
                ParseResult feed(Buffer& buffer);  

      
                ParseState getState() const { return m_parseState; }
                bool isParseComplete() const { return m_parseState == ParseState::PARSE_COMPLETE; }
                bool hasError() const { return m_parseState == ParseState::PARSE_ERROR; }

      
                std::shared_ptr<HttpRequest> getRequest() const { return m_request; }
                std::shared_ptr<HttpResponse> getResponse() const { return m_response; }
                HttpError getHttpError() const { return m_lastError; }
                const std::string& getErrorMessage() const { return m_errorMessage; }

           
                void reset();
                void clear();

       
                size_t getParsedBytes() const { return m_parsedBytes; }
                size_t getCurrentHeaderSize() const { return m_currentHeaderSize; }
                size_t getCurrentBodySize() const { return m_currentBodySize; }

             
                static bool isValidHttpMethod(const std::string& method);
                static bool isValidHttpVersion(const std::string& version);
                static bool isValidStatusCode(int statusCode);

            private:
                ParseResult parseRequestLine(Buffer& buffer);
                ParseResult parseStatusLine(Buffer& buffer);
                ParseResult parseHeaders(Buffer& buffer);
                ParseResult parseBody(Buffer& buffer);
                ParseResult parseChunkedBody(Buffer& buffer);

                bool findLine(Buffer& buffer, std::string& line);
                bool findHeaderEnd(Buffer& buffer);
                size_t getContentLength() const;
                bool isChunkedEncoding() const;
                bool needMoreData(Buffer& buffer) const;


                void setError(HttpError error, const std::string& message);
                bool checkLimits();

            private:
                SessionMode m_sessionMode = SessionMode::SESSION_MODE_UNEXPECTED;
                ParseState m_parseState = ParseState::PARSE_REQUEST_LINE;
                HttpError m_lastError = HttpError::HTTPERROR_NONE;
                std::string m_errorMessage;

     
                std::shared_ptr<HttpRequest> m_request;
                std::shared_ptr<HttpResponse> m_response;

      
                size_t m_parsedBytes = 0;      
                size_t m_currentHeaderSize = 0;
                size_t m_currentBodySize = 0;  
                size_t m_expectedBodySize = 0; 

         
                size_t m_maxHeaderSize = HttpConstants::MAX_HEADER_SIZE;
                size_t m_maxBodySize = HttpConstants::MAX_BODY_SIZE;
                size_t m_maxUrlLength = HttpConstants::MAX_URL_LENGTH;

                bool m_isChunked = false;      
                size_t m_chunkSize = 0;        
                bool m_chunkSizeRead = false;  
                bool m_finalChunk = false;     
            };

        }
    }
}
#endif