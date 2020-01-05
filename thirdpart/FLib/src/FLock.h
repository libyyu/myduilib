#ifndef __FLIB_LOCK_H__
#define __FLIB_LOCK_H__
#pragma once
#include "FType.h"


_FStdBegin
class F_DLL_API i_lock
{
public:
    i_lock() {}
    ~i_lock() {}
    virtual bool lock() = 0;
    virtual bool unlock() = 0;
    virtual int lock_ex() { return 0; }
    virtual int unlock_ex() { return 0; }
};
class F_DLL_API lock_wrapper
{
public:
    lock_wrapper(i_lock* p_lock)
    {
        mp_lock = p_lock;
        if (mp_lock != NULL)
            mp_lock->lock();
    }
    ~lock_wrapper() { unlock(); }
    void unlock()
    {
        if (mp_lock != NULL)
        {
            mp_lock->unlock();
            mp_lock = NULL;
        }
    }
private:
    i_lock* mp_lock;
};

class F_DLL_API FLock : public i_lock
{
public:
	FLock();
	~FLock();
	bool lock();
	bool unlock();
protected:
	struct impl;
	impl* mp_impl;
};

_FStdEnd

#endif//__FLIB_LOCK_H__
