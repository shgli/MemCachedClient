#ifndef _TCPCLIENT_H
#define _TCPCLIENT_H
#include <boost/asio/io_service.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/atomic.hpp>
#include <boost/noncopyable.hpp>
#include <SharedBuffer.h>
using namespace boost;
class Buffer;
class TcpClient:noncopyable
{
public:
    enum ESocketError
    {
	ESocket_Connect,
	ESocket_ReadHeader,
	ESocket_ReadBody,
	ESocket_Send,
	ESocket_BodyBuffer
    };

    typedef function<int (void* const header,Buffer*& body)> HeaderReadedEvent;
    typedef signals2::signal<function<void (const SharedBuffer& boday)> > BodyReadedEvent;
    typedef signals2::signal<function<void (ESocketError type,const system::error_code err)> > ErrorEvent;
    typedef signals2::signal<function<void ( void )> > CloseEvent;

    TcpClient(boost::asio::io_service& ioService,size_t nHeaderLen,HeaderReadedEvent onHeader);

    HeaderReadedEvent OnHeader;
    BodyReadedEvent OnBody;
    ErrorEvent OnError;
    CloseEvent OnClosed;
public:
    void Connect(const std::string& host,int port);
    void Send(const SharedBuffer& buf);
    void Close( void );

private:
    void ReadHeader();
    void ReadBody(const Buffer& bodyBuf);
    void Close4Error(ESocketError type,const system::error_code err);

    std::vector<SharedBuffer> mWritingBuffers;
    std::vector<SharedBuffer> mPendingBuffers;
    boost::mutex mWriteBufMut;
    boost::atomic_bool mIsInWriting;

    asio::ip::tcp::socket mSocket;
    size_t mHeaderLength;
    char* const mHeaderBuffer;
};
#endif
 
