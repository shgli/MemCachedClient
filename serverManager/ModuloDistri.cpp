#include "ModuloDistri.h"
void ModuloDistri::Add(ServerItem::Ptr& pServer)
{
    mServers.push_back(pServer);
}

void ModuloDistri::Remove(ServerItem::Ptr& pServer)
{
    auto itRm = std::find_if(mServers.begin()
	    ,mServers.end()
	    ,[&pServer](ServerItem::Ptr server)
	    {
	        return server == pServer;
	    });
    if(itRm != mServers.end())
    {
	mServers.erase(itRm);
    }
}

ServerItem::Ptr& ModuloDistri::Get(hash_t hash)
{
    assert(0 != mServers.size());
    if(0 != mServers.size())
    {
        return mServers[hash % mServers.size()];
    }
}

