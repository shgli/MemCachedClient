#ifndef _MEMODISTRI_H
#define _MEMODISTRI_H
#include <boost/unordered/unordered_map.hpp>
#include "DistributeAlgorithm.h"
class MemoDistri:public DistributeAlgorithm
{
public:
    virtual void Add(ServerItem::Ptr& pServer);
    virtual void Remove(ServerItem::Ptr& pServer);
    virtual ServerItem::Ptr& Get(hash_t hash);

private:
    typedef boost::unordered_map<long,ServerItem::Ptr> KeyServerMap;
    KeyServerMap mKeyServerMapping;
    struct ServerBalanceInfo
    {
	ServerBalanceInfo(ServerItem::Ptr& server)
	    :Server(server)
	    ,KeyCount(0)
	{}

	ServerItem::Ptr Server;
	std::size_t KeyCount;

	friend bool operator < (const ServerBalanceInfo& lhs,const ServerBalanceInfo& rhs)
	{
            return lhs.KeyCount * rhs.Server->Weight() < rhs.KeyCount * lhs.Server->Weight();
	}
    };

    std::vector<ServerBalanceInfo> mBalanceInfos;

};
#endif

