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


bool MemResult::FillReceiveBuffer(ERequestStatus status,VBuffer& bufs,int valueLen)
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

bool MemGetResult::FillReceiveBuffer(ERequestStatus status,VBuffer& bufs,int valueLen)
{
    if(ERequest_SUCCESS == status)
    {
        bufs.push_back(Buffer(&mFlag,4));
    }
    return Base::FillReceiveBuffer(status,bufs,valueLen);
}

MemIncResult::MemIncResult(const std::string& key)
    :MemResult(key,Buffer())
{}

bool MemIncResult::FillReceiveBuffer(ERequestStatus status,VBuffer& bufs,int valueLen)
{
    if(ERequest_SUCCESS == status)
    {
        bufs.push_back(Buffer(&mValue,8));
	return true;
    }
    else
    {
	return Base::FillReceiveBuffer(status,bufs,valueLen);
    }
}

