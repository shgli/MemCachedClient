#include "ServerItem.h"
void ServerItem::SendRequest(int requestId,const ConstBuffer& buf)
{
    mPendingRequests.push_back(requestId);
    mSocket.Send(buf);
}

void ServerItem::SendRequest(int requestId,const VConstBuffer& bufs)
{
    mPendingRequests.push_back(requestId);
    mSocket.Send(bufs);
}

