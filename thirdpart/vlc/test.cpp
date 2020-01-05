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
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <pthread.h>
#define ERROR_NO errno
#if defined(__APPLE_CC__)
#include <libproc.h>
#endif 
#endif 

const char* MyGetModulePath()
{
#ifdef _WIN32
    static char modulepath[MAX_PATH] = { 0 };
    ::GetModuleFileNameA(NULL, modulepath, MAX_PATH);
    return modulepath;
#elif defined(__APPLE_CC__)
    pid_t pid = getpid();
    static char modulepath[PROC_PIDPATHINFO_MAXSIZE] = {0};
    int ret = proc_pidpath (pid, modulepath, sizeof(modulepath));
    if(ret <= 0) return NULL;
    return modulepath;
#else
    const int len = 1024;
	static char modulepath[len] = {0};
    if(readlink("/proc/self/exe", modulepath, len-1) <=0) return NULL;
    return modulepath;
#endif
}

void MySplitpath(const char* s, char* path, char* name, char* ext)
{
	if (path) memset(path, 0, strlen(path));
	if (name) memset(name, 0, strlen(name));
	if (ext) memset(ext, 0, strlen(ext));

	size_t n = strlen(s);
	char* p = (char*)s;
	p += n;
	bool skiped = false;
	while (p != s && *p != '.') {
		if (*p == '/' || *p == '\\'){
			skiped = true;
			break;
		}
		p--;
	}
	char pwholename[256] = { 0 };
	if (*p != '.' || skiped){
		strcpy(pwholename, s);
	}
	else{
		if (ext) strcpy(ext, p);
		memcpy(pwholename, s, p - s);
	}

	p = pwholename;
	p += n;
	while (p != pwholename && *p != '/' && *p != '\\') p--;
	if (*p == '/' || *p == '\\'){
		p++;
		if (name) strcpy(name, p);
		if (path) memcpy(path, pwholename, p - pwholename - 1);
	}
	else{
		if (name) strcpy(name, pwholename);
	}
}

int main()
{
    const char* dir = MyGetModulePath();
    char path[1024] = {0};
    MySplitpath(dir, path, NULL, NULL);
    printf("path = %s\n", path);
    std::string fpath = path;
#ifdef _WIN32
	void* _plugin = ::LoadLibraryA(".\\playcenter.dll");
#elif defined(__APPLE_CC__)
    fpath += "/libplaycenter.dylib";
	void* _plugin = dlopen(fpath.c_str(), RTLD_NOW);
#else
    fpath += "/libplaycenter.so";
	void* _plugin = dlopen(fpath.c_str(), RTLD_NOW);
#endif

	if (!_plugin)
	{
		std::cerr << "加载playcenter失败" << ERROR_NO << std::endl;
	}
	else
	{
		typedef void(*func_test)();
		func_test pfunc = nullptr;
#ifdef _WIN32
		pfunc = (func_test)::GetProcAddress((HMODULE)_plugin, "test_player");
#else
		pfunc = (func_test)::dlsym(_plugin, "test_player");
#endif
		if (!pfunc)
		{
			std::cerr << "加载test_player@playcenter失败" << std::endl;
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


