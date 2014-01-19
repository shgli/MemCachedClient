#ifndef _MEMRESULT_H
#define _MEMRESULT_H
#include "SharedBuffer.h"
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/pool/pool.hpp>
#include <memcached/protocol_binary.h>
#include "memcachedClient/MemcachedCommon.h"
enum ERequestStatus
{
    ERequest_SUCCESS = 0x00,
    ERequest_KEY_ENOENT = 0x01,
    ERequest_KEY_EEXISTS = 0x02,
    ERequest_E2BIG = 0x03,
    ERequest_EINVAL = 0x04,
    ERequest_NOT_STORED = 0x05,
    ERequest_DELTA_BADVAL = 0x06,
    ERequest_AUTH_ERROR = 0x20,
    ERequest_AUTH_CONTINUE = 0x21,
    ERequest_UNKNOWN_COMMAND = 0x81,
    ERequest_ENOMEM = 0x82,
    ERequest_NETWORK_ERROR = 0x86,
    ERequest_PENDING = 0xFF
};

class MemResult
{
    std::string mKey;
    ERequestStatus mErrorCode;

    boost::mutex mSyncMut;
    boost::condition_variable mSyncEvent;
    Buffer mValue;
public:
    typedef boost::shared_ptr<MemResult> Ptr;

    MEMCACHEDCLIENT_EXPORT MemResult(const std::string& key,const Buffer& buffer);

    MEMCACHEDCLIENT_EXPORT const Buffer& Value() const { return mValue; }

    MEMCACHEDCLIENT_EXPORT int ErrorCode( void ) const { return mErrorCode; }

    MEMCACHEDCLIENT_EXPORT const std::string& Key( void ) const { return mKey; }

    MEMCACHEDCLIENT_EXPORT ERequestStatus Finish( void );

    MEMCACHEDCLIENT_EXPORT static const std::string StrError(int error);

_internal:
    virtual bool FillReceiveBuffer(ERequestStatus status,VBuffer& bufs,int valueLen,int keyLen);
    virtual bool HasMoreResult( void ) { return false; }
    void Notify(ERequestStatus err);
};

class MemGetResult:public MemResult
{
    int mFlag;
public:
    typedef MemResult Base;
    typedef boost::shared_ptr<MemGetResult> Ptr;

    int Flag( void ) const { return mFlag; }

_internal:
    MemGetResult(const std::string& key,const Buffer& buffer);
    virtual bool FillReceiveBuffer(ERequestStatus status,VBuffer& bufs,int valueLen,int keyLen);
};

typedef MemResult MemSetResult;
typedef MemResult MemAddResult;
typedef MemResult MemDeleteResult;
typedef MemResult MemReplaceResult;
typedef MemResult MemAppendResult;
typedef MemResult MemPreAppendResult;

class MemIncResult:public MemResult
{
    uint64_t mValue;
public:
    typedef MemResult Base;
    typedef boost::shared_ptr<MemIncResult> Ptr;
    uint64_t Value(int i = 0) const { return mValue; }

_internal:
    MemIncResult(const std::string& key);
    virtual bool FillReceiveBuffer(ERequestStatus status,VBuffer& bufs,int valueLen,int keyLen);
};

typedef MemIncResult MemDecResult;

typedef MemResult MemVersionResult;
typedef MemResult MemFlushResult;

class MemStatResult:public MemResult
{
public:
    typedef MemResult Base;
    struct Stat
    {
	std::string Key;
	std::string Value;
    };
    typedef std::vector<Stat> StatsT;
    typedef boost::shared_ptr<MemStatResult> Ptr;

    const StatsT& Stats( void ) const { return mStats; }

_internal:
    MemStatResult(const std::string& key);
    virtual bool FillReceiveBuffer(ERequestStatus status,VBuffer& bufs,int valueLen,int keyLen);
    virtual bool HasMoreResult( void );

private:
    StatsT mStats;
    bool mHasMoreResult;
};

#endif
 
