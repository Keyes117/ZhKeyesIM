#ifndef UTIL_UTIL_EXPORT_H_
#define UTIL_UTIL_EXPORT_H_

#define UTIL_EXPORTS

#ifdef UTIL_EXPORTS
#define UTIL_API __declspec(dllexport)
#else
#define UTIL_API __declspec(dllimport)
#endif

#endif