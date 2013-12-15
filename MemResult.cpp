#include "MemResult.h"
void MemResult::SetResult(void* result)
{
    unique_lock<shared_mutex> lock(mFinishMut);
    mResult = result;
    mFinishMut.notify_all();
}

