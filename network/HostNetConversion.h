#ifndef _HOST_NET_CONVERSION_H
#define _HOST_NET_CONVERSION_H
#include <cstdint>
#include "network/InternalMacros.h"
NETWORK_EXPORT uint64_t ntohll(uint64_t val);
NETWORK_EXPORT uint64_t htonll(uint64_t val);
#endif


