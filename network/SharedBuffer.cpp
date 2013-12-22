#include "SharedBuffer.h"
boost::object_pool<const_buffer> SharedBuffer::gBufferPool(64);

SharedBuffer::SharedBuffer()
{}

void SharedBuffer::Reset(const void* data,size_t size,D d = DefaultD)
{
     mData.reset(gBufferPool.construct(data,size),boost::bind(Free,_1,d));
}

