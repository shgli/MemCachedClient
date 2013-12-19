#ifndef _REQUESTITEM_H
#define _REQUESTITEM_H

class RequestStatus
{
private:
    int mRequestId;
    int mErrorCode;
    std::string mKey;

    std::mutex mSyncMut;
    std::condition_variable mSyncEvent;
public:
    typedef std::shared_ptr<RequestStatus> Ptr;

    void RequestId(int id) { mRequestId = id; }
    int RequestId( void )const { return mRequestId; }
    void ErrorCode(int err) { mErrorCode = err; }
    int ErrorCode( void ) const { return mErrorCode; }
    void Key(const std::string& key) { mKey = key; }
    const std::string& Key( void ) const { return mKey; }
    bool Successed( void );

    void Finish( void ) const;

    void Notify( void );
};


class MemResult
{
    RequestStatus::Ptr mStatus;
    SharedBuffer mValue;
public:
    typedef std::shared_ptr<MemResult> Ptr;

    MemResult(RequestStatus::Ptr mStatus,SharedBuffer buffer);

    const SharedBuffer& Value() const { return mValue; }
    SharedBuffer& Value() { return mValue; }

    int ErrorCode( void ) const { return mStatus->ErrorCode(); }
    const std::string& Key( void ) const { return mStatus->Key(); }
    bool Successed( void ) const { return mStatus->Successed(); }

    void Finish( void ) const { return mStatus->Finish(); }

    static const std::string StrError(int error);
};


class RequestItem
{
    RequestStatus::Ptr mStatus;
    std::vector<std::pair<Callback,MemResult::Ptr> > mCallbacks;
public:

    MemResult::Ptr Add(Callback,SharedBuffer buffer);
    MemResult::Ptr OneResult ( void ) ;

    void Notify( void );

};
#endif
 
