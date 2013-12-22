#ifndef _SERVERLIST_H
#define _SERVERLIST_H
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include <boost/signals2.hpp>
#include <boost/asio/io_service.hpp>
#include "ServerItem.h"
#include "DistributeAlgorithm.h"

typedef size_t hash_t;
using namespace boost;
class ServerList
{
public:
    typedef std::function<hash_t (const std::string&)> HashFunc;
    typedef signals2::signal<void (const ServerItem::Ptr&)> ServerEvent;

    ServerEvent OnServerAdded;
    ServerEvent OnServerRemoved;

public:
    ServerList(uint32_t replication = 200);

    bool Add(const std::string& host,int port,boost::asio::io_service& ioService);
    bool Remove(const std::string& host,int port);

    ServerItem::Ptr& Get(const std::string& key);
    size_t Count( void ) { return mServers.size(); }

    void SetDistributeAlgorithm(DistributeAlgorithm* algorithm);
    void SetHashAlogrithm(HashFunc hFunc) { mHashFunc = hFunc; }

    const std::unordered_map<std::string,ServerItem::Ptr> Items( void ) const { return mServers; }
private:
    std::unordered_map<std::string,ServerItem::Ptr> mServers;
    std::unique_ptr<DistributeAlgorithm> mDistributeAlgorithm;
    HashFunc mHashFunc;
    uint32_t mReplications;
};
#endif
 
