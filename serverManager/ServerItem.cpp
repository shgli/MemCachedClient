#include "serverManager/ServerItem.h"
SERVERMGR_EXPORT void ServerItem::SendRequest(int requestId,const ConstBuffer& buf)
{
    mPendingRequests.push_back(requestId);
    mSocket.Send(buf);
}

SERVERMGR_EXPORT void ServerItem::SendRequest(int requestId,const SVConstBuffer& bufs)
{
    mPendingRequests.push_back(requestId);
    mSocket.Send(bufs);
}

