#ifndef __FLIB_FUNC_H__
#define __FLIB_FUNC_H__
#pragma once
#include "FType.h"
#include <functional>
_FStdBegin

template<typename T>
inline void FSafeDelete(T* &p)
{
	delete p;
	p = NULL;
}

template<typename T>
inline void FSwap(T& r, T& l)
{
    T tmp = r;
    r = l;
    l = tmp;
}

F_DLL_API tm* FGetNowTime();

F_DLL_API void FSleep(unsigned int seconds);

F_DLL_API std::string FFormat(const char* format, ...);

F_DLL_API uint32 FGetCurrentThreadId();

F_DLL_API char* FGetPwd(char *buffer);

F_DLL_API bool FMakeDir(const char* path);

F_DLL_API void FSplitpath(const char* s, char* path, char* name, char* ext);

F_DLL_API const char* FGetModulePath();

F_DLL_API const char* FGetModuleName();

F_DLL_API bool FFileExists(const char* filename);

F_DLL_API bool FDirExists(const char* path);

F_DLL_API int FGetAllFiles(const char* path, bool reversal = true, const std::function<void(const char*, bool)>& action = nullptr);

F_DLL_API void FUnitPath(char *path);

F_DLL_API void FUnitPath(std::string& path);

F_DLL_API std::string FJoinPath(const std::string& path1, const std::string& path2);

F_DLL_API std::string FNormalize(const char* path);

F_DLL_API std::string FGetDirectory(const char* path);

F_DLL_API std::string FGetFilename(const char* path);

F_DLL_API std::string FGetFilenameNoExtension(const char* path);

F_DLL_API std::string FGetExtension(const char* path);

F_DLL_API void FStringReplace(std::string &strBase, const std::string& strSrc, const std::string& strDes);

F_DLL_API void FStringSplit(StringVec& outArr, const std::string &str, const std::string &pattern);

F_DLL_API bool FReadAllBytes(const char* filename, ByteArray& buffer);

F_DLL_API bool FWriteAllBytes(const char* filename, uchar* bytes, size_t size);

F_DLL_API bool FReadAllText(const char* filename, std::string& text);

F_DLL_API bool FReadTextToArray(const char* filename, std::vector<std::string>& resVec);

typedef enum FErrRet
{
    FERRRET_IGNORE = 0,
    FERRRET_CONTINUE,
    FERRRET_BREAKPOINT,
    FERRRET_ABORT
} FErrRet;

FErrRet FNotifyAssert(const char* condition, const char* fileName, int lineNumber, const char* formats, ...);


_FStdEnd

_FStdBegin
F_DLL_API void* memcpyFromRing(void * dst, const size_t cnt, const void * src, size_t b, size_t size);
F_DLL_API void* memcpyToRing(const void *src, const size_t cnt, void * dst, size_t b, size_t size);
F_DLL_API int memfind(const void* src, size_t srcsize, const void* dst, size_t dstsize);

_FStdEnd

#define F_FORMAT _FStd(FFormat)

#if defined( _DEBUG ) || defined( DEBUG )
#if FLIB_COMPILER_WINDOWS
#define FLIB_ASSERTX(x,fmt,...) { \
		static bool _ignoreAssert = false; \
		if(!_ignoreAssert && !(x)) \
		{ \
			_FStd(FErrRet) _err_result = _FStd(FNotifyAssert)(#x, __FILE__, __LINE__, fmt, ##__VA_ARGS__); \
			if (_err_result == _FStd(FErrRet)::FERRRET_IGNORE) \
			{ \
				_ignoreAssert = true; \
			} \
			else if (_err_result == _FStd(FErrRet)::FERRRET_BREAKPOINT) \
			{ \
				__debugbreak(); \
			} \
		}}
#else
#define FLIB_ASSERTX(x, fmt, ...) { \
		if (!(x)) \
		{ \
			std::string comment = _FStd(FFormat)(fmt, ##__VA_ARGS__); \
			if(!comment.empty()){ comment = std::string("Assert Comment:") + comment + std::string("\n"); } \
			fprintf (stderr, "%s", comment.c_str()); \
			assert(x); \
			exit(-1);\
		}}
#endif//FLIB_COMPILER_WINDOWS
#else
	#define FLIB_ASSERTX(x, fmt, ...)
#endif//_DEBUG

#if defined(_DEBUG) || defined(DEBUG)
#	define FLIB_ASSERT(x)		FLIB_ASSERTX(x, "")
#else
#	define FLIB_ASSERT(x)
#endif


#endif//__FLIB_FUNC_H__
