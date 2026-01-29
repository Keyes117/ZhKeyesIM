#ifndef IMPROTOCOL_IMMESSAGEDISPATCHER_H_
#define IMPROTOCOL_IMMESSAGEDISPATCHER_H_

#include <functional>
#include <memory>
#include <unordered_map>

#include "IMProtocol.h"
#include "IMMessageSender.h"
#include <mutex>


namespace ZhKeyesIM {
    namespace Protocol {

        class IMMessageDispatcher
        {
        public:

            using MessageHandler = std::function<void(std::shared_ptr<IMMessage>, std::shared_ptr<IMMessageSender>)>;

            IMMessageDispatcher() = default;
            ~IMMessageDispatcher() = default;


            bool registerHandler(MessageType type, MessageHandler handler);

            bool hasRegistered(MessageType type);

            bool updateHandler(MessageType type, MessageHandler handler);

            bool unregisterHandler(MessageType type);

            bool dispatch(std::shared_ptr<IMMessage> msg, std::shared_ptr<IMMessageSender> sender);

        private:
            std::mutex  m_handlerMutex;
            std::unordered_map<MessageType, MessageHandler> m_messageHandlers;
        };
    }
}



#endif