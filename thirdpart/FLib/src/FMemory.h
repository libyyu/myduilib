#ifndef __FLIB_MEMORY_H__
#define __FLIB_MEMORY_H__
#pragma once
#include "FType.h"
#include <new>
_FStdBegin

class F_DLL_API FMemory
{
public:
	class F_DLL_API FMemStamp
	{
	public:
		char const * const filename;
		char const * const func;
		int const lineNum;
	public:
		FMemStamp(char const *filename, char const *func, int lineNum)
			: filename(filename), func(func), lineNum(lineNum) { }
		~FMemStamp() { }
		void AllocStamp(void* p, char const* typeName) const;
	};

	static void* Malloc(size_t size, const char* file = NULL, const char* func = NULL, int line = 0);
	static void* Realloc(void* p, size_t size, const char* file = NULL, const char* func = NULL, int line = 0);
	static void* Calloc(size_t nmemb, size_t size, const char* file = NULL, const char* func = NULL, int line = 0);
	static void Free(void* p);
};

_FStdEnd


#ifdef TRACK_MEMORY_DEBUG

template <class T>
static T *operator*(const _FStd(FMemory)::FMemStamp &stamp, T *p)
{
	stamp.AllocStamp(p, typeid(T).name());
	return p;
}
FLIB_FORCEINLINE void* operator new(size_t size)
{
	void* p = _FStd(FMemory)::Malloc(size);
	if (p == NULL) throw std::bad_alloc();
	return p;
}
FLIB_FORCEINLINE void operator delete(void* p)
{
	_FStd(FMemory)::Free(p);
}
FLIB_FORCEINLINE void* operator new[](size_t size)
{
	void* p = _FStd(FMemory)::Malloc(size);
	if (p == NULL) throw std::bad_alloc();
	return p;
}
FLIB_FORCEINLINE void operator delete[](void* p)
{
	_FStd(FMemory)::Free(p);
}

#define FLIB_DEBUG_NEW  _FStd(FMemory)::FMemStamp(__FILE__, __FUNCTION__, __LINE__) * new
#else
#define FLIB_DEBUG_NEW  new
#endif

#define FLIB_ALLOC(size) _FStd(FMemory)::Malloc(size, __FILE__, __FUNCTION__, __LINE__)
#define FLIB_CALLOC(nmemb, size) _FStd(FMemory)::Calloc(nmemb, size, __FILE__, __FUNCTION__, __LINE__)
#define FLIB_REALLOC(p, size) _FStd(FMemory)::Realloc(p, size, __FILE__, __FUNCTION__, __LINE__)
#define FLIB_FREE(p) _FStd(FMemory)::Free(p)
#define FLIB_ZALLOC(type) (type*)_FStd(FMemory)::Calloc(1, sizeof(type), __FILE__, __FUNCTION__, __LINE__)
#define FLIB_ZALLOCN(type, n) (type*)_FStd(FMemory)::Calloc(n, sizeof(type), __FILE__, __FUNCTION__, __LINE__)
#define FLIB_ZREALLOC(type, p, n) (type*)_FStd(FMemory)::Realloc(p, (n) * sizeof(type), __FILE__, __FUNCTION__, __LINE__)

#endif//__FLIB_MEMORY_H__