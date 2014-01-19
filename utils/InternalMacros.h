#ifndef _UTILS_INTERNAL_MACROS_
#define _UTILS_INTERNAL_MACROS_

#ifdef _WIN32
#ifdef BUILDING_UTILS
#define UTILS_EXPORT __declspec(dllexport)
#else
#define UTILS_EXPORT __declspec(dllimport)
#endif
#else
#define UTILS_EXPORT
#endif
#endif
