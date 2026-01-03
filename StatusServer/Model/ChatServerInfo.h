#ifndef StatusServer_MODEL_CHATSERVERINFO_H_
#define StatusServer_MODEL_CHATSERVERINFO_H_

class ChatServerInfo
{
public:
    ChatServerInfo() = default;
    ~ChatServerInfo() = default;



    std::string name;
    std::string host;
    uint16_t port;
    uint16_t error;

};

#endif