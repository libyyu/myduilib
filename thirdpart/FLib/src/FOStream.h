#ifndef __FLIB_OSTREAM_H__
#define __FLIB_OSTREAM_H__
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
inline std::string to_string(char c)
{
	char buf[10] = { 0 };
	sprintf(buf, "%c", c);
	std::string result = buf;
	return result;
}
template<>
inline std::string to_string(unsigned char c)
{
	char buf[10] = { 0 };
	sprintf(buf, "%c", c);
	std::string result = buf;
	return result;
}

template<>
inline std::string to_string(short c)
{
	char buf[10] = { 0 };
	sprintf(buf, "%d", c);
	std::string result = buf;
	return result;
}
template<>
inline std::string to_string(unsigned short c)
{
	char buf[10] = { 0 };
	sprintf(buf, "%d", c);
	std::string result = buf;
	return result;
}

template<>
inline std::string to_string(int c)
{
	return std::to_string(c);
}
template<>
inline std::string to_string(unsigned int c)
{
	return std::to_string(c);
}

template<>
inline std::string to_string(long c)
{
	return std::to_string(c);
}
template<>
inline std::string to_string(unsigned long c)
{
	return std::to_string(c);
}

template<>
inline std::string to_string(int64_t c)
{
	return std::to_string(c);
}
template<>
inline std::string to_string(uint64_t c)
{
	return std::to_string(c);
}

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
inline std::string to_string(wchar_t* c)
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
inline std::string to_string(char* c)
{
	std::string result(c);
	return result;
}

template<>
inline std::string to_string(void* p)
{
	char buf[10] = { 0 };
	sprintf(buf, "%p", p);
	std::string result = buf;
	return result;
}

template<typename Type>
inline std::wstring to_wstring(Type);

template<>
inline std::wstring to_wstring(short c)
{
	wchar_t buf[10] = { 0 };
	swprintf(buf, 10, L"%d", c);
	std::wstring result = buf;
	return result;
}
template<>
inline std::wstring to_wstring(unsigned short c)
{
	wchar_t buf[10] = { 0 };
	swprintf(buf, 10, L"%d", c);
	std::wstring result = buf;
	return result;
}

template<>
inline std::wstring to_wstring(int c)
{
	return std::to_wstring(c);
}
template<>
inline std::wstring to_wstring(unsigned int c)
{
	return std::to_wstring(c);
}

template<>
inline std::wstring to_wstring(long c)
{
	return std::to_wstring(c);
}
template<>
inline std::wstring to_wstring(unsigned long c)
{
	return std::to_wstring(c);
}

template<>
inline std::wstring to_wstring(int64_t c)
{
	return std::to_wstring(c);
}
template<>
inline std::wstring to_wstring(uint64_t c)
{
	return std::to_wstring(c);
}

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
	return to_wstring<short>((short)c);
}

template<>
inline std::wstring to_wstring(wchar_t c)
{
	wchar_t buf[10] = { 0 };
	swprintf(buf, 10, L"%c", c);
	std::wstring result = buf;
	return result;
}

template<>
inline std::wstring to_wstring(const wchar_t* c)
{
	std::wstring result(c);
	return result;
}
template<>
inline std::wstring to_wstring(wchar_t* c)
{
	std::wstring result(c);
	return result;
}
std::wstring multi_to_wide(const char* c);
template<>
inline std::wstring to_wstring(const char* c)
{
	return multi_to_wide(c);
}

template<>
inline std::wstring to_wstring(char* c)
{
	return to_wstring<const char*>(c);
}

template<>
inline std::wstring to_wstring(void* p)
{
	wchar_t buf[10] = { 0 };
	swprintf(buf, 10, L"%p", p);
	std::wstring result = buf;
	return result;
}

class IOStream;
typedef IOStream& (*StreamSymbol_t)(IOStream& Target);

class IOStream
{
public:
	virtual IOStream& operator <<(char) = 0;
	virtual IOStream& operator <<(unsigned char) = 0;
	virtual IOStream& operator <<(short) = 0;
	virtual IOStream& operator <<(unsigned short) = 0;
	virtual IOStream& operator <<(int) = 0;
	virtual IOStream& operator <<(unsigned int) = 0;
	virtual IOStream& operator <<(long) = 0;
	virtual IOStream& operator <<(unsigned long) = 0;
	virtual IOStream& operator <<(int64_t) = 0;
	virtual IOStream& operator <<(uint64_t) = 0;
	virtual IOStream& operator <<(float) = 0;
	virtual IOStream& operator <<(double) = 0;
	virtual IOStream& operator <<(bool) = 0;
	virtual IOStream& operator <<(wchar_t) = 0;
	virtual IOStream& operator <<(wchar_t[]) = 0;
	virtual IOStream& operator <<(const wchar_t*) = 0;
	virtual IOStream& operator <<(char[]) = 0;
	virtual IOStream& operator <<(const char*) = 0;
	virtual IOStream& operator <<(void*) = 0;
	IOStream& operator <<(StreamSymbol_t v) { return v(*this); }
	virtual void flush() = 0;
};

class IOStreamMaker : public IOStream
{
	virtual void output(const char* data) = 0;
public:
	virtual void flush() {}
public:
	using IOStream::operator <<;
	virtual IOStream& operator <<(char v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual IOStream& operator <<(unsigned char v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual IOStream& operator <<(short v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual IOStream& operator <<(unsigned short v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual IOStream& operator <<(int v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual IOStream& operator <<(unsigned int v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual IOStream& operator <<(long v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual IOStream& operator <<(unsigned long v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual IOStream& operator <<(int64_t v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual IOStream& operator <<(uint64_t v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual IOStream& operator <<(float v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual IOStream& operator <<(double v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual IOStream& operator <<(bool v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual IOStream& operator <<(wchar_t v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual IOStream& operator <<(wchar_t v[])
	{
		output(to_string<const wchar_t*>(v).c_str());
		return *this;
	}
	virtual IOStream& operator <<(const wchar_t* v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual IOStream& operator <<(char v[])
	{
		output(to_string<const char*>(v).c_str());
		return *this;
	}
	virtual IOStream& operator <<(const char* v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	virtual IOStream& operator <<(void* v)
	{
		output(to_string(v).c_str());
		return *this;
	}
};

#ifdef _DEBUG
class TestStream
{
public:
	virtual void flush() {}

	void output(const char* data)
	{
		std::cout << data << std::endl;
	}

	class Stream : public IOStreamMaker
	{
		TestStream* m_target;
	public:
		Stream(TestStream* target) : m_target(target) {}

		void output(const char* lpstrLog)
		{
			m_target->output(lpstrLog);
		}
	};
	Stream stream() { return Stream(this); }
};
#endif

_FStdEnd

#endif//__FLIB_OSTREAM_H__