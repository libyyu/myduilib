#ifndef _UISYSTEM_H_
#define _UISYSTEM_H_
#pragma once
#include <Shlwapi.h>
namespace DuiLib
{
	typedef HMODULE  HPLUGINS;
	class UILIB_API CSystem
	{
		CSystem();
		CDuiStringPtrMap _plugins;
	public:
		virtual ~CSystem();
		static CSystem* Instance();
		bool HasLoad(LPCTSTR lpszPluginName);
		HPLUGINS LoadPlugin(LPCTSTR lpszPluginName, BOOL mod = FALSE);
		void FreePlugin(HPLUGINS hPlugin);
		void FreePlugin(LPCTSTR lpszPluginName);

		template<typename FUNC_TYPE>
		inline FUNC_TYPE GetFunction(HPLUGINS hPlugin, LPCTSTR method)
		{
			//DuiAssertX(hPlugin, _T("plugin is null."));
			FUNC_TYPE pFunc = (FUNC_TYPE)GetProcAddress(hPlugin, method);
			return pFunc;
		}

		DWORD GetDllVersion(LPCTSTR lpszDllName);

		bool IsWow64();
		bool IsWindow7();
		bool IsVista();
		bool IsAdmin();
		bool IsFileUsed(LPCTSTR lpszFileName);
		bool OpenUrl(LPCTSTR lpUrl);
		bool ReStartApp(LPCTSTR lpCmd, BOOL isAdmin = FALSE);

		dui_uint32 HKF2SDK(dui_ushort dwValue);
		dui_uint32 SDK2HKF(dui_ushort dwValue);
	};
}

#endif//_UISYSTEM_H_