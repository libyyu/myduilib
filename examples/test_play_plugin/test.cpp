#include <iostream>
#include <string>
#include <fstream>
#include <functional>
#include <cstdlib>
#include <string>
#include <cassert>
#include <vector>
#include <algorithm>
#include <thread>
#ifdef _WIN32
#include <tchar.h>
#include <Windows.h>
#include <direct.h>
#include <io.h>
#define ERROR_NO ::GetLastError()
#else
#include <dlfcn.h>
#define ERROR_NO errno
#endif 
#include "test.h"

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

typedef char str_ansi;
typedef unsigned char str_utf8;
typedef wchar_t str_utf16;
namespace
{
str_ansi * GetANSIFromUTF16(const str_utf16 * pUTF16)
{
	const int nCharacters = pUTF16 ? wcslen(pUTF16) : 0;
	if (nCharacters < 0 || nCharacters > 1024 * 1024)
		return NULL;
	#ifdef _WIN32
		int nANSICharacters = (2 * nCharacters);
		str_ansi* pANSI = new str_ansi[nANSICharacters + 1];
		memset(pANSI, 0, (nANSICharacters + 1) * sizeof(str_ansi));
		if (pUTF16)
			WideCharToMultiByte(CP_ACP, 0, pUTF16, -1, pANSI, nANSICharacters, NULL, NULL);
	#else
		str_utf8* pANSI = new str_utf8[nCharacters + 1];
		for (int z = 0; z < nCharacters; z++)
			pANSI[z] = (pUTF16[z] >= 256) ? '?' : (str_utf8)pUTF16[z];
		pANSI[nCharacters] = 0;
	#endif

	return (str_ansi*)pANSI;
}

str_utf16 * GetUTF16FromANSI(const str_ansi * pANSI)
{
	const int nCharacters = pANSI ? strlen(pANSI) : 0;
	str_utf16* pUTF16 = new str_utf16[nCharacters + 1];

	#ifdef _WIN32
		memset(pUTF16, 0, sizeof(str_utf16) * (nCharacters + 1));
		if (pANSI)
			MultiByteToWideChar(CP_ACP, 0, pANSI, -1, pUTF16, nCharacters);
	#else
		for (int z = 0; z < nCharacters; z++)
			pUTF16[z] = (str_utf16)((str_utf8)pANSI[z]);
		pUTF16[nCharacters] = 0;
	#endif

	return pUTF16;
}

str_utf16 * GetUTF16FromUTF8(const str_utf8 * pUTF8)
{
	// get the length
	int nCharacters = 0; int nIndex = 0;
	while (pUTF8[nIndex] != 0)
	{
		if ((pUTF8[nIndex] & 0x80) == 0)
			nIndex += 1;
		else if ((pUTF8[nIndex] & 0xE0) == 0xE0)
			nIndex += 3;
		else
			nIndex += 2;

		nCharacters += 1;
	}

	// make a UTF-16 string
	str_utf16* pUTF16 = new str_utf16[nCharacters + 1];
	nIndex = 0; nCharacters = 0;
	while (pUTF8[nIndex] != 0)
	{
		if ((pUTF8[nIndex] & 0x80) == 0)
		{
			pUTF16[nCharacters] = pUTF8[nIndex];
			nIndex += 1;
		}
		else if ((pUTF8[nIndex] & 0xE0) == 0xE0)
		{
			pUTF16[nCharacters] = ((pUTF8[nIndex] & 0x1F) << 12) | ((pUTF8[nIndex + 1] & 0x3F) << 6) | (pUTF8[nIndex + 2] & 0x3F);
			nIndex += 3;
		}
		else
		{
			pUTF16[nCharacters] = ((pUTF8[nIndex] & 0x3F) << 6) | (pUTF8[nIndex + 1] & 0x3F);
			nIndex += 2;
		}

		nCharacters += 1;
	}
	pUTF16[nCharacters] = 0;

	return pUTF16;
}

str_utf8 * GetUTF8FromUTF16(const str_utf16 * pUTF16)
{
	// get the size(s)
	int nCharacters = wcslen(pUTF16);
	int nUTF8Bytes = 0;
	for (int z = 0; z < nCharacters; z++)
	{
		if (pUTF16[z] < 0x0080)
			nUTF8Bytes += 1;
		else if (pUTF16[z] < 0x0800)
			nUTF8Bytes += 2;
		else
			nUTF8Bytes += 3;
	}

	// allocate a UTF-8 string
	str_utf8* pUTF8 = new str_utf8[nUTF8Bytes + 1];

	// create the UTF-8 string
	int nUTF8Index = 0;
	for (int z = 0; z < nCharacters; z++)
	{
		if (pUTF16[z] < 0x0080)
		{
			pUTF8[nUTF8Index++] = (str_utf8)pUTF16[z];
		}
		else if (pUTF16[z] < 0x0800)
		{
			pUTF8[nUTF8Index++] = 0xC0 | (pUTF16[z] >> 6);
			pUTF8[nUTF8Index++] = 0x80 | (pUTF16[z] & 0x3F);
		}
		else
		{
			pUTF8[nUTF8Index++] = 0xE0 | (pUTF16[z] >> 12);
			pUTF8[nUTF8Index++] = 0x80 | ((pUTF16[z] >> 6) & 0x3F);
			pUTF8[nUTF8Index++] = 0x80 | (pUTF16[z] & 0x3F);
		}
	}
	pUTF8[nUTF8Index++] = 0;

	// return the UTF-8 string
	return pUTF8;
}

str_utf8* GetUTF8FromANSI(const str_ansi* pANSI)
{
	str_utf16* pUTF16 = GetUTF16FromANSI(pANSI);
	str_utf8* pUTF8 = GetUTF8FromUTF16(pUTF16);
	delete[] pUTF16;
	return pUTF8;
}

str_ansi* GetANSIFromUTF8(const str_utf8* pUTF8)
{
	str_utf16* pUTF16 = GetUTF16FromUTF8(pUTF8);
	str_ansi* pANSI = GetANSIFromUTF16(pUTF16);
	delete[] pUTF16;
	return pANSI;
}


int EnumGetAllFiles(const char* path, bool reversal /*= true*/, const std::function<void(const char*, bool)>& action/* = nullptr*/)
{
	assert(path);
	char fullpath[256] = { 0 };
	sprintf(fullpath, "%s\\*", path);
	_finddata_t finddata;
	intptr_t h = _findfirst(fullpath, &finddata);
	if (h == -1)
	{
		return -1;
	}

	int ret = 0;
	do {
		if ((finddata.attrib & _A_SUBDIR) == _A_SUBDIR)
		{
			if (strcmp(finddata.name, ".") != 0 && strcmp(finddata.name, "..") != 0)
			{
				char new_path[256] = { 0 };
				sprintf(new_path, "%s\\%s", path, finddata.name);
				if (action) action(new_path, true);
				if (reversal)
				{
					ret = EnumGetAllFiles(new_path, reversal, action);
					if (ret != 0)
					{
						break;
					}
				}
			}
		}
		else
		{
			char new_path[256] = { 0 };
			sprintf(new_path, "%s\\%s", path, finddata.name);
			if (action) action(new_path, false);
		}

		if (_findnext(h, &finddata) != 0)
		{
			break;
		}
	} while (true);

	_findclose(h);
	return ret;
}

}		//end namespace em_mp3_tag

