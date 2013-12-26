#include "RequestItem.h"
RequestItem::RequestItem(Callback callback,const MemResult::Ptr& result)
    :mResult(result)
    ,mCallback(callback)
{}

void RequestItem::Notify(ERequestStatus status)
{
    mResult->Notify(status);
    if(!mCallback.empty())
    {
	mCallback(mResult);
    }
}

bool RequestItem::FillReceiveBuffer(ERequestStatus status,VBuffer& bufs,int valueLen,int keyLen)
{
    return mResult->FillReceiveBuffer(status,bufs,valueLen,keyLen); 
}

bool RequestItem::HasMoreResult( void ) const 
{
    return mResult->HasMoreResult(); 
}

