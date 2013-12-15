#ifndef _SERVERLIST_H
#define _SERVERLIST_H
#include <string>
#include <boost/signals2.hpp>
#include <boost/function.hpp>
#include <boost/unordered/unordered_map.hpp>
using namespace boost;
class ServerItem;
class ServerList
{
public:
    typedef signals2::signal<function<void (const ServerItem&)> > ServerEvent;
    ServerEvent OnServerAdded;
    ServerEvent OnServerRemoved;

public:
    void Add(const std::string& host,int port);
    void Remove(const std::string& host);

    ServerItem& Get(const std::string& key);
    size_t Count( void ) { return mServers.size(); }

private:
    unordered_map<std::string,ServerItem> mServers;
};
#endif
 
