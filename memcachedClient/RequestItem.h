#ifndef _REQUESTITEM_H
#define _REQUESTITEM_H
#include <vector>
#include <string>
#include "memcachedClient/MemResult.h"

typedef boost::function<void (const MemResult::Ptr&)> Callback;
class RequestItem
{
    MemResult::Ptr mResult;
    Callback mCallback;
public:
    explicit RequestItem(Callback callback,const MemResult::Ptr& result);

_internal:
    bool FillReceiveBuffer(ERequestStatus status,VBuffer& bufs,int valueLen,int keyLen);

    bool HasMoreResult( void ) const;
    void Notify(ERequestStatus status);
};
#endif
 
