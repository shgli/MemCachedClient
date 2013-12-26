#include <string.h>
#include <boost/make_shared.hpp>
#include "MemcachedClient.h"
#include "HostNetConversion.h"
#include <memcached/protocol_binary.h>
MemAppendResult::Ptr MemcachedClient::Append(const std::string& key,const ConstBuffer& value,Callback callback)
{
    return Append(PROTOCOL_BINARY_CMD_APPEND,key,value,callback);
}

MemPreAppendResult::Ptr MemcachedClient::PreAppend(const std::string& key,const ConstBuffer& value,Callback callback)
{
    return Append(PROTOCOL_BINARY_CMD_PREPEND,key,value,callback);
}

MemAppendResult::Ptr MemcachedClient::Append(uint8_t cmd,const std::string& key,const ConstBuffer& value,Callback callback)
{
    int requestId = mNextRequestId.fetch_add(1);

    auto server = Servers.Get(key);

    size_t requestSize = sizeof(protocol_binary_request_get) + key.size();

    MemAppendResult::Ptr result = boost::make_shared<MemAppendResult>(key,Buffer());
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
    request.request.opcode = cmd;
    request.request.keylen = key.size();
    request.request.extlen = 0;
    request.request.datatype = 0;
    request.request.reserved = 0;
    request.request.bodylen = key.size() + value.Size();
    request.request.opaque = requestId;
    request.request.cas = 0;
    AdjustEndian(&request);

    memcpy(requestBuf.GetBody<protocol_binary_request_append>(),static_cast<const void*>(key.c_str()),key.size());

    SVConstBuffer sbuf = boost::make_shared<VConstBuffer>();
    sbuf->push_back(requestBuf);
    sbuf->push_back(value);
    server->SendRequest(requestId,sbuf);

    return result;
}
