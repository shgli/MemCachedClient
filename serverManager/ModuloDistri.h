#ifndef _MODULODISTRI_H
#define _MODULODISTRI_H
#include "DistributeAlgorithm.h"
class ModuloDistri:public DistributeAlgorithm
{
public:
    ModuloDistri();
    virtual void Add(ServerItem::Ptr& pServer);
    virtual void Remove(ServerItem::Ptr& pServer);
    virtual ServerItem::Ptr& Get(hash_t hash);
private:
    std::vector<ServerItem::Ptr> mServers;
};
#endif

