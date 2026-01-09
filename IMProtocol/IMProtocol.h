#ifndef NETWORK_IMPROTOCOL_H_
#define NETWORK_IMPROTOCOL_H_

#include <cstdint>
#include <string>
/*
* 
*    +--------+--------+--------+--------+--------+--------+--------+...+--------+
*    | Magic  | Length |  Type  | SeqId  |Reserve |  Body  |  Body  |...|  Body  |
*    | 4 bytes| 4 bytes| 2 bytes| 4 bytes| 2 bytes| N bytes| N bytes|...| N bytes|
*    +--------+--------+--------+--------+--------+--------+--------+...+--------+
* 
* 字段说明：
*   Magic (4字节): 魔数，用于识别协议，固定值 0x5A484B59 ("ZHKY")
*   Length (4字节): 整个消息长度（包括头部），用于粘包/拆包处理
*   Type (2字节): 消息类型
*   SeqId (4字节): 消息序列号，用于请求/响应匹配和去重
*   Reserve (2字节): 预留字段，可用于版本号、压缩标志等
*   Body (N字节): 消息体，
*/

namespace ZhKeyesIM {
    namespace Protocol {

        constexpr uint32_t PROTOCOL_MAGIC = 0x5A484B59; // ZHKY
        constexpr size_t HEADER_SIZE = 16;              // 头部长度
        constexpr size_t MAX_PACKET_SIZE = 1024 * 1024; // 最大包1MB

        enum class MessageType : uint16_t
        {
            //连接（1-99）
            AUTH_REQ = 1,           // 认证请求（连接后第一个消息）
            AUTH_RESP = 2,          // 认证响应
            HEARTBEAT_REQ = 3,      // 心跳请求
            HEARTBEAT_RESP = 4,     // 心跳响应
            DISCONNECT = 5,


            //单聊(100-199)

            //群聊(200-299)

            //好友(300-399)

            //用户状态(400-499)

            //错误(900)

            UNKNOWN = 9999
        };

#pragma pack(push,1)
        struct MessageHeader
        {
            uint32_t magic = PROTOCOL_MAGIC;
            uint32_t length = HEADER_SIZE;
            uint16_t type = 0;
            uint16_t seqId = 0;
            uint16_t reserve = 0;
            //uint16_t
        };
#pragma pack(pop)

    /*
    * Auth request
    *   {
    *       int32_t userId
    *       std::string token
    *   }
    */

    /*
    * Auth response
    *   {
    *       bool success
    *       int32_t userId
    *       std::string token
    *   }
    */

    } //Protocol
}   //ZhKeyesIM
  


#endif