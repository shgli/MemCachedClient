#include <memory>
#include "ServerList.h"
#include "DistributeAlgorithm.h"
ServerList::ServerList(uint32_t replication)
    :mHashFunc(std::hash<std::string>())	
    ,mReplications(replication)
{}

bool ServerList::Add(const std::string& host,int port,boost::asio::io_service& ioService)
{
    std::string key = ServerItem::ToString(host,port);
    auto findResult = mServers.find(key);
    if(findResult != mServers.end())
    {
	return false;
    }
    else
    {
	auto itNewServer = mServers.insert(std::make_pair(key,std::make_shared<ServerItem>(ioService,host,port)));
	if(itNewServer.second)
	{
	    auto& serverItem = itNewServer.first->second;
	    (*serverItem)->OnError.connect([this,serverItem](TcpClient::ESocketError type,const system::error_code& ec)
		    {
		        Remove(serverItem->Host(),serverItem->Port());
		    }
		    );
	    mDistributeAlgorithm->Add(serverItem);
	    (*serverItem)->Connect(host,port);
	    OnServerAdded(serverItem);
	    return true;
	}
	return false;
    }
}

bool ServerList::Remove(const std::string& host,int port)
{
    std::string key = ServerItem::ToString(host,port);
    auto findResult = mServers.find(key);
    if(findResult != mServers.end())
    {
	mServers.erase(findResult);
	mDistributeAlgorithm->Remove(findResult->second);
	OnServerRemoved(findResult->second);
	return true;
    }
    return false;
}

ServerItem::Ptr& ServerList::Get(const std::string& key)
{
    return mDistributeAlgorithm->Get(mHashFunc(key));
}

void ServerList::SetDistributeAlgorithm(DistributeAlgorithm* algorithm)
{
    assert(nullptr != algorithm);
    mDistributeAlgorithm.reset(algorithm);
    for(auto& serverPair : mServers)
    {
	mDistributeAlgorithm->Add(serverPair.second);
    }
}

