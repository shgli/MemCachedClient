#include "MemResult.h"
MemResult::MemResult(const std::string& key,const SharedBuffer& buffer)
    :mKey(key)
    ,mErrorCode(ERequest_PENDING)
    ,mValue(buffer)
{}

void MemResult::Notify(ERequestStatus err)
{
    std::scoped_lock<std::mutex> lock;
    mErrorCode = err;
    mSyncEvent.notify_all();
}

int MemResult::Finish( void ) const
{
    std::scoped_lock<std::mutex> lock;
    mSyncEvent.wait(lock);

    return mErrorCode;
}


void MemResult::FillReceiveBuffer(VBuffer& bufs,int valueLen)
{
    if(mValue.IsNull())
    {
	mValue.reset(gValuePool.malloc(valueLen),valueLen,
		[this](void* p)
		{
		    gValuePool.free(p);
		});
    }
    bufs.push_back(mValue);
}

MemGetResult(const std::string& key,const SharedBuffer& buffer)
    :Base(key,buffer)
{}

void MemGetResult::FillReceiveBuffer(VBuffer& bufs,int valueLen)
{
    bufs.push_back(SharedBuffer(&mFlag,4));
    Base::FillReceiveBuffer(bufs);
}
