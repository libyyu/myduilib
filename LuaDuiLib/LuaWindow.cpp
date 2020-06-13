#include "UIlib.h"
#include "LuaWindow.h"
#include "base/lua_wrapper.hpp"
#include "lua_duilib_wrapper.hpp"
#include "lua_duilib_extend_wrapper.hpp"
#ifdef USE_CUSTOM_MEMORY
#define new FLIB_NEW
#endif
extern LuaEnv* globalLuaEnv;

namespace DuiLib
{
	//static TNotifyUI ToLuaMsgNotify;
	CLuaWindow::CLuaWindow(LPCTSTR szName, UINT nStyle/* = CS_DBLCLKS*/)
		: szWindowName(szName), 
		nClassStyle(nStyle), 
		luaRef(LUA_NOREF), 
		objRef(LUA_NOREF),
		InitedMessageList(false),
		CWin()
	{
	}
	CLuaWindow::~CLuaWindow()
	{
		UnBindLuaScript();
		InitedMessageList = false;
	}

	LPCTSTR CLuaWindow::GetWindowClassName() const { return szWindowName; }
	UINT CLuaWindow::GetClassStyle() const { return nClassStyle; }
	void CLuaWindow::OnFinalMessage(HWND hWnd)
	{
		PaintMgr()->RemovePreMessageFilter(this);
		PaintMgr()->RemoveNotifier(this);
		PaintMgr()->ReapObjects(PaintMgr()->GetRoot());

		CallLuaObject("OnFinalMessage");

		this->SetValid(FALSE);
		delete this;
	}
	CPaintManagerUI* CLuaWindow::PaintMgr() const { return GetPaintMgr(); }

	CDuiString CLuaWindow::ToString() const
	{
		CDuiString s = _T("CLuaWindow:");
		s += szWindowName;
		return s;
	}

	LuaEnv* CLuaWindow::GetLuaEnv() const
	{
		return globalLuaEnv;
	}

	void CLuaWindow::UnBindLuaScript()
	{
		lua_State* l = *(GetLuaEnv());
		if (l)
		{
			if (luaRef != LUA_NOREF)
			{
				luaL_unref(l, LUA_REGISTRYINDEX, luaRef);
				luaRef = LUA_NOREF;
			}
			if (objRef != LUA_NOREF)
			{
				luaL_unref(l, LUA_REGISTRYINDEX, objRef);
				objRef = LUA_NOREF;
			}
		}
	}

	void CLuaWindow::BindLuaScript()
	{
		lua_State* l = *(GetLuaEnv());
		UnBindLuaScript();
		lua_pushvalue(l, 2);
		luaRef = luaL_ref(l, LUA_REGISTRYINDEX);
		lua_pushvalue(l, 3);
		objRef = luaL_ref(l, LUA_REGISTRYINDEX);
	}

	bool CLuaWindow::onUIEvent(void* param)
	{
		if (luaRef == LUA_NOREF || objRef == LUA_NOREF)
			return true;

		LuaEnv* env = GetLuaEnv();
		lua::stack_gurad g(*env);

		const TEventUI* pEvent = static_cast<TEventUI*>(param);
		if (CallLuaObject("OnUIEvent", pEvent))
		{
			if (lua_isboolean(*GetLuaEnv(), -1))
			{
				bool ret;
				lua::pop(*GetLuaEnv(), &ret);
				return ret;
			}
		}
		return true;
	}

	bool CLuaWindow::onUIDestroy(void* param)
	{
		if (luaRef == LUA_NOREF || objRef == LUA_NOREF)
			return true;

		LuaEnv* env = GetLuaEnv();
		lua::stack_gurad g(*env);

		CControlUI* pControl = static_cast<CControlUI*>(param);
		if (CallLuaObject("OnUIDestroy", pControl))
		{
			if (lua_isboolean(*GetLuaEnv(), -1))
			{
				bool ret;
				lua::pop(*GetLuaEnv(), &ret);
				return ret;
			}
		}
		return false;
	}

