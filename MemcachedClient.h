#ifndef _MEMCACHEDCLIENT_H
#define _MEMCACHEDCLIENT_H
#include <functionl>
#include <atomic>
#include <boost/noncopyable.hpp>
#include <boost/pool/object_pool.hpp>
#include "RequestItem.h"
#include "MemLog.h"
class MemcachedClient
    :boost::noncopyable
{
public:
    explicit MemcachedClient(boost::asio::io_service& ioService);

    ServerList Servers;

    typedef std::function<void (const MemResult::Ptr&)> Callback;

    const MemGetResult::Ptr Get(const std::string& key,Callback callback = DefaultCallback);
    const MemGetResult::Ptr Get(const std::string& key,const SharedBuffer& buf,Callback callback = DefaultCallback);

private:
    void OnServerAdded(const ServerItem& item);
    void OnServerRemoved(const ServerItem& item);
    int  OnHeaderReaded(void* const header,std::vector<SharedBuffer>& body);
    void OnBodayReaded(void* const header,const std::vector<SharedBuffer>& boday);
    void FinishRequest(RequestMap::iterator,ERequestStatus err);

    static void DefaultCallback(const MemResult::Ptr&){}

private:

    boost::asio::io_service& mIoService;
    boost::pool<> mRequestPool;

    std::atomic_int mNextRequestId;

    typedef std::unordered_map<int,RequestItem::Ptr> RequestMap;
    RequestMap mRequests;

    std::mutex mSyncRequest;

    MemLog mLog;
};
#endif
 