int main()
{
#ifdef _WIN32
	void* _plugin = ::LoadLibraryA(".\\play_plugin.dll");
#elif defined(__APPLE_CC__)
	void* _plugin = dlopen("./play_plugin.dylib", RTLD_NOW);
#else
	void* _plugin = dlopen("./play_plugin.so", RTLD_NOW);
#endif

	if (!_plugin)
	{
		std::cerr << "加载play_plugin失败" << ERROR_NO << std::endl;
	}
	else
	{
		typedef int(*func_test)(const char*, const char*, int);
		func_test pfunc = nullptr;
#ifdef _WIN32
		pfunc = (func_test)::GetProcAddress((HMODULE)_plugin, "test_player");
#else
		pfunc = (func_test)::dlsym(_plugin, "test_player");
#endif
		if (!pfunc)
		{
			std::cerr << "加载test_player@play_plugin失败" << std::endl;
		}
		else
		{
			const str_ansi u8[] = "D:\\Workspace\\test\\t\\Debug\\birthday.mp3";// 相思.mp3";
			const str_utf16 u16[] = L"D:\\Workspace\\test\\t\\Debug\\相思.mp3";
			const TCHAR pwstr[] = _T("D:\\Workspace\\test\\t\\Debug\\相思.mp3");
			const char* urlpath = "http://www.libyyu.com/sound/happy-birthday";
			int ret = pfunc(u8, urlpath, -1);// "");
			
			if (ret == 0)
			{
				char ch;
				while ((ch = getchar()) == 'q')
					break;
			}
		}
#ifdef _WIN32
		::FreeLibrary((HMODULE)_plugin);
#elif defined(__APPLE_CC__)
#else
#endif
	}

#ifdef _WIN32
	system("pause");
#endif

	return 0;
}