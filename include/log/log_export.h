
#ifndef LOG_LOG_EXPORT_H_
#define LOG_LOG_EXPORT_H_

#if defined(_WIN32) && defined(LOG_EXPORTS)
#  define LOG_API __declspec(dllexport)
#elif defined(_WIN32)
#  define LOG_API __declspec(dllimport)
#else
#  define LOG_API
#endif




#endif