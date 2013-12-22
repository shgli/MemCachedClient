#ifndef _MEMRESULT_H
#define _MEMRESULT_H
#include <mutex>
#include <condition_variable>
#include <memory>
#include <boost/pool/pool.hpp>
#include <memcached/protocol_binary.h>
#include "MemcachedCommon.h"
#include "SharedBuffer.h"
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

    std::mutex mSyncMut;
    std::condition_variable mSyncEvent;
    Buffer mValue;
public:
    typedef std::shared_ptr<MemResult> Ptr;

    MemResult(const std::string& key,const Buffer& buffer);

    const Buffer& Value() const { return mValue; }

    int ErrorCode( void ) const { return mErrorCode; }

    const std::string& Key( void ) const { return mKey; }

    ERequestStatus Finish( void );

    static const std::string StrError(int error);

//internal:
    virtual bool FillReceiveBuffer(VBuffer& bufs,int valueLen);
    void Notify(ERequestStatus err);
};

class MemGetResult:public MemResult
{
    typedef MemResult Base;
    int mFlag;
public:
    typedef std::shared_ptr<MemGetResult> Ptr;
    MemGetResult(const std::string& key,const Buffer& buffer);

    int Flag( void ) const { return mFlag; }

    virtual bool FillReceiveBuffer(VBuffer& bufs,int valueLen);
};
#endif
 
