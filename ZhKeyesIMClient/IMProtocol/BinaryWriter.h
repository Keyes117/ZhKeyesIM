#ifndef ZHKEYESIMCLIENT_IMPROTOCOL_BINARYWRITER_H_
#define ZHKEYESIMCLIENT_IMPROTOCOL_BINARYWRITER_H_

#include <string>
#include <vector>
#include <cstring>

#include "ByteOrder.h"

class BinaryWriter
{
public:
BinaryWriter() {
    m_buffer.reserve(256);
}

// 写入整数（自动转网络字节序）
void writeInt8(int8_t value) {
    m_buffer.push_back(static_cast<uint8_t>(value));
}

void writeUInt8(uint8_t value) {
    m_buffer.push_back(value);
}

void writeInt16(int16_t value) {
    uint16_t net = ZhKeyes::Utils::ByteOrder::hostToNetwork16(
        *reinterpret_cast<uint16_t*>(&value)
    );
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&net);
    m_buffer.insert(m_buffer.end(), ptr, ptr + 2);
}

void writeUInt16(uint16_t value) {
    uint16_t net = ZhKeyes::Utils::ByteOrder::hostToNetwork16(value);
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&net);
    m_buffer.insert(m_buffer.end(), ptr, ptr + 2);
}

void writeInt32(int32_t value) {
    uint32_t net = ZhKeyes::Utils::ByteOrder::hostToNetwork32(
        *reinterpret_cast<uint32_t*>(&value)
    );
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&net);
    m_buffer.insert(m_buffer.end(), ptr, ptr + 4);
}

void writeUInt32(uint32_t value) {
    uint32_t net = ZhKeyes::Utils::ByteOrder::hostToNetwork32(value);
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&net);
    m_buffer.insert(m_buffer.end(), ptr, ptr + 4);
}

void writeInt64(int64_t value) {
    uint64_t net = ZhKeyes::Utils::ByteOrder::hostToNetwork64(
        *reinterpret_cast<uint64_t*>(&value)
    );
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&net);
    m_buffer.insert(m_buffer.end(), ptr, ptr + 8);
}

void writeUInt64(uint64_t value) {
    uint64_t net = ZhKeyes::Utils::ByteOrder::hostToNetwork64(value);
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&net);
    m_buffer.insert(m_buffer.end(), ptr, ptr + 8);
}

// 写入字符串（长度前缀）
void writeString(const std::string& str) {
    writeUInt32(static_cast<uint32_t>(str.size()));
    m_buffer.insert(m_buffer.end(), str.begin(), str.end());
}

// 写入原始字节
void writeBytes(const void* data, size_t len) {
    const uint8_t* ptr = static_cast<const uint8_t*>(data);
    m_buffer.insert(m_buffer.end(), ptr, ptr + len);
}

// 获取结果
std::string getData() const {
    return std::string(m_buffer.begin(), m_buffer.end());
}

size_t size() const {
    return m_buffer.size();
}

void clear() {
    m_buffer.clear();
}
private:
    std::vector<uint8_t> m_buffer;
};



#endif