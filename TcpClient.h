#ifndef _TCPCLIENT_H
#define _TCPCLIENT_H
#include <boost/asio/io_service.hpp>
#include <boost/asio/buffer.hpp>
#include <SharedBuffer.h>
class TcpClient
{
public:
    TcpClient(boost::asio::io_service& ioService);


public:
    void Connect(const std::string& host,int port);
    void Send(const SharedBuffer& buf);
    void Close( void );
};
#endif
 
