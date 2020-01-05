// demo.cpp: 定义控制台应用程序的入口点。
//
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
	void* _plugin = ::LoadLibraryA(".\\http_plugin.dll");
#elif defined(__APPLE_CC__)
	void* _plugin = dlopen("./libhttp_plugin.dylib", RTLD_NOW);
#else
	void* _plugin = dlopen("./libhttp_plugin.so", RTLD_NOW);
#endif

	if (!_plugin)
	{
		std::cerr << "加载http_plugin失败" << ERROR_NO << std::endl;
	}
	else
	{
		typedef void(*func_test)();
		func_test pfunc = nullptr;
#ifdef _WIN32
		pfunc = (func_test)::GetProcAddress((HMODULE)_plugin, "test_http");
#else
		pfunc = (func_test)::dlsym(_plugin, "test_http");
#endif
		if (!pfunc)
		{
			std::cerr << "加载test_http@http_plugin失败" << std::endl;
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


