#include <string.h>
#include "MemcachedClient.h"
#include <memcached/protocol_binary.h>
MemcachedClient::MemcachedClient()
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
}

int MemcachedClient::OnHeaderReaded(void* const header,std::vector<SharedBuffer>& boday)
{
    ();
}

void MemcachedClient::OnBodayReaded(void* const header,const std::vector<SharedBuffer>& boday)
{

}

const MemResult& MemcachedClient::Get(const std::string& key,Callback callback)
{

}

const MemResult& MemcachedClient::Get(const std::string& key,Callback callback)
{
    auto itResult = mInWorkingResult.find(key);
    if(itResult != mInWorkingResult.end())
    {
	return itResult->second;
    }
    else
    {
	itResult = mInWorkingResult.insert(key,mResultPool.);
    }

    ServerItem& server = Servers.GetServer(key);

    size_t requestSize = sizeof(protocol_binary_request_get) + key.size();
    SharedBuffer buf(mRequestPool.malloc(requestSize),requestSize
	    ,[](const void* pData)
	    {
	        if(nullptr != pData)
		{
		    mRequestPool.free(pData);
		}
	    });

    auto& request = buf.GetHeader<protocol_binary_request_get>();
    request.request.magic = PROTOCOL_BINARY_REQ;
    request.request.opcode = PROTOCOL_BINARY_CMD_GET;
    request.request.keylen = key.size();
    request.request.extlen = 0;
    request.request.datatype = 0;
    request.request.reserved = 0;
    request.request.bodylen = key.size();
    request.request.opaque = 0;
    request.request.cas = 0;

    memcpy(buf.GetBody<protocol_binary_request_get>(),key.c_str(),key.size());
    server->Send(buf);
}
