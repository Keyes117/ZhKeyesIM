#ifndef IMPROTOCOL_BINARYWRITER_H_
#define IMPROTOCOL_BINARYWRITER_H_

#include <string>
#include <vector>
#include <cstring>
#include <stdexcept>
#include "ByteOrder.h"

namespace ZhKeyesIM {
namespace Protocol {

class BinaryWriter {
public:
    static constexpr size_t MAX_BUFFER_SIZE = 10 * 1024 * 1024; // 10MB
    
    BinaryWriter() {
        m_buffer.reserve(256);
    }
    
    explicit BinaryWriter(size_t initialCapacity) {
        m_buffer.reserve(initialCapacity);
    }

    // ========== 固定长度编码（用于协议头部）==========
    void writeUInt8(uint8_t value) {
        checkCapacity(1);
        m_buffer.push_back(value);
    }

    void writeUInt16(uint16_t value) {
        checkCapacity(2);
        uint16_t net = ZhKeyes::Util::ByteOrder::hostToNetwork16(value);
        size_t oldSize = m_buffer.size();
        m_buffer.resize(oldSize + 2);
        std::memcpy(m_buffer.data() + oldSize, &net, 2);
    }

    void writeUInt32(uint32_t value) {
        checkCapacity(4);
        uint32_t net = ZhKeyes::Util::ByteOrder::hostToNetwork32(value);
        size_t oldSize = m_buffer.size();
        m_buffer.resize(oldSize + 4);
        std::memcpy(m_buffer.data() + oldSize, &net, 4);
    }

    void writeUInt64(uint64_t value) {
        checkCapacity(8);
        uint64_t net = ZhKeyes::Util::ByteOrder::hostToNetwork64(value);
        size_t oldSize = m_buffer.size();
        m_buffer.resize(oldSize + 8);
        std::memcpy(m_buffer.data() + oldSize, &net, 8);
    }

    // ========== 可变长度编码（7位编码，用于Body内容）==========
    // 将 uint32_t 编码为 1-5 字节
    void writeUInt32Var(uint32_t value) {
        do {
            uint8_t c = static_cast<uint8_t>(value & 0x7F);
            value >>= 7;
            if (value != 0) {
                c |= 0x80;  // 设置最高位表示还有后续字节
            }
            checkCapacity(1);
            m_buffer.push_back(c);
        } while (value != 0);
    }

    // 将 uint64_t 编码为 1-10 字节
    void writeUInt64Var(uint64_t value) {
        do {
            uint8_t c = static_cast<uint8_t>(value & 0x7F);
            value >>= 7;
            if (value != 0) {
                c |= 0x80;
            }
            checkCapacity(1);
            m_buffer.push_back(c);
        } while (value != 0);
    }

    // ========== 字符串写入（使用可变长度编码长度）==========
    void writeString(const std::string& str) {
        size_t len = str.size();
        if (len > UINT32_MAX) {
            throw std::runtime_error("String too long");
        }
        // 使用可变长度编码字符串长度（节省空间）
        writeUInt32Var(static_cast<uint32_t>(len));
        if (len > 0) {
            checkCapacity(len);
            size_t oldSize = m_buffer.size();
            m_buffer.resize(oldSize + len);
            std::memcpy(m_buffer.data() + oldSize, str.data(), len);
        }
    }

    // 字符串写入（使用固定长度编码，用于兼容性）
    void writeStringFixed(const std::string& str) {
        size_t len = str.size();
        if (len > UINT32_MAX) {
            throw std::runtime_error("String too long");
        }
        writeUInt32(static_cast<uint32_t>(len));
        if (len > 0) {
            checkCapacity(len);
            size_t oldSize = m_buffer.size();
            m_buffer.resize(oldSize + len);
            std::memcpy(m_buffer.data() + oldSize, str.data(), len);
        }
    }

    // ========== 原始字节写入 ==========
    void writeBytes(const void* data, size_t len) {
        if (len == 0) return;
        checkCapacity(len);
        const uint8_t* ptr = static_cast<const uint8_t*>(data);
        size_t oldSize = m_buffer.size();
        m_buffer.resize(oldSize + len);
        std::memcpy(m_buffer.data() + oldSize, ptr, len);
    }

    // ========== 工具方法 ==========
    std::string getData() const {
        return std::string(m_buffer.begin(), m_buffer.end());
    }
    
    const uint8_t* data() const {
        return m_buffer.data();
    }
    
    size_t size() const {
        return m_buffer.size();
    }
    
    void clear() {
        m_buffer.clear();
    }
    
    void reserve(size_t capacity) {
        m_buffer.reserve(capacity);
    }

private:
    void checkCapacity(size_t additionalBytes) {
        if (m_buffer.size() + additionalBytes > MAX_BUFFER_SIZE) {
            throw std::runtime_error("BinaryWriter: Buffer overflow");
        }
    }
    
    std::vector<uint8_t> m_buffer;
};

} // namespace Protocol
} // namespace ZhKeyesIM

#endif