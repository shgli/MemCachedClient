#ifndef _MEMCACHEDCLIENT_H
#define _MEMCACHEDCLIENT_H
#include <boost/function.hpp>
#include <boost/pool/object_pool.hpp>
#include <boost/unordered/unordered_map.hpp>
class MemcachedClient
{
public:
    ServerList Servers;

    typedef boost::function<void (const MemResult)> Callback;

    const MemResult& Get(const std::string& key,Callback callback = DefaultCallback);
    
private:
    static void DefaultCallback(const MemResult&){}

    boost::object_pool<MemResult> mResultPool;
    boost::pool<> mRequestPool;
    boost::unordered_map<std::string,MemResult> mInWorkingResult;
};
#endif
 
