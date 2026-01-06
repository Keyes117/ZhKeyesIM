#ifndef ZHKEYESIMCLIENT_IMPROTOCOL_BINARYREADER_H_
#define ZHKEYESIMCLIENT_IMPROTOCOL_BINARYREADER_H_

#include <string>
#include <cstring>
#include "ByteOrder.h"

namespace ZhKeyesIM {
namespace Protocol {

class BinaryReader {
public:
    BinaryReader(const std::string& data)
        : m_data(data), m_pos(0) {}
    
    BinaryReader(const char* data, size_t len)
        : m_data(data, len), m_pos(0) {}
    
    // ========== 固定长度读取（用于协议头部）==========
    bool readUInt8(uint8_t& value) {
        if (m_pos + 1 > m_data.size()) return false;
        value = static_cast<uint8_t>(m_data[m_pos]);
        m_pos += 1;
        return true;
    }
    
    bool readUInt16(uint16_t& value) {
        if (m_pos + 2 > m_data.size()) return false;
        uint16_t net;
        std::memcpy(&net, m_data.data() + m_pos, 2);
        value = ZhKeyes::Util::ByteOrder::networkToHost16(net);
        m_pos += 2;
        return true;
    }
    
    bool readUInt32(uint32_t& value) {
        if (m_pos + 4 > m_data.size()) return false;
        uint32_t net;
        std::memcpy(&net, m_data.data() + m_pos, 4);
        value = ZhKeyes::Util::ByteOrder::networkToHost32(net);
        m_pos += 4;
        return true;
    }
    
    bool readUInt64(uint64_t& value) {
        if (m_pos + 8 > m_data.size()) return false;
        uint64_t net;
        std::memcpy(&net, m_data.data() + m_pos, 8);
        value = ZhKeyes::Util::ByteOrder::networkToHost64(net);
        m_pos += 8;
        return true;
    }

    // ========== 可变长度读取（7位解码，用于Body内容）==========
    // 读取 1-5 字节的 uint32_t
    bool readUInt32Var(uint32_t& value) {
        value = 0;
        int bitCount = 0;
        uint8_t c;
        
        do {
            if (m_pos >= m_data.size()) return false;
            c = static_cast<uint8_t>(m_data[m_pos++]);
            uint32_t x = static_cast<uint32_t>(c & 0x7F);
            x <<= bitCount;
            value += x;
            bitCount += 7;
            
            // 防止溢出（最多5字节）
            if (bitCount > 35) {
                return false;  // 无效编码
            }
        } while (c & 0x80);
        
        return true;
    }

    // 读取 1-10 字节的 uint64_t
    bool readUInt64Var(uint64_t& value) {
        value = 0;
        int bitCount = 0;
        uint8_t c;
        
        do {
            if (m_pos >= m_data.size()) return false;
            c = static_cast<uint8_t>(m_data[m_pos++]);
            uint64_t x = static_cast<uint64_t>(c & 0x7F);
            x <<= bitCount;
            value += x;
            bitCount += 7;
            
            // 防止溢出（最多10字节）
            if (bitCount > 70) {
                return false;
            }
        } while (c & 0x80);
        
        return true;
    }

    // ========== 字符串读取（使用可变长度解码）==========
    bool readString(std::string& str) {
        uint32_t len;
        if (!readUInt32Var(len)) return false;
        
        if (m_pos + len > m_data.size()) return false;
        
        str.assign(m_data.data() + m_pos, len);
        m_pos += len;
        return true;
    }

    // 字符串读取（使用固定长度解码，用于兼容性）
    bool readStringFixed(std::string& str) {
        uint32_t len;
        if (!readUInt32(len)) return false;
        
        if (m_pos + len > m_data.size()) return false;
        
        str.assign(m_data.data() + m_pos, len);
        m_pos += len;
        return true;
    }

    // ========== 原始字节读取 ==========
    bool readBytes(void* buf, size_t len) {
        if (m_pos + len > m_data.size()) return false;
        std::memcpy(buf, m_data.data() + m_pos, len);
        m_pos += len;
        return true;
    }

    // ========== 工具方法 ==========
    bool skip(size_t bytes) {
        if (m_pos + bytes > m_data.size()) return false;
        m_pos += bytes;
        return true;
    }
    
    size_t remaining() const {
        return (m_pos < m_data.size()) ? (m_data.size() - m_pos) : 0;
    }
    
    bool hasData() const {
        return m_pos < m_data.size();
    }
    
    void reset() {
        m_pos = 0;
    }
    
    size_t position() const {
        return m_pos;
    }
    
    const std::string& getData() const {
        return m_data;
    }

private:
    std::string m_data;
    size_t m_pos;
};

} // namespace Protocol
} // namespace ZhKeyesIM

#endif