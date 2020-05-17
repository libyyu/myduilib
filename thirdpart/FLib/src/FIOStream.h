#ifndef __FLIB_FIOSTREAM_H__
#define __FLIB_FIOSTREAM_H__
#pragma once
#include "FType.h"
#include <string>
#include <assert.h>
#include <locale.h>
#include <iostream>
_FStdBegin

template<typename Type>
inline std::string to_string(Type);

template<>
inline std::string to_string(int8 c)
{
	char buf[10] = { 0 };
	sprintf(buf, "%c", c);
	std::string result = buf;
	return result;
}
template<>
inline std::string to_string(uint8 c)
{
	char buf[10] = { 0 };
	sprintf(buf, "%c", c);
	std::string result = buf;
	return result;
}

template<>
inline std::string to_string(int16 c)
{
	char buf[10] = { 0 };
	sprintf(buf, "%d", c);
	std::string result = buf;
	return result;
}
template<>
inline std::string to_string(uint16 c)
{
	char buf[10] = { 0 };
	sprintf(buf, "%u", c);
	std::string result = buf;
	return result;
}

template<>
inline std::string to_string(int32 c)
{
	char buf[10] = { 0 };
	sprintf(buf, "%d", c);
	std::string result = buf;
	return result;
}
template<>
inline std::string to_string(uint32 c)
{
	char buf[10] = { 0 };
	sprintf(buf, "%u", c);
	std::string result = buf;
	return result;
}

template<>
inline std::string to_string(int64 c)
{
	char buf[10] = { 0 };
	sprintf(buf, "%" PRId64, c);
	std::string result = buf;
	return result;
}
template<>
inline std::string to_string(uint64 c)
{
	char buf[10] = { 0 };
	sprintf(buf, "%" PRIu64, c);
	std::string result = buf;
	return result;
}

#if FLIB_COMPILER_64BITS
template<>
inline std::string to_string(int c)
{
	char buf[10] = { 0 };
	sprintf(buf, "%d", c);
	std::string result = buf;
	return result;
}
template<>
inline std::string to_string(uint c)
{
	char buf[10] = { 0 };
	sprintf(buf, "%d", c);
	std::string result = buf;
	return result;
}
#else
template<>
inline std::string to_string(long c)
{
	char buf[10] = { 0 };
	sprintf(buf, "%ld", c);
	std::string result = buf;
	return result;
}
template<>
inline std::string to_string(ulong c)
{
	char buf[10] = { 0 };
	sprintf(buf, "%lu", c);
	std::string result = buf;
	return result;
}
#endif

template<>
inline std::string to_string(float c)
{
	return std::to_string(c);
}

template<>
inline std::string to_string(double c)
{
	return std::to_string(c);
}

template<>
inline std::string to_string(bool c)
{
	return to_string<char>((char)c);
}

template<>
inline std::string to_string(wchar_t c)
{
	return to_string<char>((char)c);
}

std::string wide_to_multi(const wchar_t* c);

template<>
inline std::string to_string(const wchar_t* c)
{
	return wide_to_multi(c);
}
template<>
inline std::string to_string(wchar_t c[])
{
	return to_string<const wchar_t*>(c);
}

template<>
inline std::string to_string(const char* c)
{
	std::string result(c);
	return result;
}
template<>
inline std::string to_string(char c[])
{
	return to_string<const char*>(c);
}

template<>
inline std::string to_string(void* p)
{
	char buf[10] = { 0 };
	sprintf(buf, "%p", p);
	std::string result = buf;
	return result;
}

template<>
inline std::string to_string(const std::wstring& c)
{
	return to_string<const wchar_t*>(c.c_str());
}
//////////////////////////////////////////////////////////////////
////
template<typename Type>
inline std::wstring to_wstring(Type);

template<>
inline std::wstring to_wstring(int8 c)
{
	wchar_t buf[10] = { 0 };
	swprintf(buf, 10, L"%d", c);
	std::wstring result = buf;
	return result;
}
template<>
inline std::wstring to_wstring(uint8 c)
{
	wchar_t buf[10] = { 0 };
	swprintf(buf, 10, L"%d", c);
	std::wstring result = buf;
	return result;
}

