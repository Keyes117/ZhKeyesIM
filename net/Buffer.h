#ifndef NET_BUFFER_H_
#define NET_BUFFER_H_

#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

#include "net_export.h"
class NET_API Buffer
{
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    Buffer();
    ~Buffer() = default;

    Buffer(const Buffer& rhs);
    Buffer& operator=(const Buffer& rhs);

    Buffer(Buffer&& rhs) noexcept;
    Buffer& operator=(Buffer&& rhs) noexcept;

public:
    operator const char* ();

public:
    size_t readableBytes() const { return m_writeIndex - m_readIndex; }
    size_t writableBytes() const { return m_buffer.size() - m_writeIndex; }
    size_t prependableBytes() const { return m_readIndex; }

    const char* peek() const { return beginPtr() + m_readIndex; }
    const char* findCRLF() const;

    void retrieve(size_t len);
    void retrieveUntil(const char* end);
    void retrieveAll();

    std::string retrieveAllAsString();
    std::string retrieveAsString(size_t len);

    void append(const char* data, size_t len);
    void append(const std::string& str);

    void ensureWritableBytes(size_t len);


    char* beginWrite() { return beginPtr() + m_writeIndex; }
    const char* beginWrite() const { return beginPtr() + m_writeIndex; }

    void prepend(const void* data, size_t len);
    void shrink(size_t reserve = 0);

    void swap(Buffer& rhs);

private:
    char* beginPtr() { return &*m_buffer.begin(); }
    const char* beginPtr() const { return &*m_buffer.begin(); }

    void makeSpace(size_t len);
private:
    std::vector<char> m_buffer;
    size_t m_readIndex;
    size_t m_writeIndex;

};

#endif