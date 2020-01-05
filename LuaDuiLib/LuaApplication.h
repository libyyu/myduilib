#ifndef LUAAPPLICATION_H
#define LUAAPPLICATION_H
#pragma once

namespace DuiLib
{
	class LuaApplication : public Singleton<LuaApplication>
	{
		DECLARE_SINGLETON_CLASS(LuaApplication)
		LuaApplication();
	public:
		virtual ~LuaApplication();
		bool Run(HINSTANCE, HINSTANCE, LPSTR, int);
		bool ImportLuaDLL(LPCTSTR dllName, LPCTSTR dllEntry = NULL);
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
		CDuiPtrArray	 cmd_lines;
		CDuiStringPtrMap lua_plugins;
		bool             valid;
	};
}


#endif//LUAAPPLICATION_H