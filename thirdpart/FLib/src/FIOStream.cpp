#define FLIB_DLL
#include "FIOStream.h"
#if FLIB_COMPILER_WINDOWS
#include <Windows.h>
#endif
_FStdBegin
std::string wide_to_multi(const wchar_t* c)
{
	assert(c);
	setlocale(LC_ALL, "");
	std::wstring str(c);
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(_WIN64) || defined(WIN64) || defined(__WIN64__)
	int size = WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
#else
	size_t size = wcstombs(NULL, str.c_str(), 0);
#endif

	std::string mbstr(size, char(0));
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(_WIN64) || defined(WIN64) || defined(__WIN64__)
	WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, const_cast<char*>(mbstr.c_str()), size, NULL, NULL);
#else
	wcstombs(const_cast<char*>(mbstr.c_str()), const_cast<wchar_t*>(str.c_str()), (size + 1) * 4);
#endif
	return mbstr;
}

std::wstring multi_to_wide(const char* c)
{
	assert(c);
	std::string str(c);
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(_WIN64) || defined(WIN64) || defined(__WIN64__)
	int size = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0) - 1;
#else
	size_t size = mbstowcs(NULL, str.c_str(), 0);
#endif

	std::wstring wstr(size, wchar_t(0));
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(_WIN64) || defined(WIN64) || defined(__WIN64__)
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), const_cast<wchar_t*>(wstr.c_str()), size);
#else
	mbstowcs(const_cast<wchar_t*>(wstr.c_str()), str.c_str(), size);
#endif
	return wstr;
}

//FOStream& operator <<(FOStream& t, const std::string& v)
//{
//	t << v.c_str();
//	return t;
//}
//FOStream& operator <<(FOStream& t, const std::wstring& v)
//{
//	t << v.c_str();
//	return t;
//}
//
//FIStream& operator >>(FIStream& t, std::string& v)
//{
//	const char* dst = v.c_str();
//	t >> dst;
//	return t;
//}
//FIStream& operator >>(FIStream& t, std::wstring& v)
//{
//	const wchar_t* dst = v.c_str();
//	t >> dst;
//	return t;
//}

_FStdEnd