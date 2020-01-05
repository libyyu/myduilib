//#include "EntryPoint.h"
#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <string>
#include <vector>

//#pragma comment( linker, "/subsystem:console /entry:WinMainCRTStartup" )

typedef int(*EntryFunc)(HINSTANCE, HINSTANCE, LPSTR, int);
EntryFunc GetEntryPoint(HINSTANCE hInstance, LPCTSTR DllName, LPCTSTR EntryName, HMODULE& mod)
{
	TCHAR tszModule[MAX_PATH + 1] = { 0 };
	::GetModuleFileName(hInstance, tszModule, MAX_PATH);
	size_t len = _tcslen(tszModule)-1;
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
	_stprintf(szDuiLibPath, _T("%s\\%s.dll"), tszModule, DllName);
	HMODULE hMod = ::LoadLibraryEx(szDuiLibPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (!hMod)
	{
		szDuiLibPath[0] = 0x0;
		_stprintf(szDuiLibPath, _T("%s\\%s.dll"), tszModule, DllName);
		HMODULE hMod = ::LoadLibraryEx(szDuiLibPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		if (!hMod)
		{
			TCHAR szError[2048] = { 0 };
			_stprintf(szError, _T("Failed to Load %s.dll, error = %d."), DllName, ::GetLastError());
			::MessageBox(NULL, szError, _T("Error"), 0);
			return NULL;
		}
	}

	EntryFunc pFunc = (EntryFunc)GetProcAddress(hMod, EntryName);
	if (!pFunc)
	{
		TCHAR szError[2048] = { 0 };
		_stprintf(szError, _T("Failed to Load %s@%s.dll."), EntryName, DllName);
		::MessageBox(NULL, szError, _T("Error"), 0);
		::FreeLibrary(hMod);
		return NULL;
	}
	mod = hMod;
	return pFunc;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _UNICODE
	std::wstring cmdLine(lpCmdLine);
	std::vector<std::wstring> cmds;
	std::wstring param;
#else
	std::string cmdLine(lpCmdLine);
	std::vector<std::string> cmds;
	std::string param;
#endif
	printf("%s\n", lpCmdLine);
	for(size_t i=0; i<cmdLine.length(); ++i)
	{
		if(cmdLine[i] != ' ')
		{
			param += cmdLine[i];
		}	
		else
		{
			if(param.length() >0)
			{
				printf("%s\n", param.c_str());
				cmds.push_back(param);
				param.clear();
			}
		}
	}
	if(param.length() >0)
	{
		printf("%s\n", param.c_str());
		cmds.push_back(param);
		param.clear();
	}

	TCHAR szDllName[256] = {0};
	TCHAR szEntryName[256] = {0};
	if(cmds.size() >1)
	{
		_stprintf(szDllName, _T("%s"), cmds[0].c_str());
		_stprintf(szEntryName, _T("%s"), cmds[1].c_str());
	}
	else
	{
		_stprintf(szDllName, _T("%s"), _T("ConvexBrowser"));
		_stprintf(szEntryName, _T("%s"), _T("RunConvex"));
	}
	HMODULE hMod = NULL;
	EntryFunc pFunc = GetEntryPoint(hInstance, szDllName, szEntryName, hMod);
	if(!pFunc)
	{
		return -1;
	}
	int ret = pFunc(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

	::FreeLibrary(hMod);
	return ret;
}