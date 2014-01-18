#ifndef _LOG_INTERNAL_MACROS_
#define _LOG_INTERNAL_MACROS_

#ifdef BUILDING_LOG
#define LOG_EXPORT __declspec(dllexport)
#else
#define LOG_EXPORT __declspec(dllimport)
#endif
#endif

