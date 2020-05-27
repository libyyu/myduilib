#include "StdAfx.h"
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/locking.h>
#include <share.h>
#include <fcntl.h>

extern HANDLE ghModuleDuiLib;
namespace DuiLib
{
	CSystem* CSystem::Instance()
	{
		static CSystem instance;
		return &instance;
	}
	CSystem::CSystem()
	{}
	CSystem::~CSystem()
	{
		for (int i = 0; i < _plugins.GetSize(); ++i)
		{
			HPLUGINS hDLL = (HPLUGINS)_plugins.GetAt(i);
			if (hDLL)
			{
				FreePlugin(hDLL);
			}
		}
		_plugins.Empty();
	}
	bool CSystem::HasLoad(LPCTSTR lpszPluginName)
	{
		HPLUGINS hDll = (HPLUGINS)_plugins.Find(lpszPluginName);
		if (hDll != NULL)
		{
			return true;
		}
		return false;
	}
	HPLUGINS CSystem::LoadPlugin(LPCTSTR lpszPluginName, BOOL mod)
	{
		HPLUGINS hDll = (HPLUGINS)_plugins.Find(lpszPluginName);
		if (hDll != NULL)
		{
			return hDll;
		}

		CDuiString szPathDll(lpszPluginName);
		if (szPathDll.Find(_T("/")) == -1 || szPathDll.Find(_T("\\")) == -1)
		{
			TCHAR szPath[MAX_PATH] = { 0 };
			::GetModuleFileName((HMODULE)ghModuleDuiLib, szPath, MAX_PATH);
			CDuiString strPath = Path::GetDirectoryName(szPath);
			szPathDll = Path::CombinePath(strPath, lpszPluginName);
		}
		if(szPathDll.Right(4) != _T(".dll"))
			szPathDll += _T(".dll");
		if (!mod)
			hDll = (HPLUGINS)::LoadLibrary(szPathDll);
		else
			hDll =(HPLUGINS)::GetModuleHandle(szPathDll);

		_plugins.Insert(lpszPluginName, hDll);
		return hDll;
	}
	void CSystem::FreePlugin(HPLUGINS hPlugin)
	{
		if (hPlugin) ::FreeLibrary((HMODULE)hPlugin);
	}
	void CSystem::FreePlugin(LPCTSTR lpszPluginName)
	{
		HPLUGINS hDll = (HPLUGINS)_plugins.Find(lpszPluginName);
		FreePlugin(hDll);
	}

