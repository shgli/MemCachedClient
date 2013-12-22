#ifndef _TCPCLIENT_H
#define _TCPCLIENT_H
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/noncopyable.hpp>
#include <SharedBuffer.h>

class TcpClient:boost::noncopyable
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


    typedef std::function<bool (const void *,VBuffer& )> HeaderReadedEvent;
    typedef std::function<void (const void* ,const VBuffer& )> BodyReadedEvent;
    typedef signals2::signal<void (ESocketError,const system::error_code&)> ErrorEvent;
    typedef signals2::signal<void ( void )> ClosedEvent;
    typedef signals2::signal<void ()> ConnectedEvent;

    TcpClient(boost::asio::io_service& ioService,size_t nHeaderLen);
    ~TcpClient();

    ConnectedEvent OnConnected;
    HeaderReadedEvent OnHeader;
    BodyReadedEvent OnBody;
    ErrorEvent OnError;
    ClosedEvent OnClosed;
public:
    void Connect(const std::string& host,int port);
    void Send(const ConstBuffer& buf);
    void Send(const VConstBuffer& vbuf);
    void Close( void );

private:
    void Send();
    void ReadHeader( void );
    void ReadBody( void );
    void Close4Error(ESocketError type,const system::error_code err);

    VConstBuffer mWriteBuffers;
    VConstBuffer mPendingBuffers;
    VBuffer mReadBuffers;

    bool mIsInWriting;
    boost::asio::ip::tcp::socket mSocket;
    size_t mHeaderLength;
    char* const mHeaderBuffer;
};
#endif
 
