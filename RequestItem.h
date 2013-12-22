#ifndef _REQUESTITEM_H
#define _REQUESTITEM_H
#include <vector>
#include <thread>
#include <string>
#include "MemResult.h"

class RequestItem
{
    MemResult::Ptr mResult;
    Callback mCallback;
public:
    typedef std::shared_ptr<RequestItem> Ptr;

    explicit RequestItem(Callback callback,MemResult::Ptr& result);

    void FillReceiveBuffer(VBuffer& bufs,int valueLen) { mResult->FillReceiveBuffer(bufs,valueLen); }

    void Notify(ERequestStatus status);
};
#endif
 
