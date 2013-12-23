#include <string.h>
#include <boost/make_shared.hpp>
#include "MemcachedClient.h"
#include "HostNetConversion.h"
#include <memcached/protocol_binary.h>
MemGetResult::Ptr MemcachedClient::Get(const std::string& key,const Buffer& buf,Callback callback)
{
    int requestId = mNextRequestId.fetch_add(1);

    auto server = Servers.Get(key);

    size_t requestSize = sizeof(protocol_binary_request_get) + key.size();

    MemGetResult::Ptr result = boost::make_shared<MemGetResult>(key,buf);
    MemResult::Ptr baseResult = result;
    mRequests.insert(std::make_pair(requestId,RequestItem(callback,baseResult)));

    ConstBuffer requestBuf(malloc(requestSize)
	    ,requestSize
	    ,[this](void* pData)
	    {
	        free(pData);
	    });
    auto& request = requestBuf.GetHeader<protocol_binary_request_header>();
    request.request.magic = PROTOCOL_BINARY_REQ;
    request.request.opcode = PROTOCOL_BINARY_CMD_GETQ;
    request.request.keylen = key.size();
    request.request.extlen = 0;
    request.request.datatype = 0;
    request.request.reserved = 0;
    request.request.bodylen = key.size();
    request.request.opaque = requestId;
    request.request.cas = 0;
    AdjustEndian(&request);

    memcpy(requestBuf.GetBody<protocol_binary_request_get>(),static_cast<const void*>(key.c_str()),key.size());
    server->SendRequest(requestId,requestBuf);

    return result;
}

MemGetResult::Ptr MemcachedClient::Get(const std::string& key,Callback callback)
{
    return Get(key,Buffer(),callback);
}
