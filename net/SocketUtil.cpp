#include "common.h"

#include <string>

#ifdef _WIN32

#else
#include <sys/ioctl.h>
#endif

namespace net {

bool SocketUtil::InitNetwork() {
#ifdef _WIN32
    WSADATA wsaData;
    return ::WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
#else
    return true; // Linux不需要初始化
#endif
}

void SocketUtil::CleanupNetwork() {
#ifdef _WIN32
    ::WSACleanup();
#endif
}

bool SocketUtil::SetNonBlocking(SOCKET sock) {
#ifdef _WIN32
    unsigned long mode = 1;
    return ::ioctlsocket(sock, FIONBIO, &mode) == 0;
#else
    int flags = ::fcntl(sock, F_GETFL, 0);
    if (flags == -1) return false;
    return ::fcntl(sock, F_SETFL, flags | O_NONBLOCK) != -1;
#endif
}

bool SocketUtil::SetReuseAddr(SOCKET sock, bool reuse) {
    int optval = reuse ? 1 : 0;
    return ::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, 
                       reinterpret_cast<const char*>(&optval), 
                       sizeof(optval)) == 0;
}

bool SocketUtil::SetReusePort(SOCKET sock, bool reuse) {
#ifdef SO_REUSEPORT
    int optval = reuse ? 1 : 0;
    return ::setsockopt(sock, SOL_SOCKET, SO_REUSEPORT,
                       reinterpret_cast<const char*>(&optval),
                       sizeof(optval)) == 0;
#else
    // Windows不支持SO_REUSEPORT
    return true;
#endif
}

bool SocketUtil::SetTcpNoDelay(SOCKET sock, bool nodelay) {
    int optval = nodelay ? 1 : 0;
    return ::setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,
                       reinterpret_cast<const char*>(&optval),
                       sizeof(optval)) == 0;
}

bool SocketUtil::SetKeepAlive(SOCKET sock, bool keepalive) {
    int optval = keepalive ? 1 : 0;
    return ::setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE,
                       reinterpret_cast<const char*>(&optval),
                       sizeof(optval)) == 0;
}

std::string SocketUtil::GetErrorString(int error_code) {
#ifdef _WIN32
    char* msg = nullptr;
    ::FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<char*>(&msg), 0, nullptr);
    std::string result = msg ? msg : "Unknown error";
    if (msg) ::LocalFree(msg);
    return result;
#else
    return std::strerror(error_code);
#endif
}

} // namespace net