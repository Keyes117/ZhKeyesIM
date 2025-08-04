#ifndef Util_Util_EXPORT_H_
#define Util_Util_EXPORT_H_

#if defined(_WIN32) && defined(Util_EXPORTS)
#  define Util_API __declspec(dllexport)
#elif defined(_WIN32)
#  define Util_API __declspec(dllimport)
#else
#  define Util_API
#endif

#endif