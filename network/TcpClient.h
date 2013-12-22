#ifndef _TCPCLIENT_H
#define _TCPCLIENT_H
#include <boost/asio/io_service.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/noncopyable.hpp>
#include <SharedBuffer.h>
using namespace boost;
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


    typedef signals2::signal<int (void* const header,VBuffer& body)> HeaderReadedEvent;
    typedef signals2::signal<void (void* const header,const VBuffer& boday)> BodyReadedEvent;
    typedef signals2::signal<void (ESocketError type,const system::error_code err)> ErrorEvent;
    typedef signals2::signal<void ( void )> ClosedEvent;
    typedef signals2::signal<void ()> ConnectedEvent;

    TcpClient(boost::asio::io_service& ioService,size_t nHeaderLen);

    ConnectedEvent OnConnected;
    HeaderReadedEvent OnHeader;
    BodyReadedEvent OnBody;
    ErrorEvent OnError;
    ClosedEvent OnClosed;
public:
    void Connect(const std::string& host,int port);
    void Send(const Buffer& buf);
    void Send(const VBuffer& vbuf);
    void Close( void );

private:
    void ReadHeader();
    void ReadBody(const Buffer& bodyBuf);
    void Close4Error(ESocketError type,const system::error_code err);

    VBuffer mWriteBuffers;
    VBuffer mPendingBuffers;
    VBuffer mReadBuffers;

    bool mIsInWriting;
    asio::ip::tcp::socket mSocket;
    size_t mHeaderLength;
    char* const mHeaderBuffer;
};
#endif
 
