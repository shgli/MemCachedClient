#include <boost/make_shared.hpp>
#include <boost/thread/lock_types.hpp>
#include "serverManager/ServerList.h"
#include "serverManager/DistributeAlgorithm.h"
#include "log/LogManager.h"
SERVERMGR_EXPORT ServerList::ServerList(uint32_t replication)
    :mHashFunc(std::hash<std::string>())	
    ,mReplications(replication)
    ,mLog(LogManager::Instance().GetLogger("serverManager"))
{}

SERVERMGR_EXPORT bool ServerList::Add(const std::string& host,int port,boost::asio::io_service& ioService)
{
    boost::unique_lock<boost::mutex> lock(mMutex);
    std::string key = ServerItem::ToString(host,port);
    auto findResult = mServers.find(key);
    if(findResult != mServers.end())
    {
        WARN(mLog)<<"Server "<<host<<":"<<port<<" already exists in server list.";
        return false;
    }
    else
    {
        auto itNewServer = mServers.insert(std::make_pair(key,boost::make_shared<ServerItem>(ioService,host,port)));
        if(itNewServer.second)
        {
            auto& serverItem = itNewServer.first->second;
            (*serverItem)->OnError.connect([this,&serverItem](TcpClient::ESocketError type,const system::error_code& ec)
            {
                Remove(serverItem->Host(),serverItem->Port());
            }
            );
            mDistributeAlgorithm->Add(serverItem);
            (*serverItem)->Connect(host,port);
            OnServerAdded(serverItem);
            INFO(mLog)<<"Add server "<<host<<":"<<port<<" successed";
            return true;
        }
        WARN(mLog)<<"Server "<<host<<":"<<port<<" already exists in server list.";
        return false;
    }
}

SERVERMGR_EXPORT bool ServerList::Remove(const std::string& host,int port)
{
    boost::unique_lock<boost::mutex> lock(mMutex);
    std::string key = ServerItem::ToString(host,port);
    auto findResult = mServers.find(key);
    if(findResult != mServers.end())
    {
        mDistributeAlgorithm->Remove(findResult->second);
        OnServerRemoved(findResult->second);
        mServers.erase(findResult);
        INFO(mLog)<<"Remove server "<<host<<":"<<port<<" successed";
        return true;
    }
    WARN(mLog)<<"You're tring to remove a not exists server "<<host<<":"<<port;
    return false;
}

SERVERMGR_EXPORT ServerItem::Ptr ServerList::Get(const std::string& key)
{
    boost::unique_lock<boost::mutex> lock(mMutex);
    return mDistributeAlgorithm->Get(mHashFunc(key));
}

SERVERMGR_EXPORT ServerItem::Ptr ServerList::Get(const std::string& host,int port)
{
    boost::unique_lock<boost::mutex> lock(mMutex);
    std::string key = ServerItem::ToString(host,port);
    auto findResult = mServers.find(key);
    if(findResult != mServers.end())
    {
        return findResult->second;
    }

    return nullptr;
}

SERVERMGR_EXPORT DistributeAlgorithm::Ptr ServerList::SetDistributeAlgorithm(const DistributeAlgorithm::Ptr& algorithm)
{
    boost::unique_lock<boost::mutex> lock(mMutex);
    assert(nullptr != algorithm);
    INFO(mLog)<<"Set new distribute algorithm:"<<algorithm.get();
    auto oldAlgorithm = mDistributeAlgorithm;
    mDistributeAlgorithm =  algorithm;
    for(auto& serverPair : mServers)
    {
        mDistributeAlgorithm->Add(serverPair.second);
    }

    if(nullptr != oldAlgorithm)
    {
        oldAlgorithm->Clear();
    }
    return oldAlgorithm;
}

SERVERMGR_EXPORT size_t ServerList::Count( void )
{
    boost::unique_lock<boost::mutex> lock(mMutex);
    return mServers.size(); 
}

SERVERMGR_EXPORT ServerList::HashFunc ServerList::SetHashAlogrithm(HashFunc hFunc) 
{
    boost::unique_lock<boost::mutex> lock(mMutex);
    INFO(mLog)<<"set new hash function:"<<hFunc;
    auto oldFunc = mHashFunc;
    mHashFunc = hFunc; 
    return oldFunc;
}

