//#include "EntryPoint.h"
#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>
#include <tchar.h>
#include <stdio.h>

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
	HMODULE hMod = NULL;
	EntryFunc pFunc = GetEntryPoint(hInstance, _T("LuaDuiLib"), _T("RunByLua"), hMod);
	if(!pFunc)
	{
		return -1;
	}
	int ret = pFunc(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

	::FreeLibrary(hMod);
	return ret;
}