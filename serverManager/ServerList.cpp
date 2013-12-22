#include "ServerList.h"
ServerList::ServerList(uint32_t replication)
    :mHashFunc(std::hash<std::string>())	
    ,mReplications(replication)
{}

bool ServerList::Add(const std::string& host,int port,boost::io_service& ioService)
{
    std::string key = ServerItem::ToString(host,port);
    auto findResult = mServers.find(key);
    if(findResult != mServers.end())
    {
	return false;
    }
    else
    {
	auto itNewServer = mServers.insert(std::make_pair(key,std::make_shared(new ServerItem(ioService,host,port))));
	if(itNewServer->second)
	{
	    itNewServer->OnError.connect([](TcpSocket::ESocketError type,system::error_code ec)
		    {
		        Remove(itNewServer->second->Host(),itNewServer->second.Port());
		    }
		    );
	    mDistributeAlgorithm->Add(*itNewServer->second);
	    OnServerAdded(*itNewServer->first);
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
	mDistributeAlgorithm->Remove(*findResult);
	OnServerRemoved(*findResult);
	return true;
    }
    return false;
}

ServerItem& ServerList::Get(const std::string& key)
{
    return mDistributeAlgorithm->Get(mHashFunc(key));
}

void ServerList::SetDistributeAlgorithm(std::unique_ptr<DistributeAlgorithm> algorithm)
{
    assert(nullptr != algorithm);
    mDistributeAlgorithm = std::move(algorithm);
    for(auto itServerPair : mServers.begin())
    {
	mDistributeAlgorithm->Add(itServerPair->second);
    }
}

