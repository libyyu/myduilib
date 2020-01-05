#ifndef __FLIB_SEMAPHORE_H__
#define __FLIB_SEMAPHORE_H__
#pragma once
#include "FLock.h"

#if FLIB_COMPILER_WINDOWS
#ifndef INFINITE
#define INFINITE 0xFFFFFFFF
#endif
#else
#include <semaphore.h>
#include <sys/time.h>
#include <limits>
#endif

//*nix -lrt -pthread

_FStdBegin

class F_DLL_API FSemaphore
{
public:
#if FLIB_COMPILER_WINDOWS
    static const uint32 kInfinite = INFINITE;
    typedef FLIB_HANDLE sem_t;
#elif FLIB_COMPILER_LINUX
    static const uint32 kInfinite = UINTMAX_MAX;
#else
    static const uint32 kInfinite = UINT_MAX;
#endif

	FSemaphore(int32 num = 0);
	~FSemaphore();
    
	// P
	bool wait(uint32 millisecond = kInfinite);
    
	// V
	bool signal();
protected:
#if !FLIB_COMPILER_WINDOWS
	static int32 getAbsTimespec(struct timespec* ts, int32 millisecond);
#endif    

private:
    FSemaphore(const FSemaphore&){}
    void operator=(const FSemaphore&){}
    sem_t _sem;
};

_FStdEnd

#endif//__FLIB_SEMAPHORE_H__
