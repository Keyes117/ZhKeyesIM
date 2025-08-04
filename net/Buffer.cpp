#include "Buffer.h"

Buffer::Buffer()
    :m_buffer(kCheapPrepend + kInitialSize),
    m_readIndex(kCheapPrepend),
    m_writeIndex(kCheapPrepend)
{
}

Buffer::Buffer(const Buffer& rhs)
    :m_buffer(rhs.m_buffer),
    m_readIndex(rhs.m_readIndex),
    m_writeIndex(rhs.m_writeIndex)
{

}

Buffer& Buffer::operator=(const Buffer& rhs)
{
    if (this != &rhs)
    {
        m_buffer = rhs.m_buffer;
        m_readIndex = rhs.m_readIndex;
        m_writeIndex = rhs.m_writeIndex;
    }

    return *this;
}

Buffer::Buffer(Buffer&& rhs) noexcept
    :m_buffer(std::move(rhs.m_buffer)),
    m_readIndex(rhs.m_readIndex),
    m_writeIndex(rhs.m_writeIndex)
{
    rhs.m_readIndex = rhs.m_writeIndex = kCheapPrepend;
}

Buffer& Buffer::operator=(Buffer&& rhs) noexcept
{
    if (this != &rhs) {
        m_buffer = std::move(rhs.m_buffer);
        m_readIndex = rhs.m_readIndex;
        m_writeIndex = rhs.m_writeIndex;

        // 将源对象重置为初始状态
        rhs.m_readIndex = rhs.m_writeIndex = kCheapPrepend;
    }
    return *this;
}

Buffer::operator const char* ()
{
    return peek();
}

const char* Buffer::findCRLF() const
{
    const char crlf[] = "\r\n";
    const char* crlfpos = std::search(peek(), beginPtr() + m_writeIndex, crlf, crlf + 2);

    return crlfpos == beginPtr() + m_writeIndex ? nullptr : crlfpos;
}

void Buffer::retrieve(size_t len)
{
    assert(len <= readableBytes());
    if (len < readableBytes())
        m_readIndex += len;
    else
        retrieveAll();
}

void Buffer::retrieveUntil(const char* end)
{
    assert(peek() <= end && end <= beginPtr() + m_writeIndex);
    retrieve(static_cast<size_t>(end - peek()));
}

void Buffer::retrieveAll()
{
    m_readIndex = m_writeIndex = kCheapPrepend;
}

std::string Buffer::retrieveAllAsString()
{
    std::string str(peek(), readableBytes());
    retrieveAll();
    return str;
}

std::string Buffer::retrieveAsString(size_t len)
{
    assert(len <= readableBytes());
    std::string str(peek(), len);
    retrieve(len);
    return str;

}

void Buffer::append(const char* data, size_t len)
{
    ensureWritableBytes(len);
    std::copy(data, data + len, beginPtr() + m_writeIndex);
    m_writeIndex += len;
}

void Buffer::append(const std::string& str)
{
    append(str.data(), str.size());
}

void Buffer::ensureWritableBytes(size_t len)
{
    if (writableBytes() < len)
        makeSpace(len);

    assert(writableBytes() >= len);
}

void Buffer::prepend(const void* data, size_t len)
{
    assert(len <= prependableBytes());
    m_readIndex -= len;
    const char* d = static_cast<const char*>(data);
    std::copy(d, d + len, beginPtr() + m_readIndex);
}

void Buffer::shrink(size_t reserve /* = 0*/)
{
    Buffer other;
    other.ensureWritableBytes(readableBytes() + reserve);
    other.append(peek(), readableBytes());
    swap(other);
}

void Buffer::swap(Buffer& rhs)
{
    m_buffer.swap(rhs.m_buffer);
    std::swap(m_readIndex, rhs.m_readIndex);
    std::swap(m_writeIndex, rhs.m_writeIndex);
}

void Buffer::makeSpace(size_t len)
{
    if (writableBytes() + prependableBytes() < len + kCheapPrepend)
    {
        //真正需要扩容
        m_buffer.resize(m_writeIndex + len);
    }
    else
    {
        size_t readable = readableBytes();
        std::copy(beginPtr() + m_readIndex,
            beginPtr() + m_writeIndex,
            beginPtr() + kCheapPrepend);

        m_readIndex = kCheapPrepend;
        m_writeIndex = m_readIndex + readable;
    }
}

