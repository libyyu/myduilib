#ifndef LUAAPPLICATION_H
#define LUAAPPLICATION_H
#pragma once
#include "LuaWindow.h"
namespace DuiLib
{
	class LuaApplication : public Singleton<LuaApplication>
	{
		DECLARE_SINGLETON_CLASS(LuaApplication)
		LuaApplication();
	public:
		CTimerSource     TimerSource;
		virtual ~LuaApplication();
		bool Run(HINSTANCE, HINSTANCE, LPSTR, int);
		bool ImportLuaDLL(LPCTSTR dllName, LPCTSTR dllEntry = NULL);
		void OnGlobalTimer(IDuiTimer*, HWND, CLuaWindow* , WPARAM );
		CLuaWindow* GetMainWindow() { return m_pMainWindow; }
		void SetMainWindow(CLuaWindow* pWindow) { m_pMainWindow = pWindow; }
	protected:
		bool Initialize();
		void Unitialize();
		bool RunInternal(HINSTANCE, HINSTANCE, LPSTR, int);
	private:
		bool StartupLua();
		bool ShutdownLua();
		bool RegisterScript();
		bool UnloadLuaDLL(LPCTSTR dllName);
	private:
		CLuaWindow*		 m_pMainWindow;
		CDuiPtrArray	 cmd_lines;
		CDuiStringPtrMap lua_plugins;
		bool             valid;
		bool			 exiting;
	};
}


#endif//LUAAPPLICATION_H