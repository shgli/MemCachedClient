#include <boost/make_shared.hpp>
#include <cstdint>
#include <memcached/protocol_binary.h>
#include "MemcachedClient.h"

MemDeleteResult::Ptr MemcachedClient::Delete(const std::string& key,Callback callback)
{
    int requestId = mNextRequestId.fetch_add(1);

    auto server = Servers.Get(key);


    MemDeleteResult::Ptr result = boost::make_shared<MemDeleteResult>(key,Buffer());
    MemResult::Ptr baseResult = result;
    mRequests.insert(std::make_pair(requestId,RequestItem(callback,baseResult)));

    size_t requestSize = sizeof(protocol_binary_request_delete) + key.size();
    ConstBuffer requestBuf(malloc(requestSize)
        ,requestSize
        ,[this](void* pData)
    {
        free(pData);
    });
    auto& request = requestBuf.GetHeader<protocol_binary_request_header>();
    request.request.magic = PROTOCOL_BINARY_REQ;
    request.request.opcode = PROTOCOL_BINARY_CMD_DELETE;
    request.request.keylen = key.size();
    request.request.extlen = 0;
    request.request.datatype = 0;
    request.request.reserved = 0;
    request.request.bodylen = key.size();
    request.request.opaque = requestId;
    request.request.cas = 0;
    AdjustEndian(&request);

    memcpy(requestBuf.GetBody<protocol_binary_request_delete>(),static_cast<const void*>(key.c_str()),key.size());
    server->SendRequest(requestId,requestBuf);

    return result;
}

