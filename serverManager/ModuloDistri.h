#ifndef _MODULODISTRI_H
#define _MODULODISTRI_H
#include "serverManager/DistributeAlgorithm.h"
class ModuloDistri:public DistributeAlgorithm
{
public:
    ModuloDistri();

smgr_internal:
    virtual void Add(ServerItem::Ptr& pServer);
    virtual void Remove(ServerItem::Ptr& pServer);
    virtual ServerItem::Ptr Get(hash_t hash);
    virtual void Clear() {}

private:
    std::vector<ServerItem::Ptr> mServers;
};
#endif

