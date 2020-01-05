// WebService.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <fstream>
#include <functional>
#include <cstdlib>
#include <string>
#ifdef _WIN32
#include <Windows.h>
#define ERROR_NO ::GetLastError()
#else
#include <dlfcn.h>
#define ERROR_NO errno
#endif 

int main()
{
#ifdef _WIN32
	TCHAR tszModule[MAX_PATH + 1] = { 0 };
	::GetModuleFileName(NULL, tszModule, MAX_PATH);
	size_t len = _tcslen(tszModule) - 1;
	while (len >= 0)
	{
		if (tszModule[len] == '\\')
		{
			tszModule[len] = 0x0;
			break;
		}
		len--;
	}

	TCHAR szDuiLibPath[2048] = { 0 };
	_stprintf(szDuiLibPath, _T("%s\\webservice.dll"), tszModule);
	HMODULE _plugin = ::LoadLibraryEx(szDuiLibPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
#elif defined(__APPLE_CC__)
	void* _plugin = dlopen("./libwebservice.dylib", RTLD_NOW);
#else
	void* _plugin = dlopen("./libwebservice.so", RTLD_NOW);
#endif

	if (!_plugin)
	{
		std::cerr << "加载webservice失败" << ERROR_NO << std::endl;
	}
	else
	{
		typedef void(*func_test)();
		func_test pfunc = nullptr;
#ifdef _WIN32
		pfunc = (func_test)::GetProcAddress((HMODULE)_plugin, "test_webservice");
#else
		pfunc = (func_test)::dlsym(_plugin, "test_webservice");
#endif
		if (!pfunc)
		{
			std::cerr << "加载test_webservice@webservice失败" << std::endl;
		}
		else
		{
			pfunc();
		}
	}

#ifdef _WIN32
	system("pause");
#endif

	return 0;
}

