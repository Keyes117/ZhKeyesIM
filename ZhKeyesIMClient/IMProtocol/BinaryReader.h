#ifndef ZHKEYESIMCLIENT_IMPROTOCOL_BINARYWRITER_H_
#define ZHKEYESIMCLIENT_IMPROTOCOL_BINARYWRITER_H_

#include <string>
#include <cstring>
#include "util/ByteOrder.h"

namespace ZhKeyesIM {
namespace Protocol {

class BinaryReader {
public:
    BinaryReader(const std::string& data)
        : m_data(data), m_pos(0) {}
    
    BinaryReader(const char* data, size_t len)
        : m_data(data, len), m_pos(0) {}
    
    // 读取整数（自动转主机字节序）
    bool readInt8(int8_t& value) {
        if (m_pos + 1 > m_data.size()) return false;
        value = static_cast<int8_t>(m_data[m_pos]);
        m_pos += 1;
        return true;
    }
    
    bool readUInt8(uint8_t& value) {
        if (m_pos + 1 > m_data.size()) return false;
        value = static_cast<uint8_t>(m_data[m_pos]);
        m_pos += 1;
        return true;
    }
    
    bool readInt16(int16_t& value) {
        if (m_pos + 2 > m_data.size()) return false;
        uint16_t net;
        std::memcpy(&net, &m_data[m_pos], 2);
        uint16_t host = ZhKeyes::Utils::ByteOrder::networkToHost16(net);
        value = *reinterpret_cast<int16_t*>(&host);
        m_pos += 2;
        return true;
    }
    
    bool readUInt16(uint16_t& value) {
        if (m_pos + 2 > m_data.size()) return false;
        uint16_t net;
        std::memcpy(&net, &m_data[m_pos], 2);
        value = ZhKeyes::Utils::ByteOrder::networkToHost16(net);
        m_pos += 2;
        return true;
    }
    
    bool readInt32(int32_t& value) {
        if (m_pos + 4 > m_data.size()) return false;
        uint32_t net;
        std::memcpy(&net, &m_data[m_pos], 4);
        uint32_t host = ZhKeyes::Utils::ByteOrder::networkToHost32(net);
        value = *reinterpret_cast<int32_t*>(&host);
        m_pos += 4;
        return true;
    }
    
    bool readUInt32(uint32_t& value) {
        if (m_pos + 4 > m_data.size()) return false;
        uint32_t net;
        std::memcpy(&net, &m_data[m_pos], 4);
        value = ZhKeyes::Utils::ByteOrder::networkToHost32(net);
        m_pos += 4;
        return true;
    }
    
    bool readInt64(int64_t& value) {
        if (m_pos + 8 > m_data.size()) return false;
        uint64_t net;
        std::memcpy(&net, &m_data[m_pos], 8);
        uint64_t host = ZhKeyes::Utils::ByteOrder::networkToHost64(net);
        value = *reinterpret_cast<int64_t*>(&host);
        m_pos += 8;
        return true;
    }
    
    bool readUInt64(uint64_t& value) {
        if (m_pos + 8 > m_data.size()) return false;
        uint64_t net;
        std::memcpy(&net, &m_data[m_pos], 8);
        value = ZhKeyes::Utils::ByteOrder::networkToHost64(net);
        m_pos += 8;
        return true;
    }
    
    // 读取字符串（长度前缀）
    bool readString(std::string& str) {
        uint32_t len;
        if (!readUInt32(len)) return false;
        
        if (m_pos + len > m_data.size()) return false;
        
        str.assign(m_data.data() + m_pos, len);
        m_pos += len;
        return true;
    }
    
    // 读取原始字节
    bool readBytes(void* buf, size_t len) {
        if (m_pos + len > m_data.size()) return false;
        std::memcpy(buf, &m_data[m_pos], len);
        m_pos += len;
        return true;
    }
    
    // 工具方法
    size_t remaining() const {
        return m_data.size() - m_pos;
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
    
private:
    std::string m_data;
    size_t m_pos;
};

} // namespace Protocol
} // namespace ZhKeyesIM



#endif