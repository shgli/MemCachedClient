#ifndef _MEMCACHEDCLIENT_INTERNAL_MACROS_
#define _MEMCACHEDCLIENT_INTERNAL_MACROS_

#ifdef _WIN32
#ifdef BUILDING_MEMCACHEDCLIENT
#define MEMCACHEDCLIENT_EXPORT __declspec(dllexport)
#else
#define MEMCACHEDCLIENT_EXPORT __declspec(dllimport)
#endif
#else
#define MEMCACHEDCLIENT_EXPORT 
#endif
#endif
