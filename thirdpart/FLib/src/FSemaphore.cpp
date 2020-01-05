#define FLIB_DLL
#include "FSemaphore.h"
#include <time.h>
#include <functional>
#if FLIB_COMPILER_WINDOWS
#include <Windows.h>
#else
#include <semaphore.h>
#include <sys/time.h>
#include <limits>
#include <errno.h>
#endif
#if FLIB_COMPILER_CYGWIN
#include <limits.h>
#endif


_FStdBegin

FSemaphore::FSemaphore(int32 num /*= 0*/)
{
#if FLIB_COMPILER_WINDOWS
	_sem = ::CreateSemaphore(NULL, num, LONG_MAX, NULL);
#else
	sem_init(&_sem, 0, num);
#endif    
}

FSemaphore::~FSemaphore()
{
#if FLIB_COMPILER_WINDOWS
	if (NULL != _sem)
	{
		if (0 != ::CloseHandle(_sem))
		{
			_sem = NULL;
		}
	}
#else
	sem_destroy(&_sem);
#endif    
}

// P
bool FSemaphore::wait(uint32 millisecond /*= kInfinite*/)
{
#if FLIB_COMPILER_WINDOWS
	if (NULL == _sem)
		return false;

	DWORD ret = ::WaitForSingleObject(_sem, millisecond);
	if (WAIT_OBJECT_0 == ret || WAIT_ABANDONED == ret)
	{
		return true;
	}
	else
	{
		return false;
	}
#else
	int32_t ret = 0;

	if (kInfinite == millisecond)
	{
		ret = sem_wait(&_sem);
	}
	else
	{
		struct timespec ts = { 0, 0 };
		getAbsTimespec(&ts, millisecond);
#if _POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600
		ret = sem_timedwait(&_sem, &ts);
#else
		ret = sem_wait(&_sem);
#endif
	}

	return -1 != ret;
#endif
}

// V
bool FSemaphore::signal()
{
#if FLIB_COMPILER_WINDOWS
	BOOL ret = FALSE;

	if (NULL != _sem)
	{
		ret = ::ReleaseSemaphore(_sem, 1, NULL);
	}
	return TRUE == ret;
#else
	return -1 != sem_post(&_sem);
#endif    
}

#if !FLIB_COMPILER_WINDOWS
int32 FSemaphore::getAbsTimespec(struct timespec* ts, int32 millisecond)
{
	if (NULL == ts)
		return EINVAL;

	struct timeval tv;
	int32_t ret = gettimeofday(&tv, NULL);
	if (0 != ret)
		return ret;

	ts->tv_sec = tv.tv_sec;
	ts->tv_nsec = tv.tv_usec * 1000UL;

	ts->tv_sec += millisecond / 1000UL;
	ts->tv_nsec += millisecond % 1000UL * 1000000UL;

	ts->tv_sec += ts->tv_nsec / (1000UL * 1000UL * 1000UL);
	ts->tv_nsec %= (1000UL * 1000UL * 1000UL);

	return 0;
}
#endif

_FStdEnd

