#ifndef _DuiType_H
#define _DuiType_H
#pragma once

#include <stdlib.h>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <set>

namespace DuiLib
{

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
	typedef unsigned char                   dui_uchar;
	typedef unsigned short                  dui_ushort;
	typedef unsigned int                    dui_uint;
	typedef unsigned long                   dui_ulong;

	typedef signed char                     dui_int8;
	typedef dui_uchar                       dui_uint8;
	typedef signed short                    dui_int16;
	typedef dui_ushort                      dui_uint16;

#if !defined(__LP64__) && !defined(_WIN64)
	typedef signed int                      dui_int32;
	typedef dui_uint                        dui_uint32;
#else
	typedef signed long                     dui_int32;
	typedef unsigned long                   dui_uint32;
#endif

#if defined(_MSC_VER)
	typedef   signed __int64                dui_int64;
	typedef unsigned __int64                dui_uint64;
#else
	typedef   signed long long              dui_int64;
	typedef unsigned long long              dui_uint64;
#endif

	typedef unsigned int					dui_dword;
	typedef int								dui_bool;
	typedef unsigned char					dui_byte;
	typedef unsigned short					dui_word;

	typedef dui_uint8						dui_UTF8;
	typedef dui_uint32						dui_UTF32;

	typedef char							dui_char;
	typedef wchar_t							dui_wchar;

	typedef std::string						dui_String;
	typedef std::wstring					dui_WString;
	typedef std::stringstream 				dui_StringStream;
	typedef std::wstringstream 				dui_WStringStream;
	typedef std::basic_string<dui_UTF8, std::char_traits<dui_UTF8> >    dui_UTF8String;
	typedef std::basic_string<dui_UTF32, std::char_traits<dui_UTF32> >	dui_UTF32String;

#if defined(UNICODE) || defined(_UNICODE)
	typedef dui_WString						dui_stdstring;
	typedef dui_wchar						dui_stdchar;
	typedef dui_WStringStream				dui_stringstream;

	#define dui_strftime					wcsftime
	#define dui_strcat						wcscat      
	#define dui_fopen						_wfopen

	#define dui_sprintf						wsprintf
	#define dui_fprintf						fwprintf
	#define dui_strrchr						wcsrchr
	#define dui_strncpy						wcsncpy
	#define dui_strlen						wcslen
	#define dui_strcmp						wcscmp
	#define dui_strcpy						lstrcpy
	#define dui_vfprintf					vfwprintf
#else
	typedef dui_String						dui_stdstring;
	typedef dui_char						dui_stdchar;
	typedef dui_StringStream				dui_stringstream;

	#define dui_strftime					strftime
	#define dui_strcat						strcat
	#define dui_fopen						fopen

	#define dui_sprintf						sprintf_s
	#define dui_fprintf						fprintf
	#define dui_strrchr						strrchr
	#define dui_strncpy						strncpy
	#define dui_strlen						strlen
	#define dui_strcmp						strcmp
	#define dui_strcpy						strcpy
	#define dui_vfprintf					vfprintf
#endif//Unicode

};


#define DECLARE_CLASS(Cls) \
        public:\
        virtual LPCTSTR GetClassName() const \
        { \
            return _T(#Cls); \
        };

#endif//_DuiType_H