template<>
inline std::wstring to_wstring(int16 c)
{
	wchar_t buf[10] = { 0 };
	swprintf(buf, 10, L"%d", c);
	std::wstring result = buf;
	return result;
}

template<>
inline std::wstring to_wstring(uint16 c)
{
	wchar_t buf[10] = { 0 };
	swprintf(buf, 10, L"%u", c);
	std::wstring result = buf;
	return result;
}

template<>
inline std::wstring to_wstring(int32 c)
{
	wchar_t buf[10] = { 0 };
	swprintf(buf, 10, L"%d", c);
	std::wstring result = buf;
	return result;
}
template<>
inline std::wstring to_wstring(uint32 c)
{
	wchar_t buf[10] = { 0 };
	swprintf(buf, 10, L"%u", c);
	std::wstring result = buf;
	return result;
}

template<>
inline std::wstring to_wstring(int64 c)
{
	wchar_t buf[10] = { 0 };
	swprintf(buf, 10, L"%" PRId64, c);
	std::wstring result = buf;
	return result;
}
template<>
inline std::wstring to_wstring(uint64 c)
{
	wchar_t buf[10] = { 0 };
	swprintf(buf, 10, L"%" PRIu64, c);
	std::wstring result = buf;
	return result;
}

#if FLIB_COMPILER_64BITS
template<>
inline std::wstring to_wstring(int c)
{
	wchar_t buf[10] = { 0 };
	swprintf(buf, L"%d", c);
	std::wstring result = buf;
	return result;
}
template<>
inline std::wstring to_wstring(uint c)
{
	wchar_t buf[10] = { 0 };
	swprintf(buf, L"%d", c);
	std::wstring result = buf;
	return result;
}
#else
template<>
inline std::wstring to_wstring(long c)
{
	wchar_t buf[10] = { 0 };
	swprintf(buf, L"%ld", c);
	std::wstring result = buf;
	return result;
}
template<>
inline std::wstring to_wstring(ulong c)
{
	wchar_t buf[10] = { 0 };
	swprintf(buf, L"%lu", c);
	std::wstring result = buf;
	return result;
}
#endif

template<>
inline std::wstring to_wstring(float c)
{
	return std::to_wstring(c);
}

template<>
inline std::wstring to_wstring(double c)
{
	return std::to_wstring(c);
}

template<>
inline std::wstring to_wstring(bool c)
{
	return to_wstring<uint8>((uint8)c);
}

template<>
inline std::wstring to_wstring(wchar_t c)
{
	wchar_t buf[10] = { 0 };
	swprintf(buf, 10, L"%c", c);
	std::wstring result = buf;
	return result;
}

std::wstring multi_to_wide(const char* c);

template<>
inline std::wstring to_wstring(const char* c)
{
	return multi_to_wide(c);
}

template<>
inline std::wstring to_wstring(char c[])
{
	return to_wstring<const char*>(c);
}

template<>
inline std::wstring to_wstring(const wchar_t* c)
{
	std::wstring result(c);
	return result;
}

template<>
inline std::wstring to_wstring(wchar_t c[])
{
	return to_wstring<const wchar_t*>(c);
}

template<>
inline std::wstring to_wstring(void* p)
{
	wchar_t buf[10] = { 0 };
	swprintf(buf, 10, L"%p", p);
	std::wstring result = buf;
	return result;
}

template<>
inline std::wstring to_wstring(const std::string& c)
{
	return to_wstring<const char*>(c.c_str());
}
//////////////////////////////////////////////////////////////////
////
class FOStream;
typedef FOStream& (*FOStreamSymbol_t)(FOStream& Target);

