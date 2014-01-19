#ifndef _INTERNAL_MACROS_H
#define _INTERNAL_MACROS_H

#ifdef _WIN32
#ifdef BUILDING_COMMON
#define COMMON_EXPORT __declspec(dllexport)
#else
#define COMMON_EXPORT __declspec(dllimport) 
#endif
#else
#define COMMON_EXPORT
#endif
#endif

