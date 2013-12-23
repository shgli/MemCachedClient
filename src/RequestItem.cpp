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
