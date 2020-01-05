#define FLIB_DLL
#include "FLock.h"
#include <time.h>
#include <sstream>
#include <iostream>
#if FLIB_COMPILER_WINDOWS
#include <windows.h>
#else
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <errno.h>
#endif

_FStdBegin

#if FLIB_COMPILER_WINDOWS
struct FLock::impl
{
	CRITICAL_SECTION cs;
};
#else
struct FLock::impl
{
	pthread_mutex_t m;
};
#endif

FLock::FLock()
{
	mp_impl = new impl;
#if FLIB_COMPILER_WINDOWS
	::InitializeCriticalSection(&(mp_impl->cs));
#else
	pthread_mutexattr_t   mta;
	pthread_mutexattr_init(&mta);
	pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&(mp_impl->m), &mta);
#endif
}
FLock::~FLock()
{
	assert(mp_impl);
#if FLIB_COMPILER_WINDOWS
	DeleteCriticalSection(&(mp_impl->cs));
#else
	pthread_mutex_destroy(&(mp_impl->m));
#endif
	delete mp_impl;
	mp_impl = NULL;
}
bool FLock::lock()
{
#if FLIB_COMPILER_WINDOWS
	EnterCriticalSection(&(mp_impl->cs));
#else
	pthread_mutex_lock(&(mp_impl->m));
#endif
	return true;
}
bool FLock::unlock()
{
#if FLIB_COMPILER_WINDOWS
	LeaveCriticalSection(&(mp_impl->cs));
#else
	pthread_mutex_unlock(&(mp_impl->m));
#endif
	return false;
}

_FStdEnd
