#ifndef ZHKEYESIMSERVER_IMSESSION_H_
#define ZHKEYESIMSERVER_IMSESSION_H_

#include <memory>


#include "net/Buffer.h"
#include "net/TCPConnection.h"
#include "IMProtocol/IMMessageSender.h"
#include "IMProtocol/IMMessage.h"

class IMServer;

class IMSession : public ZhKeyesIM::Protocol::IMMessageSender
{
public:
    using SessionID = uint32_t;

    IMSession(IMServer* server, std::shared_ptr<TCPConnection> spConn);
    ~IMSession() = default;

    SessionID getSessionId() { return m_sessionId; }

    virtual bool sendMessage(const ZhKeyesIM::Protocol::IMMessage& msg) override;

    void setUserInfo(uint32_t uid, const std::string& token);

private:
    static uint32_t generateID();

    void onRead(Buffer& buf);
    void onWrite();

private:
    IMServer* m_pServer;
    std::shared_ptr<TCPConnection> m_spConn;
    SessionID m_sessionId;

    uint32_t    m_uid;
    std::string m_token;

};


#endif