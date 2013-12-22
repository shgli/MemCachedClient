#ifndef _DISTRIBUTEALGORITHM_H
#define _DISTRIBUTEALGORITHM_H
#include "ServerItem.h"

typedef size_t hash_t;
class DistributeAlgorithm
{
public:
    virtual ~DistributeAlgorithm() {}

    virtual void Add(ServerItem::Ptr& pServer) = 0;
    virtual void Remove(ServerItem::Ptr& pServer) = 0;
    virtual ServerItem::Ptr& Get(hash_t hash) = 0;
};
#endif

 
