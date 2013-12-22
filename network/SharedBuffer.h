#ifndef _SHAREDBUFFER_H
#define _SHAREDBUFFER_H
#include <functional>
#include <memory>
#include <boost/asio/buffer.hpp>
#include <boost/pool/object_pool.hpp>
using namespace boost;
using namespace boost::asio;
template<typename value_type>
class SharedBuffer
{
public:
    typedef std::function<void (void* data)> D;

    SharedBuffer(void* data,size_t size,D d = DefaultD)
	:mData(gBufferPool.construct(data,size),std::bind(Free,std::placeholders::_1,d))
    {}

    SharedBuffer(){}

    operator const value_type& () const{ return *mData; }

    void Reset(void* data,size_t size,D d = DefaultD)
    {
          mData.reset(gBufferPool.construct(data,size),std::bind(Free,std::placeholders::_1,d));
    }

    template<typename T>
    T& GetHeader( void )
    {
	return const_cast<T&>(*buffer_cast<const T*>(*mData.get()));
    }

    template<typename T>
    void* GetBody( void )
    {
	return const_cast<char*>(buffer_cast<const char*>(*mData.get()) + sizeof(T));
    }

    bool IsNull( void ) const { return nullptr == mData.get();}

private:
    static void Free(value_type* pBuffer,D d)
    {
	d(const_cast<void*>(buffer_cast<const void*>(*pBuffer)));
	gBufferPool.destroy(pBuffer);
    }

    static void DefaultD(const void* data) {}

    std::shared_ptr<value_type> mData;
    static boost::object_pool<value_type> gBufferPool;
};
typedef SharedBuffer<mutable_buffer> Buffer;
typedef SharedBuffer<const_buffer> ConstBuffer;
typedef std::vector<Buffer> VBuffer;
typedef std::vector<ConstBuffer> VConstBuffer;

#endif
 
