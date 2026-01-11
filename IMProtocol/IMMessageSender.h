#ifndef IMPROTOCOL_IMMESSAGESENDER_H_
#define IMPROTOCOL_IMMESSAGESENDER_H_

#include <memory>

#include "IMMessage.h"

namespace ZhKeyesIM {
    namespace Protocol
    {
        class IMMessageSender : public std::enable_shared_from_this<IMMessageSender>
        {
        public:
            IMMessageSender() = default;
            virtual ~IMMessageSender() = default;

            virtual bool sendMessage(const IMMessage& msg) = 0;
        };

    }
}




#endif