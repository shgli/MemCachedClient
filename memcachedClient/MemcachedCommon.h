#ifndef _MEMCACHEDCOMMON_H
#define _MEMCACHEDCOMMON_H

#ifdef _MEMCACHED_CLIENT_COMPILE
#define _internal public //conflict with std::ios_base::internal
#else
#define _internal protected
#endif

typedef size_t hash_t;

#include "memcachedClient/InternalMacros.h"
#endif
 
