#ifndef _SERVERITEM_H
#define _SERVERITEM_H
#include "TcpClient.h"
class ServerItem
{
public:
    explicit ServerItem(const std::string& host,int port = 11211,int weight = 1)
	:mHost(host)
	,mPort(port)
	,mWeight(weight)
    {}

    TcpClient* operator-> () { return mSocket; }

    const std::string& Host( void ) const { return mHost; }
    int Port( void ) const { return mPort; }
    int Weight( void ) const { return mWeight; }
private:
    TcpClient mSocket;
    std::string mHost;
    int mPort;
    int mWeight;
};
#endif
 
