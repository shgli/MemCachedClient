#include <string.h>
#include "MemcachedClient.h"
#include <memcached/protocol_binary.h>
MemcachedClient::MemcachedClient(boost::asio::io_service& ioService)
    :mIoService(ioService)
    ,mRequestPool(64)
{
    Servers.OnServerAdded.connect(std::bind(&MemcachedClient::OnServerAdded,this));
    Servers.OnServerRemoved.connect(std::bind(&MemcachedClient::OnServerRemoved,this));
}

void MemcachedClient::OnServerAdded(const ServerItem::Ptr& item)
{
    (*item)->OnHeader = std::bind(&MemcachedClient::OnHeaderReaded,this);
    (*item)->OnBody = std::bind(&MemcachedClient::OnBodayReaded,this);
}

void MemcachedClient::OnServerRemoved(const ServerItem::Ptr& item)
{
    //item->OnHeader.disconnect(&MemcachedClient::OnHeaderReaded,this);
    //item->OnBody.disconnect(&MemcachedClient::OnBodayReaded,this);

    for(int request : item->PendingRequests())
    {
	FinishRequest(mRequests.find(request),ERequest_NETWORK_ERROR);
    }
}

int MemcachedClient::OnHeaderReaded(const void* header,VBuffer& body)
{
    const protocol_binary_response_header* pHeader = static_cast<const protocol_binary_response_header*>(header);
    assert(nullptr != pHeader);
    if(nullptr != pHeader)
    {
	int requestId = pHeader->response.opaque;
	int valueLen = pHeader->response.bodylen - pHeader->response.extlen;

	auto requestIt = mRequests.find(requestId);
	if(mRequests.end() != requestIt)
	{
	    if(0 == valueLen)
	    {
		FinishRequest(requestIt,(ERequestStatus)pHeader->response.status);
		return 0;
	    }
	    else
	    {
	        return requestIt->second.FillReceiveBuffer(body,valueLen);
	    }
	}
	else
	{
	    MEMLOG(mLog,Error) << "cannot find request " << requestId;
	    return 0;
	}
    }
    else
    {
	MEMLOG(mLog,Error) << "readed header is null ";
	return 0;
    }
}

void MemcachedClient::OnBodayReaded(const void* header,const VBuffer& boday)
{
    const protocol_binary_response_header* pHeader = static_cast<const protocol_binary_response_header*>(header);
    assert(nullptr != pHeader);
    if(nullptr != pHeader)
    {
	int requestId = pHeader->response.opaque;

	auto requestIt = mRequests.find(requestId);
	if(mRequests.end() != requestIt)
	{
	    FinishRequest(requestId,(ERequestStatus)pHeader->response.status);
	}
	else
	{
	    MEMLOG(mLog,Error) << "cannot find request " << requestId;
	}
    }
    else
    {
	MEMLOG(mLog,Error) << "readed header is null ";
    }

}

void MemcachedClient::FinishRequest(RequestMap::iterator,ERequestStatus err)
{
    mRequests.erase(requestIt);
    mIoService.post([requestIt,err]()
	    {
	        requestIt->second->Notify(err);
	    });
}

/*static*/void MemcachedClient::AdjustEndian(protocol_binary_request_header* header)
{
    auto & request = header->request;
    request.keylen = htons(request.keylen);
    request.bodylen = htonl(request.bodylen);
    request.cas = htonll(request.cas);
}

/*static*/void MemcachedClient::AdjustEndian(protocol_binary_response_header* header)
{
    auto& response = header->response;
    response.keylen = ntohs(response.keylen);
    response.bodylen = ntohl(response.bodylen);
    response.cas = ntohll(response.cas);
    response.status = ntohs(response.status);
}

const MemGetResult::Ptr MemcachedClient::Get(const std::string& key,const Buffer& buf,Callback callback)
{
    int requestId = mNextRequestId.fetch_and_add(1);

    auto server = Servers.Get(key);

    size_t requestSize = sizeof(protocol_binary_request_get) + key.size();

    auto result = std::make_shared<MemGetResult>(key,buf);
    mRequests.insert(std::make_pair(requestId,RequestItem(callback,result)));

    ConstBuffer buf(mRequestPool.malloc(requestSize),requestSize
		,[this](void* pData)
		{
		    if(nullptr != pData)
		    {
			mRequestPool.free(pData);
		    }
		});
    auto& request = buf.GetHeader<protocol_binary_request_header>();
    request.request.magic = PROTOCOL_BINARY_REQ;
    request.request.opcode = PROTOCOL_BINARY_CMD_GET;
    request.request.keylen = key.size();
    request.request.extlen = 0;
    request.request.datatype = 0;
    request.request.reserved = 0;
    request.request.bodylen = key.size();
    request.request.opaque = requestId;
    request.request.cas = 0;
    AdjustEndian(&request);

    memcpy(&buf.GetBody<protocol_binary_request_get>(),key.c_str(),key.size());
    server->SendRequest(requestId,buf);

    return result;
}

const MemGetResult::Ptr MemcachedClient::Get(const std::string& key,Callback callback)
{
    return Get(key,Buffer(),callback);
}

