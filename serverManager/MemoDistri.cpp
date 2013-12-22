#include <algorithm>
#include "MemoDistri.h"
auto BalanceInfoCompare = [](const ServerBalanceInfo& lhs,const ServerBalanceInfo& rhs)
{
    return lhs.KeyCount * rhs.Server.Weight() < rhs.KeyCount * lhs.Server.Weight();
}

void MemoDistri::Add(ServerItem::Ptr pServer)
{
    mBalanceInfo.push_back(ServerBalanceInfo(pServer,0));
    std::push_heap(mBalanceInfo.begin(),mBalanceInfo.end());
}

void MemoDistri::Remove(ServerItem::Ptr pServer)
{
    auto const itServer = std::find_if(mBalanceInfo.begin()
	    ,mBalanceInfo.end()
	    ,[](const ServerBalanceInfo& info)
	    {
	        return info.Server == pServer;
	    });
    
    if(itServer != mBalanceInfo.end())
    {
	mBalanceInfo.erase(itServer);
    }
    
    std::vector<KeyServerMap::const_iterator> needRemoveList;
    for(auto const it : mKeyServerMapping)
    {
	if(it->second == pServer)
	{
	    needRemoveList.push_back(it);
	}
    }

    for(auto const rmIt : needRemoveList)
    {
	mKeyServerMapping.erase(rmIt);
    }
}

ServerItem::Ptr MemoDistri::Get(hash_t hash)
{
    auto const itServer = mKeyServerMapping.find(hash);
    if(itServer == mKeyServerMapping.end())
    {
	if(mBalanceInfo.size() > 0)
	{
	    auto result = mBalanceInfo.front().Server;
	    ++mBalanceInfo.front().KeyCount;
	    std::pop_head(mBalanceInfo.begin(),mBalanceInfo.end(),BalanceInfoCompare);
	    std::push_head(mBalanceInfo.begin(),mBalanceInfo.end(),BalanceInfoCompare);

	    mKeyServerMapping[hash] = result;
            return result;
	}

        return nullptr;
    }
    else
    {
	return itServer->second;
    }

}

