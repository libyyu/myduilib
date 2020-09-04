#ifndef LUA_WINDOW_H
#define LUA_WINDOW_H
#pragma once 
#include "Utils/Window.h"
#include "base/LuaEnv.hpp"
#include <unordered_map>

namespace DuiLib
{
	class CLuaWindow :
		public CWin, 
		public INotifyUI, 
		public IMessageFilterUI,
		public IDialogBuilderCallback, 
		public IListCallbackUI
	{
		DECLARE_CLASS(CLuaWindow)
	public:
		CLuaWindow(LPCTSTR szName, UINT nStyle = CS_DBLCLKS);
		~CLuaWindow();
		LPCTSTR GetWindowClassName() const;
		UINT GetClassStyle() const;
		void OnFinalMessage(HWND hWnd);

		CPaintManagerUI* PaintMgr() const;
		LuaEnv* GetLuaEnv() const;
		void BindLuaScript();
		void UnBindLuaScript();
		CDuiString ToString() const;
	public:
		bool onUIEvent(void* param);
		bool onUIDestroy(void* param);
		bool onUISize(void* param);
		bool onUINotify(void* param);
		bool onUIPaint(void* param);
		void Notify(TNotifyUI &msg);
		LRESULT MessageHandler(UINT msg, WPARAM wparam, LPARAM lparam, bool &handled);
		CControlUI *CreateControl(LPCTSTR pstrClass);
		void GetItemText(CControlUI* pControl, int iIndex, int iSubItem, CDuiString& text);
	protected:
		LRESULT ProcessWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnCreate(UINT msg, WPARAM wparam, LPARAM lparam, BOOL &handled);
		LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnSysCommand(UINT msg, WPARAM wparam, LPARAM lparam, BOOL &handled);
		LRESULT OnMenuCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		template <typename ... Args>
		inline bool CallLuaObject(const char* method, const Args &... args)
		{
			LuaEnv* env = GetLuaEnv();
			return env->doTableFunc(luaRef.index(), method, objRef, args...);
		}

		template <typename ... Args>
		inline bool CallLuaTable(const char* method, const Args &... args)
		{
			LuaEnv* env = GetLuaEnv();
			return env->doTableFunc(luaRef.index(), method, args...);
		}

		void GetNeedHandleMessage();
	private:
		const CDuiString szWindowName;
		const UINT nClassStyle;
		lua::reference luaRef;
		lua::reference objRef;
		std::unordered_map<UINT, bool> NeedHandleMessage;
		bool InitedMessageList;
	};
}


#endif//LUA_WINDOW_H