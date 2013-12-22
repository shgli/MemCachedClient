#ifndef _MEMCACHEDCLIENT_H
#define _MEMCACHEDCLIENT_H
#include <functional>
#include <atomic>
#include <boost/noncopyable.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/pool/object_pool.hpp>
#include "RequestItem.h"
#include "ServerList.h"
#include "MemLog.h"
class MemcachedClient
    :boost::noncopyable
{
public:
    explicit MemcachedClient(boost::asio::io_service& ioService);

    ServerList Servers;


    const MemGetResult::Ptr Get(const std::string& key,Callback callback = DefaultCallback);
    const MemGetResult::Ptr Get(const std::string& key,const Buffer& buf,Callback callback = DefaultCallback);

private:
    typedef std::unordered_map<int,RequestItem> RequestMap;

    void OnServerAdded(const ServerItem::Ptr& item);
    void OnServerRemoved(const ServerItem::Ptr& item);
    int  OnHeaderReaded(const void* header,VBuffer& body);
    void OnBodayReaded(const void* header,const VBuffer& boday);
    void FinishRequest(RequestMap::iterator,ERequestStatus err);

    static void DefaultCallback(const MemResult::Ptr&){}
    static void AdjustEndian(protocol_binary_response_header* response);
    static void AdjustEndian(protocol_binary_request_header* request);
private:

    boost::asio::io_service& mIoService;
    boost::pool<> mRequestPool;

    std::atomic_int mNextRequestId;


    RequestMap mRequests;

    std::mutex mSyncRequest;

    MemLog mLog;
};
#endif
 
