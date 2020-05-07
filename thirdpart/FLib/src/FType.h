#ifndef __FLIB_TYPE_H__
#define __FLIB_TYPE_H__
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string>
#include <vector>
#include <set>
#include <stddef.h>
#include "FPlatform.h"
#if FLIB_COMPILER_CYGWIN
#include <cstring>
#elif !FLIB_COMPILER_MSVC
#include <string.h>
#endif
#if FLIB_COMPILER_WINDOWS
#include <tchar.h>
#if FLIB_UNICODE
	#undef _T
    #define _T(type)           L##type
    #define _W(fun)            w##fun
    #define Fchar              wchar_t
    #define Fstring 	       std::wstring
    #define Fstringstream      std::wstringstream
    #define Fstrftime          wcsftime
    #define Fstrcat            wcscat      

    #define Ffprintf           fwprintf
	#define Fstrstr			   wcsstr
	#define Fcsstr			   wcschr
    #define Fstrrchr           wcsrchr
    #define Fstrncpy           wcsncpy
	#define Fstrncasecmp	   _wcsnicmp
    #define Fstrlen            wcslen
    #define Fstrcmp            wcscmp
	#define Fstrcpy            wcscpy
	#define Fcscspn			   wcscspn
    #define Fvfprintf          vfwprintf
    #define Fvsnprintf		   _vsnwprintf_s 
	#define Fsprintf	       swprintf
#else
    #undef _T
    #define _T(type)           type
    #define _W(fun)            fun
    #define Fchar              char
    #define Fstring			   std::string
    #define Fstringstream      std::stringstream
    #define Fstrftime          strftime
    #define Fstrcat            strcat

    #define Ffprintf           fprintf
	#define Fstrstr			   strstr
    #define Fstrrchr           strrchr
	#define Fcsstr			   strchr
    #define Fstrncpy           strncpy
	#define Fstrncasecmp	   _strnicmp
    #define Fstrlen            strlen
    #define Fstrcmp            strcmp
    #define Fstrcpy            strcpy
	#define Fcscspn			   strcspn
    #define Fvfprintf          vfprintf
	#define Fvsnprintf		   vsnprintf 
	#define Fsprintf	       sprintf
#endif
#else
#include <wchar.h>
#if FLIB_UNICODE
    #undef _T
    #define _T(type)           L##type
    #define _W(fun)            w##fun
    #define Fchar              wchar_t
    #define Fstring 	       std::wstring
    #define Fstringstream      std::wstringstream
    #define Fstrftime          wcsftime
    #define Fstrcat            wcscat      

    #define Ffprintf           fwprintf
	#define Fstrstr			   wcsstr
	#define Fcsstr			   wcschr
    #define Fstrrchr           wcsrchr
    #define Fstrncpy           wcsncpy
	#define Fstrncasecmp	   wcsncasecmp
    #define Fstrlen            wcslen
    #define Fstrcmp            wcscmp
	#define Fstrcpy            wcscpy
	#define Fcscspn			   wcscspn
    #define Fvfprintf          vfwprintf
    #define Fvsnprintf		   vswprintf 
	#define Fsprintf	       swprintf
#else
    #undef _T
    #define _T(type)           type
    #define _W(fun)            fun
    #define Fchar              char
    #define Fstring			   std::string
    #define Fstringstream      std::stringstream
    #define Fstrftime          strftime
    #define Fstrcat            strcat

    #define Ffprintf           fprintf
	#define Fstrstr			   strstr
    #define Fstrrchr           strrchr
	#define Fcsstr			   strchr
    #define Fstrncpy           strncpy
	#define Fstrncasecmp	   strncasecmp
    #define Fstrlen            strlen
    #define Fstrcmp            strcmp
    #define Fstrcpy            strcpy
	#define Fcscspn			   strcspn
    #define Fvfprintf          vfprintf
	#define Fvsnprintf		   vsnprintf 
	#define Fsprintf	       sprintf
#endif
#endif

#ifdef  _DEBUG
     #define  FDeBugOut(type)   cout(type)
#else
     #define  FDeBugOut(type)   //cout(type)
#endif

#if defined(__cplusplus)
    #define _FDeclsBegin   extern "C" {
    #define _FDeclsEnd     }
#else
    #define _FDeclsBegin
    #define _FDeclsEnd
#endif

#if defined(__cplusplus)
    #define  _FCFun   extern "C"
#else
    #define  _FCFun   extern
#endif //__cplusplus

#define  _FNameSpaceBegin(name)   namespace name {
#define  _FNameSpaceEnd           }
#define  _FUsing(name)            using namespace name;

#define _FStdBegin  _FNameSpaceBegin(FStd)
#define _FStdEnd    _FNameSpaceEnd
#define _FStdUsing  _FUsing(FStd)
#define _FStd(x)	FStd::x
#define _FStr(x)    _T(x)
/********************************************************************************
 
 Base integer types for all target OS's and CPU's
 
 UInt8            8-bit unsigned integer 
 SInt8            8-bit signed integer
 UInt16          16-bit unsigned integer 
 SInt16          16-bit signed integer           
 UInt32          32-bit unsigned integer 
 SInt32          32-bit signed integer   
 UInt64          64-bit unsigned integer 
 SInt64          64-bit signed integer   
 
 *********************************************************************************/
typedef unsigned char                   uchar;
typedef unsigned short                  ushort;
typedef unsigned int                    uint;
typedef unsigned long                   ulong;

