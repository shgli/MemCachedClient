#include <cstdint>
#include <boost/make_shared.hpp>
#include <memcached/protocol_binary.h>
#include "HostNetConversion.h"
#include "MemcachedClient.h"
MemIncResult::Ptr MemcachedClient::SelfOp(uint8_t cmd,const std::string& key,uint64_t delta,uint64_t initValue,uint32_t expiry,Callback callback)
{
    int requestId = mNextRequestId.fetch_add(1);

    auto server = Servers.Get(key);


    MemIncResult::Ptr result = boost::make_shared<MemIncResult>(key);
    MemResult::Ptr baseResult = result;
    mRequests.insert(std::make_pair(requestId,RequestItem(callback,baseResult)));

    size_t requestSize = sizeof(protocol_binary_request_incr) + key.size();
    ConstBuffer requestBuf(malloc(requestSize)
	    ,requestSize
	    ,[this](void* pData)
	    {
	        free(pData);
	    });
    auto& request = requestBuf.GetHeader<protocol_binary_request_header>();
    request.request.magic = PROTOCOL_BINARY_REQ;
    request.request.opcode = cmd;
    request.request.keylen = key.size();
    request.request.extlen = 0;
    request.request.datatype = 0;
    request.request.reserved = 0;
    request.request.bodylen = key.size();
    request.request.opaque = requestId;
    request.request.cas = 0;
    AdjustEndian(&request);

    auto& incRequest = requestBuf.GetHeader<protocol_binary_request_incr>();
    incRequest.message.body.delta = htonll(delta);
    incRequest.message.body.initial = htonll(initValue);
    incRequest.message.body.expiration = htonl(expiry);

    memcpy(requestBuf.GetBody<protocol_binary_request_incr>(),static_cast<const void*>(key.c_str()),key.size());
    server->SendRequest(requestId,requestBuf);

    return result;

}

MemIncResult::Ptr MemcachedClient::Increment(const std::string& key,uint64_t delta,uint64_t initValue,uint32_t expiry,Callback callback)
{
    return SelfOp(PROTOCOL_BINARY_CMD_INCREMENT,key,delta,initValue,expiry,callback);
}

MemDecResult::Ptr MemcachedClient::Decrement(const std::string& key,uint64_t delta,uint64_t initValue,uint32_t expiry,Callback callback)
{
    return SelfOp(PROTOCOL_BINARY_CMD_DECREMENT,key,delta,initValue,expiry,callback);
}

