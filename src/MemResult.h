#ifndef _MEMRESULT_H
#define _MEMRESULT_H
#include <thread>
#include <memory>
#include <boost/pool/pool.hpp>
#include <memcached/protocol_binary.h>
#include "MemcachedCommon.h"
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
    static boost::pool<> gValuePool;
    std::string mKey;
    int mErrorCode;

    std::mutex mSyncMut;
    std::condition_variable mSyncEvent;
    SharedBuffer mValue;
public:
    typedef std::shared_ptr<MemResult> Ptr;

    MemResult(const std::string& key,const SharedBuffer& buffer);

    const SharedBuffer& Value() const { return mValue; }

    int ErrorCode( void ) const { return mErrorCode; }

    const std::string& Key( void ) const { return mKey; }

    int Finish( void ) const;

    static const std::string StrError(int error);

internal:
    virtual void FillReceiveBuffer(VBuffer& bufs,int valueLen);
    void Notify(Request_Status err);
};

class MemGetResult:public MemResult
{
    typedef MemResult Base;
    int mFlag;
public:
    typedef std::shared_ptr<MemGetResult> Ptr;
    MemGetResult(const std::string& key,const SharedBuffer& buffer);

    int Flag( void ) const { return mFlag }

    virtual void FillReceiveBuffer(VBuffer& bufs,int valueLen);
};
#endif
 
