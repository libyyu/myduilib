#ifndef __OSTREAM_H__
#define __OSTREAM_H__

#pragma once

#include <string>

namespace DuiLib
{
	class IOStream;
	typedef IOStream& (*StreamSymbol_t)(IOStream& Target);

	class DUILIB_API IOStream
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
		virtual IOStream& operator <<(char []) = 0;
		virtual IOStream& operator <<(const char*) = 0;
		virtual IOStream& operator <<(void*) = 0;
		IOStream& operator <<(StreamSymbol_t v) { return v(*this); }
		virtual void flush() = 0;
	};

	class DUILIB_API IOStreamMaker : public IOStream
	{
		virtual void output(const char* data) = 0;
	public:
		virtual void flush() {}
	public:
		using IOStream::operator <<;
		virtual IOStream& operator <<(char);
		virtual IOStream& operator <<(unsigned char);
		virtual IOStream& operator <<(short);
		virtual IOStream& operator <<(unsigned short);
		virtual IOStream& operator <<(int);
		virtual IOStream& operator <<(unsigned int);
		virtual IOStream& operator <<(long);
		virtual IOStream& operator <<(unsigned long);
		virtual IOStream& operator <<(int64_t);
		virtual IOStream& operator <<(uint64_t);
		virtual IOStream& operator <<(float);
		virtual IOStream& operator <<(double);
		virtual IOStream& operator <<(bool);
		virtual IOStream& operator <<(char[]);
		virtual IOStream& operator <<(const char*);
		virtual IOStream& operator <<(void*);
	};

	class DUILIB_API Convert
	{
	public:
		static std::string MBToUTF8(const char* pmb, int mLen = -1);
		static std::string UTF8ToMB(const char* pu8, int utf8Len = -1);
		static std::wstring MBToUnicode(const char* pmb, int mLen = -1);
		static std::string UnicodeToMB(const wchar_t* pun, int uLen = -1);
		static std::wstring UTF8ToUnicode(const char* pu8, int utf8Len = -1);
		static std::string UnicodeToUTF8(const wchar_t* pun, int uLen = -1);
		static std::string ToUTF8(LPCTSTR pstr);
	};

	template<typename Type>
	inline std::string to_string(Type);

	template<>
	inline std::string to_string(char c)
	{
		char buf[10] = { 0 };
		sprintf_s(buf, "%c", c);
		std::string result = buf;
		return result;
	}
	template<>
	inline std::string to_string(unsigned char c)
	{
		char buf[10] = { 0 };
		sprintf_s(buf, "%c", c);
		std::string result = buf;
		return result;
	}

	template<>
	inline std::string to_string(short c)
	{
		char buf[10] = { 0 };
		sprintf_s(buf, "%d", c);
		std::string result = buf;
		return result;
	}
	template<>
	inline std::string to_string(unsigned short c)
	{
		char buf[10] = { 0 };
		sprintf_s(buf, "%d", c);
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
	inline std::string to_string(const char c[])
	{
		std::string result(c);
		return result;
	}
	template<>
	inline std::string to_string(char c[])
	{
		std::string result(c);
		return result;
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
	inline std::string to_string(const wchar_t c[])
	{
		std::string result = Convert::UnicodeToUTF8(c, -1);
		return result;
	}

	template<>
	inline std::string to_string(wchar_t c[])
	{
		std::string result = Convert::UnicodeToUTF8(c, -1);
		return result;
	}
	template<>
	inline std::string to_string(const wchar_t* c)
	{
		std::string result = Convert::UnicodeToUTF8(c, -1);
		return result;
	}
	template<>
	inline std::string to_string(wchar_t* c)
	{
		std::string result = Convert::UnicodeToUTF8(c, -1);
		return result;
	}


	template<>
	inline std::string to_string(void* p)
	{
		char buf[10] = { 0 };
		sprintf_s(buf, "%p", p);
		std::string result = buf;
		return result;
	}

	template<typename Type>
	inline std::wstring to_wstring(Type);

	template<>
	inline std::wstring to_wstring(wchar_t c)
	{
		wchar_t buf[10] = { 0 };
		wsprintfW(buf, L"%c", c);
		std::wstring result = buf;
		return result;
	}

	template<>
	inline std::wstring to_wstring(short c)
	{
		wchar_t buf[10] = { 0 };
		wsprintfW(buf, L"%d", c);
		std::wstring result = buf;
		return result;
	}
	template<>
	inline std::wstring to_wstring(unsigned short c)
	{
		wchar_t buf[10] = { 0 };
		wsprintfW(buf, L"%d", c);
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
		return to_wstring<wchar_t>((wchar_t)c);
	}

	template<>
	inline std::wstring to_wstring(const wchar_t c[])
	{
		std::wstring result(c);
		return result;
	}
	template<>
	inline std::wstring to_wstring(wchar_t c[])
	{
		std::wstring result(c);
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

	template<>
	inline std::wstring to_wstring(const char c[])
	{
		std::wstring result = Convert::UTF8ToUnicode(c, -1);
		return result;
	}
	template<>
	inline std::wstring to_wstring(char c[])
	{
		std::wstring result = Convert::UTF8ToUnicode(c, -1);
		return result;
	}
	template<>
	inline std::wstring to_wstring(const char* c)
	{
		std::wstring result = Convert::UTF8ToUnicode(c, -1);
		return result;
	}
	template<>
	inline std::wstring to_wstring(char* c)
	{
		std::wstring result = Convert::UTF8ToUnicode(c, -1);
		return result;
	}

	template<>
	inline std::wstring to_wstring(void* p)
	{
		wchar_t buf[10] = { 0 };
		wsprintfW(buf, L"%p", p);
		std::wstring result = buf;
		return result;
	}

	class TestStream : public IOStreamMaker
	{
	public:
		TestStream() {	}
		void output(const char* data)
		{
			OutputDebugStringA(data);
		}
	};
}

#ifdef UNICODE
#define TODUISTRING  DuiLib::to_wstring
#else
#define TODUISTRING  DuiLib::to_string
#endif // UNICODE

#endif//__OSTREAM_H__