class F_DLL_API FOStream
{
public:
	virtual FOStream& operator <<(int8) = 0;
	virtual FOStream& operator <<(uint8) = 0;
	virtual FOStream& operator <<(int16) = 0;
	virtual FOStream& operator <<(uint16) = 0;
	virtual FOStream& operator <<(int32) = 0;
	virtual FOStream& operator <<(uint32) = 0;
	virtual FOStream& operator <<(int64) = 0;
	virtual FOStream& operator <<(uint64) = 0;
#if FLIB_COMPILER_64BITS
	virtual FOStream& operator <<(int) = 0;
	virtual FOStream& operator <<(uint) = 0;
#else
	virtual FOStream& operator <<(long) = 0;
	virtual FOStream& operator <<(ulong) = 0;
#endif
	virtual FOStream& operator <<(bool) = 0;
	virtual FOStream& operator <<(float) = 0;
	virtual FOStream& operator <<(double) = 0;
	virtual FOStream& operator <<(wchar_t) = 0;
	virtual FOStream& operator <<(wchar_t[]) = 0;
	virtual FOStream& operator <<(const wchar_t*) = 0;
	virtual FOStream& operator <<(char[]) = 0;
	virtual FOStream& operator <<(const char*) = 0;
	virtual FOStream& operator <<(const std::string&) = 0;
	virtual FOStream& operator <<(const std::wstring&) = 0;

	FOStream& operator <<(FOStreamSymbol_t v) { return v(*this); }

	//friend FOStream& operator<<(FOStream& str, const std::string& v);
	//friend FOStream& operator<<(FOStream& str, const std::wstring& v);
};

class FIStream
{
public:
	virtual FIStream& operator >>(int8&) = 0;
	virtual FIStream& operator >>(uint8&) = 0;
	virtual FIStream& operator >>(int16&) = 0;
	virtual FIStream& operator >>(uint16&) = 0;
	virtual FIStream& operator >>(int32&) = 0;
	virtual FIStream& operator >>(uint32&) = 0;
	virtual FIStream& operator >>(int64&) = 0;
	virtual FIStream& operator >>(uint64&) = 0;
#if FLIB_COMPILER_64BITS
	virtual FIStream& operator >>(int&) = 0;
	virtual FIStream& operator >>(uint&) = 0;
#else
	virtual FIStream& operator >>(long&) = 0;
	virtual FIStream& operator >>(ulong&) = 0;
#endif
	virtual FIStream& operator >>(bool&) = 0;
	virtual FIStream& operator >>(float&) = 0;
	virtual FIStream& operator >>(double&) = 0;
	virtual FIStream& operator >>(wchar_t&) = 0;
	virtual FIStream& operator >>(wchar_t[]) = 0;
	virtual FIStream& operator >>(const wchar_t*) = 0;
	virtual FIStream& operator >>(char[]) = 0;
	virtual FIStream& operator >>(const char*) = 0;
	virtual FIStream& operator >>(std::string&) = 0;
	virtual FIStream& operator >>(std::wstring&) = 0;

	//friend FIStream& operator >>(FIStream& str, std::string& v);
	//friend FIStream& operator >>(FIStream& str, std::wstring& v);
};

class FIOStream : public FIStream, public FOStream
{

};

class FOStreamMaker : public FOStream
{
	virtual void output(const char* data) = 0;
public:
	virtual void flush() {}
public:
	using FOStream::operator <<;
	virtual FOStream& operator <<(int8 v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual FOStream& operator <<(uint8 v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual FOStream& operator <<(int16 v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual FOStream& operator <<(uint16 v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual FOStream& operator <<(int32 v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual FOStream& operator <<(uint32 v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual FOStream& operator <<(int64 v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual FOStream& operator <<(uint64 v)
	{
		output(to_string(v).c_str());
		return *this;
	}
#if FLIB_COMPILER_64BITS
	virtual FOStream& operator <<(int v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual FOStream& operator <<(uint v)
	{
		output(to_string(v).c_str());
		return *this;
	}
#else
	virtual FOStream& operator <<(long v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual FOStream& operator <<(ulong v)
	{
		output(to_string(v).c_str());
		return *this;
	}
#endif
	virtual FOStream& operator <<(bool v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual FOStream& operator <<(float v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual FOStream& operator <<(double v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	
	virtual FOStream& operator <<(wchar_t v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual FOStream& operator <<(wchar_t v[])
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual FOStream& operator <<(const wchar_t* v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual FOStream& operator <<(char v[])
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual FOStream& operator <<(const char* v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual FOStream& operator <<(const std::string& v)
	{
		output(v.c_str());
		return *this;
	}
	virtual FOStream& operator <<(const std::wstring& v)
	{
		output(to_string<const std::wstring&>(v).c_str());
		return *this;
	}
	
};

_FStdEnd

#endif//__FLIB_FIOSTREAM_H__