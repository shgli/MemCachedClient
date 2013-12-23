#include "MemResult.h"
MemResult::MemResult(const std::string& key,const Buffer& buffer)
    :mKey(key)
    ,mErrorCode(ERequest_PENDING)
    ,mValue(buffer)
{}

void MemResult::Notify(ERequestStatus err)
{
    boost::unique_lock<boost::mutex> lock(mSyncMut);
    mErrorCode = err;
    mSyncEvent.notify_all();
}

ERequestStatus MemResult::Finish( void )
{
    boost::unique_lock<boost::mutex> lock(mSyncMut);
    mSyncEvent.wait(lock);

    return mErrorCode;
}


bool MemResult::FillReceiveBuffer(VBuffer& bufs,int valueLen)
{
    if(mValue.IsNull())
    {
	mValue.Reset(malloc(valueLen),valueLen,
		[this](void* p)
		{
		    free(p);
		});
    }
    bufs.push_back(mValue);

    return true;
}

MemGetResult::MemGetResult(const std::string& key,const Buffer& buffer)
    :Base(key,buffer)
{}

bool MemGetResult::FillReceiveBuffer(VBuffer& bufs,int valueLen)
{
    bufs.push_back(Buffer(&mFlag,4));
    return Base::FillReceiveBuffer(bufs,valueLen);
}
