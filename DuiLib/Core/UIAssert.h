#ifndef __DUI_ASSERTX_H__
#define __DUI_ASSERTX_H__

#ifdef _MSC_VER
#pragma once
#endif

namespace DuiLib
{
	// Assert function return values
	enum ErrRet
	{
		ERRRET_IGNORE = 0,
		ERRRET_CONTINUE,
		ERRRET_BREAKPOINT,
		ERRRET_ABORT
	};

	//- Global functions ----------------------------------------------------------
	UILIB_API ErrRet NotifyAssert(const TCHAR* condition, const TCHAR* fileName, int lineNumber, const TCHAR* formats, ...);
}

#if defined(_DEBUG) || defined(DEBUG)
	#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(_WIN64) || defined(WIN64) || defined(__WIN64__)
        #define DuiAssertX(x, formats, ...) { \
		static bool _ignoreAssert = false; \
		if (!_ignoreAssert && !(x)) \
		{ \
			DuiLib::ErrRet _err_result = DuiLib::NotifyAssert(#x, __FILET__, __LINE__, formats, ##__VA_ARGS__); \
			if (_err_result == DuiLib::ErrRet::ERRRET_IGNORE) \
			{ \
				_ignoreAssert = true; \
			} \
			else if (_err_result == DuiLib::ErrRet::ERRRET_BREAKPOINT) \
			{ \
			__debugbreak(); \
			} \
		}}
	#else
		#define DuiAssertX(x, formats, ...) { \
		if (!(x)) \
		{ \
			DuiLib::CDuiString comment; \
			comment.Format(formats, ##__VA_ARGS__); \
			DuiLib::CDuiString msg; \
			if(!comment.IsEmpty()){ msg = DuiLib::CDuiString(_T("Assert comment:")) + comment + DuiLib::CDuiString(_T("\n")); } \
#ifndef _UNICODE
			fprintf (stderr, _T("%s"), msg); \
#else
			fwprintf(stderr, _T("%s"), msg); \
#endif
			assert(x); \
			exit(-1);\
		}}
#	endif
#else
#		define DuiAssertX(x, formats, ...)
#endif

#if defined(_DEBUG) || defined(DEBUG)
#	define DuiAssert(x)		DuiAssertX(x, _T(""))
#else
#	define DuiAssert(x)
#endif

#endif
