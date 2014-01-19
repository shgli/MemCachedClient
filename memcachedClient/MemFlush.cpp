#include <cstdint>
#include <boost/make_shared.hpp>
#include <memcached/protocol_binary.h>
#include "MemcachedClient.h"
MemFlushResult::Ptr MemcachedClient::Flush(ServerItem::Ptr server,uint32_t expiry,Callback callback)
{
    int requestId = mNextRequestId.fetch_add(1);


    MemFlushResult::Ptr result = boost::make_shared<MemFlushResult>("flush",Buffer());
    MemResult::Ptr baseResult = result;
    mRequests.insert(std::make_pair(requestId,RequestItem(callback,baseResult)));

    size_t requestSize = sizeof(protocol_binary_request_flush);
    ConstBuffer requestBuf(malloc(requestSize)
	    ,requestSize
	    ,[this](void* pData)
	    {
	        free(pData);
	    });
    auto& request = requestBuf.GetHeader<protocol_binary_request_header>();
    request.request.magic = PROTOCOL_BINARY_REQ;
    request.request.opcode = PROTOCOL_BINARY_CMD_FLUSH;
    request.request.keylen = 0;
    request.request.extlen = 0;
    request.request.datatype = 0;
    request.request.reserved = 0;
    request.request.bodylen = 0;
    request.request.opaque = requestId;
    request.request.cas = 0;
    AdjustEndian(&request);

    auto& flushRequest = requestBuf.GetHeader<protocol_binary_request_flush>();
    flushRequest.message.body.expiration = htonl(expiry);

    server->SendRequest(requestId,requestBuf);

    return result;
}
