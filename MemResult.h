#ifndef _MEMRESULT_H
#define _MEMRESULT_H
#include "MemcachedCommon.h"
#include <boost/thread/shared_mutex.hpp>
class MemResult
{
    using namespace boost;
public:
    MemResult();

    int ErrorCode( void ) const { return mErrorCode; }

    template<typename T>
    T Finish<T>( void )
    {
	scoped_lock<shared_mutex> lock(mFinishLock);
	mFinishCond.wait(lock);

	return *(T*)mResult;
    }

internal:
    void SetResult(void* result);
    
private:
    int mErrorCode;

    shared_mutex mFinishMut;
    condition_variable mFinishCond;

    void* mResult;
};
#endif
 
