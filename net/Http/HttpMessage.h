/**
 * @desc:   Http消息基类，HttpMessage.h
 * @author: ZhKeyes
 * @date:   2025/8/5
 */
#ifndef NET_HTTP_HTTPMESSAGE_H_
#define NET_HTTP_HTTPMESSAGE_H_

#include "net_export.h"

class NET_API HttpMessage
{
public:
    virtual ~HttpMessage() = default;
}

#endif