#ifndef NETWORK_IMPROTOCOL_H_
#define NETWORK_IMPROTOCOL_H_

#include <cstdint>
#include <string>
/*
* 
*    +--------+--------+--------+--------+--------+--------+--------+--------+--------+...+--------+
*    | Magic  | Length | Version|  Type  | SeqId  |  Flags | Reserve|  Body  |  Body  |...|  Body  |
*    | 4 bytes| 4 bytes| 1 bytes| 2 bytes| 4 bytes| 2 bytes| 2 bytes| N bytes| N bytes|...| N bytes|
*    +--------+--------+--------+--------+--------+--------+--------+--------+--------+...+--------+
* 
* 字段说明：
 *   Magic (4字节): 魔数，用于识别协议，固定值 0x5A484B59 ("ZHKY")
 *   Length (4字节): 整个消息长度（包括头部），用于粘包/拆包处理，网络字节序
 *   Version (1字节): 协议版本号，当前版本为 1
 *   Type (2字节): 消息类型，网络字节序
 *   SeqId (4字节): 消息序列号，用于请求/响应匹配和去重，网络字节序
 *   Flags (1字节): 标志位，bit0=压缩标志，bit1=加密标志，bit2-7=保留
 *   Reserve (2字节): 预留字段，
 *   Body (N字节): 消息体，
 * 
 *   总头部长度：4+4+1+2+4+1+2 = 18 字节
*/

namespace ZhKeyesIM {
    namespace Protocol {

        constexpr uint32_t PROTOCOL_MAGIC = 0x5A484B59;  // ZHKY
        constexpr uint8_t PROTOCOL_VERSION = 1;          // 当前协议版本
        constexpr size_t HEADER_SIZE = 18;               // 头部长度
        constexpr size_t MAX_PACKET_SIZE = 1024 * 1024;  // 最大包1MB
        constexpr size_t MIN_PACKET_SIZE = HEADER_SIZE;  // 最小包（只有头部）

        // 标志位定义
        namespace Flags {
            constexpr uint8_t COMPRESSED = 0x01;  // bit 0: 压缩标志
            constexpr uint8_t ENCRYPTED = 0x02;   // bit 1: 加密标志
            // bit 2-7: 保留
        }

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
            uint32_t magic;      // 0x5A484B59
            uint32_t length;     // 整个消息长度（包括头部）
            uint8_t version;     // 协议版本
            uint16_t type;       // 消息类型
            uint32_t seqId;      // 消息序列号
            uint8_t flags;       // 标志位
            uint16_t reserve;    // 预留字段
            
            MessageHeader()
                : magic(PROTOCOL_MAGIC)
                , length(HEADER_SIZE)
                , version(PROTOCOL_VERSION)
                , type(0)
                , seqId(0)
                , flags(0)
                , reserve(0)
            {
            }
        };
#pragma pack(pop)


        // 静态断言，确保头部大小正确
        static_assert(sizeof(MessageHeader) == HEADER_SIZE,
            "MessageHeader size mismatch!");

        // 辅助函数：检查标志位
        inline bool isCompressed(uint8_t flags) {
            return (flags & Flags::COMPRESSED) != 0;
        }

        inline bool isEncrypted(uint8_t flags) {
            return (flags & Flags::ENCRYPTED) != 0;
        }

        inline void setCompressedFlag(uint8_t& flags, bool compressed) {
            if (compressed) {
                flags |= Flags::COMPRESSED;
            }
            else {
                flags &= ~Flags::COMPRESSED;
            }
        }

        inline void setEncryptedFlag(uint8_t& flags, bool encrypted) {
            if (encrypted) {
                flags |= Flags::ENCRYPTED;
            }
            else {
                flags &= ~Flags::ENCRYPTED;
            }
        }
    /*
    * Auth request
    *   {
    *       int32_t userId
    *       std::string token
    *   }
    */

    /*
    * Auth response  success = 1
    *   {
    *       bool success
    *       int32_t userId
    *       std::string token
    *       std::string name
    *       std::string email
    *       std::string nick
    *       std::string desc
    *       std::string sex
    *       std::string icon
    *       std::string back;
    *   }
    * 
    * Auth response  success = 1
    * {
    *       bool suceess
    *       int32_t userId
    *       std::string errorMsg
    * }
        
        
    */

    } //Protocol
}   //ZhKeyesIM
  


#endif