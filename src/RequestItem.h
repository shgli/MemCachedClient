#ifndef _REQUESTITEM_H
#define _REQUESTITEM_H
#include <vector>
#include <string>
#include "MemResult.h"

typedef boost::function<void (const MemResult::Ptr&)> Callback;
class RequestItem
{
    MemResult::Ptr mResult;
    Callback mCallback;
public:
    explicit RequestItem(Callback callback,const MemResult::Ptr& result);

    bool FillReceiveBuffer(ERequestStatus status,VBuffer& bufs,int valueLen) { return mResult->FillReceiveBuffer(status,bufs,valueLen); }

    void Notify(ERequestStatus status);
};
#endif
 
