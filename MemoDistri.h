#ifndef _MEMODISTRI_H
#define _MEMODISTRI_H
#include <memory>
#include "DistributeAlgorithm.h"
class MemoDistri:public DistributeAlgorithm
{
public:
    virtual void Add(ServerItem::Ptr pServer);
    virtual void Remove(ServerItem::Ptr pServer);
    virtual ServerItem::Ptr Get(hash_t hash);

private:
    typedef std::unordered_map<long,ServerItem::Ptr> KeyServerMap;
    KeyServerMap mKeyServerMapping;
    struct ServerBalanceInfo
    {
	ServerItem::Ptr Server;
	std::size_t KeyCount;
    };

    std::vector<ServerBalanceInfo> mBalanceInfos;

};
#endif

