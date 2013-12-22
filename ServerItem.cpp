#include "ServerItem.h"
void ServerItem::SendRequest(int requestId,const TcpClient::Buffer& buf)
{
    mPendingRequests.push_back(requestId);
    mSocket.Send(buf);
}

void ServerItem::SendRequest(int requestId,const VBuffer& bufs)
{
    mPendingRequests.push_back(requestId);
    mSocket.Send(bufs);
}

