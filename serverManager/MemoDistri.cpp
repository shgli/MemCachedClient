#include <algorithm>
#include "serverManager/MemoDistri.h"

void MemoDistri::Add(ServerItem::Ptr& pServer)
{
    mBalanceInfos.push_back(ServerBalanceInfo(pServer));
    std::push_heap(mBalanceInfos.begin(),mBalanceInfos.end());
}

void MemoDistri::Remove(ServerItem::Ptr& pServer)
{
    auto const itServer = std::find_if(mBalanceInfos.begin()
        ,mBalanceInfos.end()
        ,[&pServer](const ServerBalanceInfo& info)
    {
        return info.Server == pServer;
    });

    if(itServer != mBalanceInfos.end())
    {
        mBalanceInfos.erase(itServer);
    }

    std::vector<KeyServerMap::const_iterator> needRemoveList;
    for(auto it = mKeyServerMapping.begin(); it != mKeyServerMapping.end(); ++it)
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
    assert(0 !=   mBalanceInfos.size());
    if(0 == mBalanceInfos.size())
    {
        return ServerItem::Ptr(nullptr);
    }

    auto const itServer = mKeyServerMapping.find(hash);
    if(itServer == mKeyServerMapping.end())
    {
        assert(0 != mBalanceInfos.size());
        if(mBalanceInfos.size() > 0)
        {
            auto& result = mBalanceInfos.front().Server;
            ++mBalanceInfos.front().KeyCount;

            auto BalanceInfoCompare = [](const ServerBalanceInfo& lhs,const ServerBalanceInfo& rhs)
            {
                return lhs.KeyCount * rhs.Server->Weight() < rhs.KeyCount * lhs.Server->Weight();
            };

            std::pop_heap(mBalanceInfos.begin(),mBalanceInfos.end(),BalanceInfoCompare);
            std::push_heap(mBalanceInfos.begin(),mBalanceInfos.end(),BalanceInfoCompare);

            mKeyServerMapping[hash] = result;
            return result;
        }
    }
    else
    {
        return itServer->second;
    }

    return nullptr;
}

void MemoDistri::Clear()
{
    mKeyServerMapping.clear();
    mBalanceInfos.clear();
}

