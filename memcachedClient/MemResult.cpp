#include "memcachedClient/MemResult.h"
MEMCACHEDCLIENT_EXPORT MemResult::MemResult(const std::string& key,const Buffer& buffer)
    :mKey(key)
    ,mErrorCode(ERequest_PENDING)
    ,mValue(buffer)
{}

MEMCACHEDCLIENT_EXPORT ERequestStatus MemResult::Finish( void )
{
    boost::unique_lock<boost::mutex> lock(mSyncMut);
    mSyncEvent.wait(lock);

    return mErrorCode;
}

void MemResult::Notify(ERequestStatus err)
{
    boost::unique_lock<boost::mutex> lock(mSyncMut);
    mErrorCode = err;
    mSyncEvent.notify_all();
}




bool MemResult::FillReceiveBuffer(ERequestStatus status,VBuffer& bufs,int valueLen,int keyLen)
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

bool MemGetResult::FillReceiveBuffer(ERequestStatus status,VBuffer& bufs,int valueLen,int keyLen)
{
    if(ERequest_SUCCESS == status)
    {
        bufs.push_back(Buffer(&mFlag,4));
    }
    return Base::FillReceiveBuffer(status,bufs,valueLen,keyLen);
}

MemIncResult::MemIncResult(const std::string& key)
    :MemResult(key,Buffer())
{}

bool MemIncResult::FillReceiveBuffer(ERequestStatus status,VBuffer& bufs,int valueLen,int keyLen)
{
    if(ERequest_SUCCESS == status)
    {
        bufs.push_back(Buffer(&mValue,8));
	return true;
    }
    else
    {
	return Base::FillReceiveBuffer(status,bufs,valueLen,keyLen);
    }
}
MemStatResult::MemStatResult(const std::string& key)
    :MemResult(key,Buffer())
{}

bool MemStatResult::FillReceiveBuffer(ERequestStatus status,VBuffer& bufs,int valueLen,int keyLen)
{
    if(0 == valueLen)
    {
	mHasMoreResult = false;
	return false;
    }

    mStats.push_back(Stat());
    auto& stat = mStats.back();
    stat.Key.resize(keyLen);
    stat.Value.resize(valueLen);
    bufs.push_back(Buffer(stat.Key));
    bufs.push_back(Buffer(stat.Value));
    return true;
}

bool MemStatResult::HasMoreResult( void )
{
    return mHasMoreResult;
}

