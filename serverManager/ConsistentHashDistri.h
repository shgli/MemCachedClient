#ifndef _cONSISTENThASHdISTRI_h
#define _cONSISTENThASHdISTRI_h
#include "DistributeAlgorithm.h"
#include "ServerItem.h"
class ConsistentHashDistri
    :public DistributeAlgorithm
{
public:
    typedef std::map<hash_t,ServerItem::Ptr> CycleType;
    virtual void Add(ServerItem::Ptr pServer);
    virtual void Remove(ServerItem::Ptr pServer);
    virtual ServerItem::Ptr Get(hash_t hash);

private:
    hash_t Hash(const std::string& md5Value,int offset);
    CycleType mCircle;
};
#endif

