#ifndef __FLIB_DREADAWRITELOCK_H__
#define __FLIB_DREADAWRITELOCK_H__
#pragma once
#include "FLock.h"

_FStdBegin

class F_DLL_API FDReadAWriteLock
{
    typedef struct _F_InsideLock_
    {
        int nReadCount;
        bool bWriteFlag;
        FLock lock;
    }SInsideLock;
private:
	SInsideLock m_Lock;
public:
	FDReadAWriteLock(void);
	virtual ~FDReadAWriteLock(void);
public:
	void EnterWrite();
    void LeaveWrite();
    int EnterRead();
    int LeaveRead();
    int GetReadCount();
    bool IsWrite();
};

_FStdEnd

#endif//__FLIB_DREADAWRITELOCK_H__