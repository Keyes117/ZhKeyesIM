#ifndef IMPROTOCOL_IMMESSAGEDISPATCHER_H_
#define IMPROTOCOL_IMMESSAGEDISPATCHER_H_

#include <memory>

#include "IMMessageHandler.h"
#include "IMProtocol.h"

class IMMessageDispatcher
{
public:
    IMMessageDispatcher();
    ~IMMessageDispatcher();


    void registerHandler(ZhKeyesIM::Protocol::MessageType type, std::weak_ptr<IMMessageHandler> handler);
};


#endif