	DWORD CSystem::GetDllVersion(LPCTSTR lpszDllName)
	{
		HINSTANCE hinstDll;
		DWORD dwVersion = 0;

		/* For security purposes, LoadLibrary should be provided with a fully-qualified
		path to the DLL. The lpszDllName variable should be tested to ensure that it
		is a fully qualified path before it is used. */
		hinstDll = ::LoadLibrary(lpszDllName);

		if (hinstDll)
		{
			DLLGETVERSIONPROC pDllGetVersion;
			pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hinstDll, "DllGetVersion");

			/* Because some DLLs might not implement this function, you must test for
			it explicitly. Depending on the particular DLL, the lack of a DllGetVersion
			function can be a useful indicator of the version. */

			if (pDllGetVersion)
			{
				DLLVERSIONINFO2 dvi;
				HRESULT hr;

				ZeroMemory(&dvi, sizeof(dvi));
				dvi.info1.cbSize = sizeof(dvi);

				hr = (*pDllGetVersion)(&dvi.info1);

				if (SUCCEEDED(hr))
				{
					dwVersion = MAKELONG(dvi.info1.dwMinorVersion, dvi.info1.dwMajorVersion);
				}
			}
			::FreeLibrary(hinstDll);
		}
		return dwVersion;
	}

	bool CSystem::IsWow64()
	{
		HMODULE hModule = GetModuleHandle(_T("kernel32"));
		if (!hModule) return FALSE;
		typedef BOOL(WINAPI * LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
		LPFN_ISWOW64PROCESS fnIsWow64Process;
		BOOL bIsWow64 = FALSE;
		fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(hModule, "IsWow64Process");
		if (NULL != fnIsWow64Process)
		{
			fnIsWow64Process(GetCurrentProcess(), &bIsWow64);
		}
		return bIsWow64;
	}

	bool CSystem::IsWindow7()
	{
		OSVERSIONINFO ovi = { sizeof(OSVERSIONINFO) };
		BOOL bRet = ::GetVersionEx(&ovi);
		return ((bRet != FALSE) && (ovi.dwMajorVersion == 6) && (ovi.dwMinorVersion >= 1));
	}
	bool CSystem::IsVista()
	{
		OSVERSIONINFO ovi = { sizeof(OSVERSIONINFO) };
		BOOL bRet = ::GetVersionEx(&ovi);
		return ((bRet != FALSE) && (ovi.dwMajorVersion >= 6));
	}
	bool CSystem::IsAdmin()
	{
		BOOL bIsElevated = FALSE;
		HANDLE hToken = NULL;
		UINT16 uWinVer = LOWORD(GetVersion());
		uWinVer = MAKEWORD(HIBYTE(uWinVer), LOBYTE(uWinVer));

		if (uWinVer < 0x0600)
			return FALSE;

		if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
		{
			struct {
				DWORD TokenIsElevated;
			}te;
			DWORD dwReturnLength = 0;

			if (GetTokenInformation(hToken,/*TokenElevation*/(_TOKEN_INFORMATION_CLASS)20, &te, sizeof(te), &dwReturnLength)) {
				if (dwReturnLength == sizeof(te))
					bIsElevated = te.TokenIsElevated;
			}
			CloseHandle(hToken);
		}
		return bIsElevated;
	}

	bool CSystem::OpenUrl(LPCTSTR lpUrl)
	{
		ASSERT(lpUrl);
		SHELLEXECUTEINFO ShellInfo = { 0 };
		ShellInfo.cbSize = sizeof(ShellInfo);
		ShellInfo.hwnd = NULL;
		ShellInfo.lpVerb = _T("open");
		ShellInfo.lpFile = lpUrl;
		ShellInfo.nShow = SW_SHOWNORMAL;
		ShellInfo.fMask = SEE_MASK_NOCLOSEPROCESS;

		return ::ShellExecuteEx(&ShellInfo);
	}

	bool CSystem::ReStartApp(LPCTSTR lpCmd, BOOL isAdmin/* = FALSE*/)
	{
		TCHAR s_exeFileName[MAX_PATH] = { 0 };
		DWORD n_result = ::GetModuleFileName(NULL, s_exeFileName, sizeof(TCHAR) * MAX_PATH);

		SHELLEXECUTEINFO ShellInfo = { 0 };
		ShellInfo.cbSize = sizeof(ShellInfo);
		ShellInfo.hwnd = NULL;
		ShellInfo.lpVerb = ((this->IsVista() && isAdmin) ? _T("runas") : _T("open"));
		ShellInfo.lpFile = s_exeFileName;
		ShellInfo.nShow = SW_SHOWNORMAL;
		ShellInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShellInfo.lpParameters = lpCmd;

		return ::ShellExecuteEx(&ShellInfo);
	}

	dui_uint32 CSystem::HKF2SDK(dui_ushort dwValue)
	{
		return ((dwValue & HOTKEYF_ALT) ? MOD_ALT : 0) \
			| ((dwValue & HOTKEYF_CONTROL) ? MOD_CONTROL : 0) \
			| ((dwValue & HOTKEYF_SHIFT) ? MOD_SHIFT : 0);
	}
	dui_uint32 CSystem::SDK2HKF(dui_ushort dwValue)
	{
		return ((dwValue & MOD_ALT) ? HOTKEYF_ALT : 0) \
			| ((dwValue & MOD_CONTROL) ? HOTKEYF_CONTROL : 0) \
			| ((dwValue & MOD_SHIFT) ? HOTKEYF_SHIFT : 0);
	}

	bool CSystem::IsFileUsed(LPCTSTR lpszFileName)
	{
		bool ret = false;
		int fh = _tsopen(lpszFileName, _O_WRONLY, _SH_DENYRW, _S_IWRITE);
		if (-1 == fh)
			ret = true;
		else
			_close(fh);
		return ret;
	}
}