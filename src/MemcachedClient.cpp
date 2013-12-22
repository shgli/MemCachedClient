#include <string.h>
#include "MemcachedClient.h"
#include <memcached/protocol_binary.h>
MemcachedClient::MemcachedClient(boost::asio::io_service& ioService)
    :mIoService(ioService)
{
    Servers.OnServerAdded.connect(&MemcachedClient::OnServerAdded,this);
    Servers.OnServerRemoved.connect(&MemcachedClient::OnServerRemoved,this);
}

void MemcachedClient::OnServerAdded(const ServerItem& item)
{
    item->OnHeader.connect(&MemcachedClient::OnHeaderReaded,this);
    item->OnBody.connect(&MemcachedClient::OnBodayReaded,this);
}

void MemcachedClient::OnServerRemoved(const ServerItem& item)
{
    item->OnHeader.disconnect(&MemcachedClient::OnHeaderReaded,this);
    item->OnBody.disconnect(&MemcachedClient::OnBodayReaded,this);

    for(int request : item.PendingRequests())
    {
	FinishRequest(mRequests.find(request),ERequest_NETWORK_ERROR);
    }
}

int MemcachedClient::OnHeaderReaded(void* const header,std::vector<SharedBuffer>& body)
{
    const protocol_binary_response_header* pHeader = static_cast<protocol_binary_response_header*>(header);
    assert(nullptr != pHeader);
    if(nullptr != pHeader)
    {
	int requestId = pHeader->response.opaque;
	int valueLen = pHeader->request.bodylen - pHeader->response->extlen;

	auto requestIt = mRequests.find(request);
	if(mRequests.end() != requestIt)
	{
	    if(0 == valueLen)
	    {
		FinishRequest(pHeader->response.status);
		return 0;
	    }
	    else
	    {
	        return requestIt->second.FillBuffer(body);
	    }
	}
	else
	{
	    LOG(mLog,Error) << "cannot find request " << requestId;
	    return 0;
	}
    }
    else
    {
	LOG(mLog,Error) << "readed header is null ";
	return 0;
    }
}

void MemcachedClient::OnBodayReaded(void* const header,const std::vector<SharedBuffer>& boday)
{
    const protocol_binary_response_header* pHeader = static_cast<protocol_binary_response_header*>(header);
    assert(nullptr != pHeader);
    if(nullptr != pHeader)
    {
	int requestId = pHeader->response.opaque;

	auto requestIt = mRequests.find(request);
	if(mRequests.end() != requestIt)
	{
	    FinishRequest(requestId,pHeader->response.status);
	}
	else
	{
	    LOG(mLog,Error) << "cannot find request " << requestId;
	}
    }
    else
    {
	LOG(mLog,Error) << "readed header is null ";
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

/*static*/void MemcachedClient::AdjustEndian(const protocol_binary_request_header* request)
{
    request->keylen = htons(request->keylen);
    request->bodylen = htonl(request->bodylen);
    request->cas = htonll(request->cas);
}

/*static*/void MemcachedClient::AdjustEndian(const protocol_binary_response_header* response)
{
    response->keylen = ntohs(response->keylen);
    response->bodylen = ntohl(request->bodylen);
    response->cas = ntohll(request->cas);
    response->status = ntohs(request->status);
}

const MemGetResult::Ptr MemcachedClient::Get(const std::string& key,const SharedBuffer& buf,Callback callback)
{
    int requestId = mNextRequestId.fetch_and_add(1);

    ServerItem& server = Servers.GetServer(key);

    size_t requestSize = sizeof(protocol_binary_request_get) + key.size();

    auto result = std::make_shared<MemGetResult>(key,buf);
    mRequests.insert(std::make_pair(requestId,RequestItem(callback,result)));

    SharedBuffer buf(mRequestPool.malloc(requestSize),requestSize
		,[](const void* pData)
		{
		    if(nullptr != pData)
		    {
			mRequestPool.free(pData);
		    }
		});
    auto& request = buf.GetHeader<protocol_binary_request_get>().request;
    request.magic = PROTOCOL_BINARY_REQ;
    request.opcode = PROTOCOL_BINARY_CMD_GET;
    request.keylen = key.size();
    request.extlen = 0;
    request.datatype = 0;
    request.reserved = 0;
    request.bodylen = key.size();
    request.opaque = requestId;
    request.cas = 0;
    AdjustEndian(&request);

    memcpy(buf.GetBody<protocol_binary_request_get>(),key.c_str(),key.size());
    server.SendRequest(requestId,request.request.opaque,buf);

    return result;
}

const MemGetResult::Ptr MemcachedClient::Get(const std::string& key,Callback callback)
{
    return Get(key,SharedBuffer(),callback);
}
