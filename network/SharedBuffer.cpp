#include "SharedBuffer.h"
template<typename value_type>
boost::object_pool<value_type> SharedBuffer<value_type>::gBufferPool(64);

