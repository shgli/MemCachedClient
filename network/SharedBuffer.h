#ifndef _SHAREDBUFFER_H
#define _SHAREDBUFFER_H
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/pool/object_pool.hpp>
#include <boost/bind.hpp>
using namespace boost;
using namespace boost::asio;
template<typename value_type>
class SharedBuffer
{
public:
    typedef boost::function<void (void* data)> D;

    SharedBuffer(void* data,size_t size,D d = DefaultD)
	:mData(gBufferPool.construct(data,size),boost::bind(Free,_1,d))
    {}

    SharedBuffer(const std::string& data)
	:mData(gBufferPool.construct(const_cast<void*>((const void*)data.c_str()),data.size()),DefaultD)
    {}

    SharedBuffer(){}

    operator const value_type& () const{ return *mData; }

    void Reset(void* data,size_t size,D d = DefaultD)
    {
          mData.reset(gBufferPool.construct(data,size),boost::bind(Free,_1,d));
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

    size_t Size( void ) const
    {
	return buffer_size(*mData.get());
    }

    std::string ToString( void ) const 
    {
	std::string strValue;
	strValue.resize(buffer_size(*mData));
	memcpy(const_cast<void*>((const void*)strValue.c_str()),buffer_cast<void *>(*mData.get()),strValue.size());

	return strValue;
    }

private:
    static void Free(value_type* pBuffer,D d)
    {
	d(const_cast<void*>(buffer_cast<const void*>(*pBuffer)));
	gBufferPool.destroy(pBuffer);
    }

    static void DefaultD(const void* data) {}

    boost::shared_ptr<value_type> mData;
    static boost::object_pool<value_type> gBufferPool;
};

template<typename value_type>
boost::object_pool<value_type> SharedBuffer<value_type>::gBufferPool(64);

typedef SharedBuffer<mutable_buffer> Buffer;
typedef SharedBuffer<const_buffer> ConstBuffer;
typedef std::vector<Buffer> VBuffer;
typedef std::vector<ConstBuffer> VConstBuffer;
typedef boost::shared_ptr<VBuffer> SVBuffer;
typedef boost::shared_ptr<VConstBuffer> SVConstBuffer;

#endif
 
