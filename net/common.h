#ifndef NET_COMMON_H_
#define NET_COMMON_H_

#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>

// 自动链接Windows Socket库
#pragma comment(lib, "ws2_32.lib")

#define GetSocketError() ::WSAGetLastError()

// 避免宏重定义警告
#ifndef EWOULDBLOCK
#define EWOULDBLOCK WSAEWOULDBLOCK
#endif

#ifndef EAGAIN 
#define EAGAIN WSAEWOULDBLOCK
#endif

#ifndef EINPROGRESS
#define EINPROGRESS WSAEINPROGRESS
#endif

#ifndef EINTR
#define EINTR WSAEINTR
#endif

#ifndef ECONNREFUSED
#define ECONNREFUSED WSAECONNREFUSED
#endif

#ifndef ECONNRESET
#define ECONNRESET WSAECONNRESET
#endif

#ifndef ENOTCONN
#define ENOTCONN WSAENOTCONN
#endif

#else
// Linux平台
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

typedef int SOCKET;
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define GetSocketError() errno
#define closesocket(s)  ::close(s)
#endif // _WIN32



#include <string>

namespace net {

    // 跨平台的socket工具函数
    class SocketUtil {
    public:
        // 初始化网络库（Windows需要WSAStartup）
        static bool InitNetwork();

        // 清理网络库
        static void CleanupNetwork();

        // 设置非阻塞模式
        static bool SetNonBlocking(SOCKET sock);

        // 设置socket选项：重用地址
        static bool SetReuseAddr(SOCKET sock, bool reuse = true);

        // 设置socket选项：重用端口
        static bool SetReusePort(SOCKET sock, bool reuse = true);

        // 设置TCP_NODELAY
        static bool SetTcpNoDelay(SOCKET sock, bool nodelay = true);

        // 设置socket选项：保活
        static bool SetKeepAlive(SOCKET sock, bool keepalive = true);

        // 获取错误信息字符串
        static std::string GetErrorString(int error_code);
    };

} // namespace net

#endif // NET_COMMON_H_ 