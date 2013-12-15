#include "SharedBuffer.h"
boost::object_pool<const_buffer> SharedBuffer::gBufferPool(64);
