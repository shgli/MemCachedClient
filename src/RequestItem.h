#ifndef _REQUESTITEM_H
#define _REQUESTITEM_H
#include <vector>
#include <thread>
#include <string>
#include "MemResult.h"

typedef std::function<void (const MemResult::Ptr&)> Callback;
class RequestItem
{
    MemResult::Ptr mResult;
    Callback mCallback;
public:
    explicit RequestItem(Callback callback,const MemResult::Ptr& result);

    bool FillReceiveBuffer(VBuffer& bufs,int valueLen) { return mResult->FillReceiveBuffer(bufs,valueLen); }

    void Notify(ERequestStatus status);
};
#endif
 