	bool CLuaWindow::onUISize(void* param)
	{
		if (luaRef == LUA_NOREF || objRef == LUA_NOREF)
			return true;

		LuaEnv* env = GetLuaEnv();
		lua::stack_gurad g(*env);

		CControlUI* pControl = static_cast<CControlUI*>(param);
		if (CallLuaObject("OnUISize", pControl))
		{
			if (lua_isboolean(*GetLuaEnv(), -1))
			{
				bool ret;
				lua::pop(*GetLuaEnv(), &ret);
				return ret;
			}
		}
		return false;
	}
	bool CLuaWindow::onUINotify(void* param)
	{
		if (luaRef == LUA_NOREF || objRef == LUA_NOREF)
			return true;
		
		LuaEnv* env = GetLuaEnv();
		lua::stack_gurad g(*env);

		const TNotifyUI* pNotify = static_cast<TNotifyUI*>(param);
		if (CallLuaObject("OnUINotify", pNotify))
		{
			if (lua_isboolean(*GetLuaEnv(), -1))
			{
				bool ret;
				lua::pop(*GetLuaEnv(), &ret);
				return ret;
			}
		}
		return false;
	}
	bool CLuaWindow::onUIPaint(void* param)
	{
		if (luaRef == LUA_NOREF || objRef == LUA_NOREF)
			return true;
		
		LuaEnv* env = GetLuaEnv();
		lua::stack_gurad g(*env);

		CControlUI* pControl = static_cast<CControlUI*>(param);
		if (CallLuaObject("OnUIPaint", pControl))
		{
			if (lua_isboolean(*GetLuaEnv(), -1))
			{
				bool ret;
				lua::pop(*GetLuaEnv(), &ret);
				return ret;
			}
		}
		return false;
	}

	void CLuaWindow::Notify(TNotifyUI &msg)
	{
		if (luaRef == LUA_NOREF || objRef == LUA_NOREF)
			return;
		LuaEnv* env = GetLuaEnv();
		lua::stack_gurad g(*env);
		//ToLuaMsgNotify.dwTimestamp = msg.dwTimestamp;
		//ToLuaMsgNotify.lParam = msg.lParam;
		//ToLuaMsgNotify.pSender = msg.pSender;
		//ToLuaMsgNotify.ptMouse = msg.ptMouse;
		//ToLuaMsgNotify.sType = msg.sType;
		//ToLuaMsgNotify.sVirtualWnd = msg.sVirtualWnd;
		//ToLuaMsgNotify.wParam = msg.wParam;
		if (CallLuaObject("OnNotify", &msg))
		{
			
		}
	}
	LRESULT CLuaWindow::MessageHandler(UINT msg, WPARAM wparam, LPARAM lparam, bool &handled)
	{
		if (luaRef == LUA_NOREF || objRef == LUA_NOREF)
			return FALSE;
		if (!InitedMessageList)
		{
			GetNeedHandleMessage();
			InitedMessageList = true;
		}
		
		if (NeedHandleMessage.count(msg) == 0 || !NeedHandleMessage[msg])
		{
			return FALSE;
		}
		LuaEnv* env = GetLuaEnv();
		lua::stack_gurad g(*env);
		if (CallLuaObject("MessageHandler", msg, wparam, lparam))
		{
			bool b;
			lua::pop(*GetLuaEnv(), &b);
			handled = b;
		}
		return FALSE;
	}
	CControlUI * CLuaWindow::CreateControl(LPCTSTR pstrClass)
	{
		if (luaRef == LUA_NOREF || objRef == LUA_NOREF)
			return NULL;
		LuaEnv* env = GetLuaEnv();
		lua::stack_gurad g(*env);
		CControlUI* pControl = nullptr;
		if (CallLuaObject("CreateControl", pstrClass))
		{
			lua::pop(*GetLuaEnv(), &pControl);
		}
		return pControl;
	}

