#include <string.h>
#include <boost/make_shared.hpp>
#include "MemcachedClient.h"
#include "HostNetConversion.h"
#include <memcached/protocol_binary.h>

MemcachedClient::MemcachedClient(boost::asio::io_service& ioService)
    :mIoService(ioService)
    ,mNextRequestId(0)
{
    Servers.OnServerAdded.connect(boost::bind(&MemcachedClient::OnServerAdded,this,_1));
    Servers.OnServerRemoved.connect(boost::bind(&MemcachedClient::OnServerRemoved,this,_1));
}

void MemcachedClient::OnServerAdded(const ServerItem::Ptr& item)
{
    (*item)->OnHeader = boost::bind(&MemcachedClient::OnHeaderReaded,this,_1,_2);
    (*item)->OnBody = boost::bind(&MemcachedClient::OnBodayReaded,this,_1,_2);
}

void MemcachedClient::OnServerRemoved(const ServerItem::Ptr& item)
{
    for(int request : item->PendingRequests())
    {
	FinishRequest(mRequests.find(request),ERequest_NETWORK_ERROR);
    }
}

bool MemcachedClient::OnHeaderReaded(void* header,VBuffer& body)
{
    protocol_binary_response_header* pHeader = static_cast<protocol_binary_response_header*>(header);
    assert(nullptr != pHeader);
    if(nullptr != pHeader)
    {
	AdjustEndian(pHeader);
	int requestId = pHeader->response.opaque;
	int valueLen = pHeader->response.bodylen - pHeader->response.extlen;

	auto requestIt = mRequests.find(requestId);
	if(mRequests.end() != requestIt)
	{
	    if(0 == valueLen)
	    {
		FinishRequest(requestIt,(ERequestStatus)pHeader->response.status);
		return false;
	    }
	    else
	    {
	        return requestIt->second.FillReceiveBuffer(body,valueLen);
	    }
	}
	else
	{
	    MEMLOG(mLog,Error) << "cannot find request " << requestId;
	    return false;
	}
    }
    else
    {
	MEMLOG(mLog,Error) << "readed header is null ";
	return false;
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
	    FinishRequest(requestIt,(ERequestStatus)pHeader->response.status);
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

void MemcachedClient::FinishRequest(RequestMap::iterator requestIt,ERequestStatus err)
{
    mIoService.post([this,requestIt,err]()
	    {
	        requestIt->second.Notify(err);
                mRequests.erase(requestIt);
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
    request.request.opcode = PROTOCOL_BINARY_CMD_GET;
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

