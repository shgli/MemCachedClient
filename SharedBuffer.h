#ifndef _SHAREDBUFFER_H
#define _SHAREDBUFFER_H
#include <boost/asio/buffer.hpp>
#include <boost/pool/object_pool.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
using namespace boost;
using namespace boost::asio;

class SharedBuffer
{
public:
    typedef boost::function<void (const void* data)> D;

    SharedBuffer(const void* data,size_t size,D d = DefaultD)
	:mData(gBufferPool.construct(data,size),boost::bind(Free,_1,d))
    {}

    typedef const_buffer value_type;
    typedef const boost::asio::const_buffer* const_iterator;
    const value_type* begin() const { return mData.get();}
    const value_type* end() const { return mData.get() + 1;}

    template<T>
    T& GetHeader( void )
    {
	return const_cast<T&>(*buffer_cast<const T*>(*mData.get()));
    }

    template<typename T>
    void* GetBody( void )
    {
	return const_cast<void*>(buffer_cast<const void*>(*mData.get()) + sizeof(T));

    }

private:
    static void Free(const_buffer* pBuffer,D d)
    {
	d(buffer_cast<const void*>(*pBuffer));
	gBufferPool.destroy(pBuffer);
    }

    static void DefaultD(const void* data) {}

    boost::shared_ptr<const_buffer> mData;
    static boost::object_pool<const_buffer> gBufferPool;
};
#endif
 