typedef char							int8;
typedef uchar                           uint8;
typedef short							int16;
typedef ushort                          uint16;


#if FLIB_COMPILER_64BITS
typedef long							int32;
typedef unsigned long                   uint32;
#else
typedef int								int32;
typedef unsigned int                    uint32;
#endif

#if FLIB_COMPILER_WINDOWS
typedef __int64           				int64;
typedef unsigned __int64  				uint64;
#else
typedef __int64_t         				int64;
typedef __uint64_t        				uint64;
#endif

#if FLIB_COMPILER_WINDOWS
typedef unsigned char byte;
#endif

typedef void *FLIB_HANDLE;
typedef std::set<std::string> StringSet;
typedef std::vector<std::string> StringVec;
typedef std::vector<uchar> ByteArray;

#undef FLIB_DISALLOW_CONSTRUCTORS
#define FLIB_DISALLOW_CONSTRUCTORS(TypeName)    \
    TypeName(const TypeName&);            \
    void operator= (const TypeName&);


#define flib_lengthof(x)   (sizeof(x)/sizeof(x[0]))

#ifndef __FLIB_FILET__
#define __FLIB_STR2WSTR(str)	L##str
#define _FLIB_STR2WSTR(str)	__FLIB_STR2WSTR(str)
#if FLIB_UNICODE
#define __FLIB_FILET__		_FLIB_STR2WSTR(__FILE__)
#define __FLIB_FUNCTIONT__	_FLIB_STR2WSTR(__FUNCTION__)
#else
#define __FLIB_FILET__		__FILE__
#define __FLIB_FUNCTIONT__	__FUNCTION__
#endif
#endif

#define FLIB_PI 3.1415926f
#define FLIB_D2R(d) (((d)*FLIB_PI) / 180)
#define FLIB_R2D(r) (((r)*180) / FLIB_PI)
#define FLIB_MIN(a,b) ((a)<(b)) ? (a) : (b)
#define FLIB_MAX(a,b) ((a)>(b)) ? (a) : (b)
#define FLIB_ABS(a) ((a) < (0) ? (-a) : (a))

#define min2(a, b) (((a) > (b)) ? (b) : (a))
#define min3(a, b, c) (min2(min2((a), (b)), (c)))
#define max2(a, b) (((a) > (b)) ? (a) : (b))
#define max3(a, b, c) (max2(max2((a), (b)), (c)))
#define min4(a, b, c, d) (min2(min2((a), (b)), min2((c), (d))))
#define max4(a, b, c, d) (max2(max2((a), (b)), max2((c), (d))))

#define FLIB_JOIN( X, Y ) FLIB_DO_JOIN( X, Y )
#define FLIB_DO_JOIN( X, Y ) FLIB_DO_JOIN2(X,Y)
#define FLIB_DO_JOIN2( X, Y ) X##Y
#define FLIB_MAKE64(low, high)   ((uint64) (((uint32) (low)) | ((uint64) ((uint32) (high))) << 32))

#if FLIB_COMPILER_WINDOWS
#define SEP               '\\'
#define PRId64            "I64d"
#define PRIu64            "I64u"
#define PRIx64            "I64x"
#define _STDCALL          __stdcall
#else
#define SEP               '/'
#define PRId64            "lld"
#define PRIu64            "llu"
#define PRIx64            "llx"
#define INFINITE          0xFFFFFFFF
#define _STDCALL
#endif

#if !FLIB_COMPILER_MSVC && !defined(MAX_PATH)
#define MAX_PATH 255
#endif /*MAX_PATH*/

#if defined(NDEBUG)
#define FLIB_ENSURE(p) (void)(p)
#else
#define FLIB_ENSURE(p) assert(p)
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#if FLIB_COMPILER_CYGWIN
_FDeclsBegin
_CRTIMP FILE* __cdecl __MINGW_NOTHROW _fdopen(int, const char*);
//_CRTIMP FILE* __cdecl __MINGW_NOTHROW fdopen(int, const char*);
_CRTIMP int __cdecl __MINGW_NOTHROW _strnicmp(const char*, const char*, size_t);
_CRTIMP int __cdecl _wcsnicmp(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount);
_CRTIMP int __cdecl _swprintf(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Format,...);
_CRTIMP int __cdecl _vswprintf(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Format,va_list _Args);
_CRTIMP int __cdecl _snwprintf(wchar_t * __restrict__ _Dest,size_t _Count,const wchar_t * __restrict__ _Format,...);
_CRTIMP int __cdecl _vsnwprintf(wchar_t * __restrict__ _Dest,size_t _Count,const wchar_t * __restrict__ _Format,va_list _Args);
//#define _fdopen fdopen
_FDeclsEnd
#endif

_FStdBegin
enum FLIB_LOGLEVEL 
{
	FLIB_LOGLEVEL_TRACE,
	FLIB_LOGLEVEL_DEBUG,
	FLIB_LOGLEVEL_INFO,
	FLIB_LOGLEVEL_WARN,
	FLIB_LOGLEVEL_ERROR,
	FLIB_LOGLEVEL_FATAL,
    FLIB_LOGLEVEL_NUM_LOG_LEVELS
};
const static char* FLIB_LogLevelName[FLIB_LOGLEVEL::FLIB_LOGLEVEL_NUM_LOG_LEVELS] =
{
	"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL",
};
_FStdEnd

#endif//__FLIB_TYPE_H__
