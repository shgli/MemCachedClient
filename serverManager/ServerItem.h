#ifndef _SERVERITEM_H
#define _SERVERITEM_H
#include <boost/lexical_cast.hpp>
#include "network/TcpClient.h"
#include "serverManager/InternalMacros.h"

class ServerItem
{
public:
    typedef boost::shared_ptr<ServerItem> Ptr;

    SERVERMGR_EXPORT explicit ServerItem(boost::asio::io_service& ioService,const std::string& host,int port = 11211,int weight = 1)
        :mSocket(ioService,24)
        ,mHost(host)
        ,mPort(port)
        ,mWeight(weight)
    {}

    SERVERMGR_EXPORT TcpClient* operator-> () { return &mSocket; }

    SERVERMGR_EXPORT const std::string& Host( void ) const { return mHost; }
    SERVERMGR_EXPORT int Port( void ) const { return mPort; }
    SERVERMGR_EXPORT int Weight( void ) const { return mWeight; }

    SERVERMGR_EXPORT static std::string ToString(const std::string& host,int port) 
    {
        return host + boost::lexical_cast<std::string>(port);
    }

    SERVERMGR_EXPORT std::string ToString( void ) const { return ToString(mHost,mPort);}

    SERVERMGR_EXPORT void SendRequest(int requestId,const ConstBuffer& buf);
    SERVERMGR_EXPORT void SendRequest(int requestId,const SVConstBuffer& bufs);

    SERVERMGR_EXPORT  const std::vector<int>& PendingRequests() const { return mPendingRequests; }

private:
    TcpClient mSocket;
    std::string mHost;
    int mPort;
    int mWeight;

    std::vector<int> mPendingRequests;
};
#endif

