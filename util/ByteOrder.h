#ifndef UTIL_ByteOrder_H_
#define UTIL_ByteOrder_H_

#include <cstdint>
namespace ZhKeyes{
    namespace Util{

class ByteOrder
{
public:
    static inline bool isLittleEndian() {
        constexpr uint16_t test = 0x0001;
        return *reinterpret_cast<const uint8_t*>(&test) == 0x01;
    }
    
    static inline bool isBigEndian() {
        return !isLittleEndian();
    }
    
    // ========== 16位转换 ==========
    static inline uint16_t swap16(uint16_t value) {
        return (value >> 8) | (value << 8);
    }
    
    static inline uint16_t hostToNetwork16(uint16_t value) {
        return isLittleEndian() ? swap16(value) : value;
    }
    
    static inline uint16_t networkToHost16(uint16_t value) {
        return isLittleEndian() ? swap16(value) : value;
    }
    
    // ========== 32位转换 ==========
    static inline uint32_t swap32(uint32_t value) {
        return ((value & 0x000000FFU) << 24) |
               ((value & 0x0000FF00U) << 8)  |
               ((value & 0x00FF0000U) >> 8)  |
               ((value & 0xFF000000U) >> 24);
    }
    
    static inline uint32_t hostToNetwork32(uint32_t value) {
        return isLittleEndian() ? swap32(value) : value;
    }
    
    static inline uint32_t networkToHost32(uint32_t value) {
        return isLittleEndian() ? swap32(value) : value;
    }
    
    // ========== 64位转换 ==========
    static inline uint64_t swap64(uint64_t value) {
        return ((value & 0x00000000000000FFULL) << 56) |
               ((value & 0x000000000000FF00ULL) << 40) |
               ((value & 0x0000000000FF0000ULL) << 24) |
               ((value & 0x00000000FF000000ULL) << 8)  |
               ((value & 0x000000FF00000000ULL) >> 8)  |
               ((value & 0x0000FF0000000000ULL) >> 24) |
               ((value & 0x00FF000000000000ULL) >> 40) |
               ((value & 0xFF00000000000000ULL) >> 56);
    }
    
    static inline uint64_t hostToNetwork64(uint64_t value) {
        return isLittleEndian() ? swap64(value) : value;
    }
    
    static inline uint64_t networkToHost64(uint64_t value) {
        return isLittleEndian() ? swap64(value) : value;
    }
    
    // ========== 带符号整数的便捷方法 ==========
    static inline int16_t hostToNetwork16(int16_t value) {
        uint16_t temp = *reinterpret_cast<uint16_t*>(&value);
        temp = hostToNetwork16(temp);
        return *reinterpret_cast<int16_t*>(&temp);
    }
    
    static inline int16_t networkToHost16(int16_t value) {
        uint16_t temp = *reinterpret_cast<uint16_t*>(&value);
        temp = networkToHost16(temp);
        return *reinterpret_cast<int16_t*>(&temp);
    }
    
    static inline int32_t hostToNetwork32(int32_t value) {
        uint32_t temp = *reinterpret_cast<uint32_t*>(&value);
        temp = hostToNetwork32(temp);
        return *reinterpret_cast<int32_t*>(&temp);
    }
    
    static inline int32_t networkToHost32(int32_t value) {
        uint32_t temp = *reinterpret_cast<uint32_t*>(&value);
        temp = networkToHost32(temp);
        return *reinterpret_cast<int32_t*>(&temp);
    }
    
    static inline int64_t hostToNetwork64(int64_t value) {
        uint64_t temp = *reinterpret_cast<uint64_t*>(&value);
        temp = hostToNetwork64(temp);
        return *reinterpret_cast<int64_t*>(&temp);
    }
    
    static inline int64_t networkToHost64(int64_t value) {
        uint64_t temp = *reinterpret_cast<uint64_t*>(&value);
        temp = networkToHost64(temp);
        return *reinterpret_cast<int64_t*>(&temp);
    }
    
};

}
}


#endif