	void CLuaWindow::GetItemText(CControlUI* pControl, int iIndex, int iSubItem, CDuiString& text)
	{
		if (luaRef == LUA_NOREF || objRef == LUA_NOREF)
			text = _T("");
		LuaEnv* env = GetLuaEnv();
		lua::stack_gurad g(*env);
		if (CallLuaObject("GetItemText", pControl, iIndex, iSubItem))
		{			
			lua::pop(*GetLuaEnv(), &text);
		}
	}

	void CLuaWindow::GetNeedHandleMessage()
	{
		if (luaRef == LUA_NOREF || objRef == LUA_NOREF)
			return;
		
		LuaEnv* env = GetLuaEnv();
		lua::stack_gurad g(*env);

		if (CallLuaObject("GetProcessWindowMessage"))
		{
			std::vector<int> message_list;
			lua::pop(*GetLuaEnv(), &message_list);

			for (auto& m : message_list)
			{
				if (NeedHandleMessage.count(m) > 0)
					NeedHandleMessage.emplace(m, true);
				else
					NeedHandleMessage.insert(std::pair<UINT, bool>(m, true));
			}
		}
	}

	LRESULT CLuaWindow::ProcessWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT hr = __super::ProcessWindowMessage(uMsg, wParam, lParam, bHandled);
		if (bHandled) return hr;
		if (luaRef == LUA_NOREF || objRef == LUA_NOREF)
			return FALSE;
		if (!InitedMessageList)
		{
			GetNeedHandleMessage();
			InitedMessageList = true;
		}

		if (NeedHandleMessage.count(uMsg) == 0 || !NeedHandleMessage[uMsg])
		{
			return FALSE;
		}
		LuaEnv* env = GetLuaEnv();
		lua::stack_gurad g(*env);
		if (CallLuaObject("ProcessWindowMessage", uMsg, wParam, lParam))
		{
			bool b;
			lua::pop(*GetLuaEnv(), &b);
			bHandled = b;
		}
		return FALSE;
	}
	LRESULT CLuaWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		__super::OnCreate(uMsg, wParam, lParam, bHandled);
		
		if (luaRef == LUA_NOREF || objRef == LUA_NOREF)
			return FALSE;
		LuaEnv* env = GetLuaEnv();
		lua::stack_gurad g(*env);
		if (CallLuaObject("OnCreate", wParam, lParam))
		{
			bHandled = TRUE;
		}

		GetPaintMgr()->AddPreMessageFilter(this);
		GetPaintMgr()->AddMessageFilter(this);
		GetPaintMgr()->AddNotifier(this);

		RECT rcClient;
		::GetClientRect(*this, &rcClient);
		::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
			rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

		CenterWindow();

		return TRUE;
	}
	LRESULT CLuaWindow::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		__super::OnDestroy(uMsg, wParam, lParam, bHandled);
		
		if (luaRef == LUA_NOREF || objRef == LUA_NOREF)
			return TRUE;
		LuaEnv* env = GetLuaEnv();
		lua::stack_gurad g(*env);
		if (CallLuaObject("OnDestroy", wParam, lParam))
		{
			bHandled = TRUE;
		}
		return TRUE;
	}
	LRESULT CLuaWindow::OnSysCommand(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		LRESULT ret = __super::OnSysCommand(msg, wParam, lParam, bHandled);
		if (luaRef == LUA_NOREF || objRef == LUA_NOREF)
			return ret;
		LuaEnv* env = GetLuaEnv();
		lua::stack_gurad g(*env);
		if (CallLuaObject("OnSysCommand", wParam, lParam))
		{
			bHandled = TRUE;
		}
		return ret;
	}

	LRESULT CLuaWindow::OnMenuCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LuaEnv* env = GetLuaEnv();
		lua::stack_gurad g(*env);
		MenuCmdMsg* pMsg = (MenuCmdMsg*)wParam;
		if (CallLuaObject("OnMenuCommand", pMsg))
		{
			bHandled = TRUE;
		}
		delete pMsg;
		return 0;
	}
}