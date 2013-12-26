#ifndef _SERVERITEM_H
#define _SERVERITEM_H
#include <boost/lexical_cast.hpp>
#include "TcpClient.h"
class ServerItem
{
public:
    typedef boost::shared_ptr<ServerItem> Ptr;

    explicit ServerItem(boost::asio::io_service& ioService,const std::string& host,int port = 11211,int weight = 1)
	:mSocket(ioService,24)
	,mHost(host)
	,mPort(port)
	,mWeight(weight)
    {}

    TcpClient* operator-> () { return &mSocket; }

    const std::string& Host( void ) const { return mHost; }
    int Port( void ) const { return mPort; }
    int Weight( void ) const { return mWeight; }

    static std::string ToString(const std::string& host,int port) 
    {
       	return host + boost::lexical_cast<std::string>(port);
    }

    std::string ToString( void ) const { return ToString(mHost,mPort);}

    void SendRequest(int requestId,const ConstBuffer& buf);
    void SendRequest(int requestId,const SVConstBuffer& bufs);

    const std::vector<int>& PendingRequests() const { return mPendingRequests; }

private:
    TcpClient mSocket;
    std::string mHost;
    int mPort;
    int mWeight;

    std::vector<int> mPendingRequests;
};
#endif
 
