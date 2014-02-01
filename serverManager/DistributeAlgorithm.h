#ifndef _DISTRIBUTEALGORITHM_H
#define _DISTRIBUTEALGORITHM_H
#include "serverManager/ServerItem.h"
#include "serverManager/InternalMacros.h"
typedef size_t hash_t;
class SERVERMGR_EXPORT DistributeAlgorithm
{
public:
    typedef boost::shared_ptr<DistributeAlgorithm> Ptr;
    virtual ~DistributeAlgorithm() {}

smgr_internal:
    virtual void Add(ServerItem::Ptr& pServer) = 0;
    virtual void Remove(ServerItem::Ptr& pServer) = 0;
    virtual ServerItem::Ptr Get(hash_t hash) = 0;
    virtual void Clear( void ) = 0;
};
#endif

 
