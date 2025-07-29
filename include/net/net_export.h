#ifndef NET_NET_EXPORT_H_
#define NET_NET_EXPORT_H_

#if defined(_WIN32) && defined(NET_EXPORTS)
#  define NET_API __declspec(dllexport)
#elif defined(_WIN32)
#  define NET_API __declspec(dllimport)
#else
#  define NET_API
#endif

#endif
