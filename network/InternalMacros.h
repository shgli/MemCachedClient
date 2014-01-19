#ifndef _NETWORK_INTERNAL_MACROS_
#define _NETWORK_INTERNAL_MACROS_

#ifdef _WIN32
#ifdef BUILDING_NETWORK
#define NETWORK_EXPORT __declspec(dllexport)
#else
#define NETWORK_EXPORT __declspec(dllimport)
#endif
#else
#define NETWORK_EXPORT
#endif
#endif
