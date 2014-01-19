#include <cassert>
#include <boost/lexical_cast.hpp>
#include "serverManager/ConsistentHashDistri.h"
#include "utils/md5.h"
ConsistentHashDistri::ConsistentHashDistri(int replis)
    :mReplications(replis)
{}

void ConsistentHashDistri::Add(ServerItem::Ptr& pServer)
{
    int totalReplications = (mReplications*pServer->Weight()) & 0x8FFFFFF4;
    for(int i = 0; i < totalReplications / 4; ++i)
    {
        MD5 md5(pServer->ToString() + boost::lexical_cast<std::string>(i));
        auto md5Value = md5.toString();

        for(int h = 0; h < 4; ++h)
        {
            hash_t hashValue = Hash(md5Value,h);
            mCircle[hashValue] = pServer;
        }
    }
}

hash_t ConsistentHashDistri::Hash(const std::string& md5Value,int offset)
{
    int startIndex = offset * 4;
    return (md5Value[0 + startIndex] & 0xFF)
        | ((md5Value[1 + startIndex] & 0xFF) << 8)
        | ((md5Value[2 + startIndex] & 0xFF) << 16)
        | ((md5Value[3 + startIndex] & 0xFF) << 24);
}

void ConsistentHashDistri::Remove(ServerItem::Ptr& pServer)
{
    std::vector<CycleType::const_iterator> needRemoveList;
    for(auto it = mCircle.begin();
        it != mCircle.end();
        ++it)
    {
        if(it->second == pServer)
        {
            needRemoveList.push_back(it);
        }
    }

    for(auto const rmIt : needRemoveList)
    {
        mCircle.erase(rmIt);
    }
}

ServerItem::Ptr ConsistentHashDistri::Get(hash_t hash)
{
    assert(0 != mCircle.size());

    if(0 !=  mCircle.size())
    {
        auto result = mCircle.find(hash);
        if(result == mCircle.end())
        {
            result = mCircle.upper_bound(hash);
            if(result == mCircle.end())
            {
                result = mCircle.begin();
            }
        }
        return result->second;
    }
    return ServerItem::Ptr(nullptr);
}

