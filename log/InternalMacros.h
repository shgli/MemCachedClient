#ifndef _LOG_INTERNAL_MACROS_
#define _LOG_INTERNAL_MACROS_

#ifdef _WIN32
#ifdef BUILDING_LOG
#define LOG_EXPORT __declspec(dllexport)
#else
#define LOG_EXPORT __declspec(dllimport)
#endif
#else
#define LOG_EXPORT
#endif
#endif

