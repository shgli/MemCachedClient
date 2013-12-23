#include <memcached/protocol_binary.h>
#include "MemcachedClient.h"
MemSetResult::Ptr MemcachedClient::Store(uint8_t cmd
	,const std::string& key
	,uint32_t flag
	,uint32_t expiry
	,const ConstBuffer& buf
	,Callback callback)
{
    int requestId = mNextRequestId.fetch_add(1);

    auto server = Servers.Get(key);


    MemSetResult::Ptr result = boost::make_shared<MemSetResult>(key,buf);
    MemResult::Ptr baseResult = result;
    mRequests.insert(std::make_pair(requestId,RequestItem(callback,baseResult)));

    size_t requestSize = sizeof(protocol_binary_request_set) + key.size();
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
    request.request.bodylen = key.size() + buf.Size();
    request.request.opaque = requestId;
    request.request.cas = 0;
    AdjustEndian(&request);

    auto& setRequest = requestBuf.GetHeader<protocol_binary_request_set>();
    setRequest.message.body.flags = htonl(flag);
    setRequest.message.body.expiration = htonl(expiry);

    memcpy(requestBuf.GetBody<protocol_binary_request_get>(),static_cast<const void*>(key.c_str()),key.size());

    ConstVBuffer buf;
    buf.push_back(requestBuf);
    buf.push_back(buf);
    server->SendRequest(requestId,buf);

    return result;
}

MemSetResult::Ptr MemcachedClient::Set(const std::string& key,uint32_t flag,uint32_t expiry,const ConstBuffer& buf,Callback callback)
{
    return Store(PROTOCOL_BINARY_CMD_SET,key,flag,expiry,buf,callback);
}

MemAddResult::Ptr MemcachedClient::Add(const std::string& key,uint32_t flag,uint32_t expiry,const ConstBuffer& buf,Callback callback)
{
    return Store(PROTOCOL_BINARY_CMD_ADD,key,flag,expiry,buf,callback);
}

MemReplaceResult::Ptr MemcachedClient::Replace(const std::string& key,uint32_t flag,uint32_t expiry,const ConstBuffer& buf,Callback callback)
{
    return Store(PROTOCOL_BINARY_CMD_REPLACE,key,flag,expiry,buf,callback);
}

