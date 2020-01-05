#ifndef __FLIB_CONVERT_H__
#define __FLIB_CONVERT_H__
#pragma once
#include "FType.h"
#include <iostream>
#include <sstream> 

_FStdBegin
namespace ByteConverter
{
    template<size_t T>
    inline void convert(char *val)
    {
        std::swap(*val, *(val + T - 1));
        convert<T - 2>(val + 1);
    }

    inline bool  IsLittle_Endian() 
    {
        unsigned short t = 0x1122;
        return 0x22 == *((unsigned char *)(&t));
    }
    template<> inline void convert<0>(char *) {}
    template<> inline void convert<1>(char *) {}            // ignore central byte

    template<typename T> inline void apply(T *val)
    {
        convert<sizeof(T)>((char *)(val));
    }
};
template<typename T> 
inline void EndianConvert(T& val)
{ 
#ifdef Big_Endian
    if (ByteConverter::IsLittle_Endian()) {
        ByteConverter::apply<T>(&val);
    }
#else
    if (!ByteConverter::IsLittle_Endian()) {
        ByteConverter::apply<T>(&val);
    }
#endif
}

template<typename T> 
void EndianConvert(T*);         // will generate link error

inline void EndianConvert(uint8& t) { }
inline void EndianConvert(int8& t) { }
_FStdEnd

_FStdBegin
class F_DLL_API FConvert
{
public:
    typedef std::string stringtype;
	typedef std::wstring wstringtype;
public:
	static bool IsTextUTF8(const char* str);

	static bool IsTextGBK(const char* str);

	static stringtype UTF16ToUTF8(const wchar_t* ptext);

	static stringtype UTF16ToGB2312(const wchar_t* ptext);

	static wstringtype GB2312ToUTF16(const char* ptext);

	static stringtype GB2312ToUTF8(const char* ptext);

	static wstringtype UTF8ToUTF16(const char* ptext);

	static stringtype UTF8ToGB2312(const char* ptext);

	template <class Out_type,class In_type>
	static Out_type Convert(In_type &t)
	{
		Out_type out_result;
		_Convert_<Out_type, In_type>(t, &out_result);
		return out_result;
	}
protected:
	static void _UTF16ToUTF8ofChar(char* pOut, const wchar_t* pText);
	static void _UTF8ToUTF16ofChar(wchar_t* pOut, const char *pText);
	static void _Gb2312ToUTF16ofChar(wchar_t* pOut, const char &gbBuffer);
	static void _UTF16ToGB2312ofChar(char* pOut, const wchar_t& uData);

	template <class Out_type,class In_type>
	static bool _Convert_(In_type &t, Out_type* o)
	{
		std::stringstream str;	
		str<<t;
		Out_type out_result;
		if(!(str>>out_result))
			return false;
		*o = out_result;
		return true;
	}
};
_FStdEnd

#endif//__FLIB_CONVERT_H__
