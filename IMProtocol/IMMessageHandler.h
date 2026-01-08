#ifndef IMProtocol_IMMessageHandler_H_
#define IMProtocol_IMMessageHandler_H_

class IMMessageHandler
{
public:
    IMMessageHandler();
    virtual ~IMMessageHandler();

    virtual void handlerMessage(const Protocol::IMMessage& msg, IMSession* session) = 0;
};


#endif