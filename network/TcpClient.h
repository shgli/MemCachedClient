#ifndef _TCPCLIENT_H
#define _TCPCLIENT_H
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/noncopyable.hpp>
#include "network/SharedBuffer.h"
#include "network/InternalMacros.h"
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


    typedef boost::function<bool (void *,VBuffer& )> HeaderReadedEvent;
    typedef boost::function<void (const void* ,const VBuffer& )> BodyReadedEvent;
    typedef signals2::signal<void (ESocketError,const system::error_code&)> ErrorEvent;
    typedef signals2::signal<void ( void )> ClosedEvent;
    typedef signals2::signal<void ()> ConnectedEvent;

    NETWORK_EXPORT TcpClient(boost::asio::io_service& ioService,size_t nHeaderLen);
    NETWORK_EXPORT ~TcpClient();

    ConnectedEvent OnConnected;
    HeaderReadedEvent OnHeader;
    BodyReadedEvent OnBody;
    ErrorEvent OnError;
    ClosedEvent OnClosed;
public:
    NETWORK_EXPORT void Connect(const std::string& host,int port);
    NETWORK_EXPORT void Send(const ConstBuffer& buf);
    NETWORK_EXPORT void Send(const SVConstBuffer& vbuf);
    NETWORK_EXPORT void Close( void );

protected:
    NETWORK_EXPORT boost::asio::ip::tcp::socket& Socket() { return mSocket; }

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

