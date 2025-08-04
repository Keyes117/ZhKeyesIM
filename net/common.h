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

#endif // NET_COMMON_H_ 

