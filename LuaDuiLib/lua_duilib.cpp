#define LUA_LIB
#include "UIlib.h"
#include "LuaWindow.h"
#include "base/lua_wrapper.hpp"
#include "lua_duilib_wrapper.hpp"
#include "lua_duilib_extend_wrapper.hpp"
#include <algorithm>
#include <functional>

#ifdef USE_CUSTOM_MEMORY
#define new FLIB_NEW
#endif

#define LAMBDA_METHOD(x)   .def(#x, x)
#define LAMBDA_METHOD2(n, x) .def(n, x)

extern LuaEnv* globalLuaEnv;

namespace DuiLib
{
	class CDelegateStaticEx : public CDelegateBase
	{
		typedef std::function<bool(void*)> Fn;
		Fn pfunc;
	public:
		CDelegateStaticEx(Fn pFn) : pfunc(pFn), CDelegateBase((void*)(&pFn), NULL) { }
		CDelegateStaticEx(const CDelegateStaticEx& rhs) : pfunc(rhs.pfunc), CDelegateBase(rhs) { }
		virtual CDelegateBase* Copy() const { return new CDelegateStaticEx(*this); }
	protected:
		virtual bool Invoke(void* param)
		{
			return (pfunc)(param);
		}
	};

	class CLuaDelegateStaticEx : public CDelegateBase
	{
		typedef std::function<bool(void*, int)> Fn;
		Fn pfunc;
		int nLuaRef;
	public:
		CLuaDelegateStaticEx(Fn pFn, int LuaRef = LUA_NOREF) : pfunc(pFn), nLuaRef(LuaRef), CDelegateBase((void*)(&pFn), NULL) { }
		CLuaDelegateStaticEx(const CLuaDelegateStaticEx& rhs) : pfunc(rhs.pfunc), nLuaRef(rhs.nLuaRef), CDelegateBase(rhs) { }
		virtual CDelegateBase* Copy() const { return new CLuaDelegateStaticEx(*this); }
	protected:
		virtual bool Invoke(void* param)
		{
			return (pfunc)(param, nLuaRef);
		}
	};

	inline CDelegateStaticEx MakeDelegate(const std::function<bool(void*)>& pStdFn)
	{
		return CDelegateStaticEx(pStdFn);
	}

	inline CLuaDelegateStaticEx MakeDelegate(int r, const std::function<bool(void*, int)>& pStdFn)
	{
		return CLuaDelegateStaticEx(pStdFn, r);
	}
}

namespace DuiLib
{
	template<typename T>
	inline T* NewControl()
	{
		return new T();
	}

	template<typename T>
	inline void DeleteControl(void* p)
	{
		if(!p) return;
		//bool b = CDuiObjectMgr::Get().FindObject(p);
		//if(!b) return;
		//
		__try
		{
			T* pControl = static_cast<T*>(p);
			if (pControl) pControl->Delete();
		}
		__except(EXCEPTION_ACCESS_VIOLATION)
		{ 
		}
	}

	static CDuiString CControlUI_ToString(CControlUI* p)
	{
		return p->ToString();
	}

	static int CControlUI_SetText(lua_State* l)
	{
		CControlUI* pControl = nullptr;
		CDuiString strText;
		lua::get(l, 1, &pControl, &strText);
		pControl->SetText(strText);
		return 0;
	}
	static int CControlUI_SetToolTip(lua_State* l)
	{
		CControlUI* pControl = nullptr;
		CDuiString strText;
		lua::get(l, 1, &pControl, &strText);
		pControl->SetToolTip(strText);
		return 0;
	}
	static int CControlUI_SetUserData(lua_State* l)
	{
		CControlUI* pControl = nullptr;
		CDuiString strText;
		lua::get(l, 1, &pControl, &strText);
		pControl->SetUserData(strText);
		return 0;
	}
	static int CControlUI_DoEvent(lua_State* l)
	{
		CControlUI* pControl = nullptr;
		TEventUI* pEvent = nullptr;
		lua::get(l, 1, &pControl, &pEvent);
		if(pEvent) pControl->DoEvent(*pEvent);
		return 0;
	}

	static int CControlUI_OnInitAdd(lua_State* l)
	{
		CControlUI* pControl = nullptr;
		lua::get(l, 1, &pControl);
		lua_pushvalue(*globalLuaEnv, 2);
		int luaFunc = luaL_ref(*globalLuaEnv, LUA_REGISTRYINDEX);
		pControl->OnInit += MakeDelegate(luaFunc, [=](void* param, int luaFunc)->bool
		{
			if (!globalLuaEnv) return true;
			CControlUI* pTargetControl = (CControlUI*)param;
			lua::stack_gurad g(*globalLuaEnv);
			if (globalLuaEnv->doFunc(luaFunc, pTargetControl))
			{
				if (lua_isboolean(*globalLuaEnv, -1))
				{
					bool ret;
					lua::pop(*globalLuaEnv, &ret);
					return ret;
				}
			}
			return true;
		});
		return 0;
	}
	static int CControlUI_OnDestroyAdd(lua_State* l)
	{
		CControlUI* pControl = nullptr;
		lua::get(l, 1, &pControl);
		lua_pushvalue(*globalLuaEnv, 2);
		int luaFunc = luaL_ref(*globalLuaEnv, LUA_REGISTRYINDEX);
		pControl->OnDestroy += MakeDelegate(luaFunc, [=](void* param, int luaFunc)->bool
			{
				if (!globalLuaEnv) return true;
				CControlUI* pTargetControl = (CControlUI*)param;
				lua::stack_gurad g(*globalLuaEnv);
				if (globalLuaEnv->doFunc(luaFunc, pTargetControl))
				{
					if (lua_isboolean(*globalLuaEnv, -1))
					{
						bool ret;
						lua::pop(*globalLuaEnv, &ret);
						return ret;
					}
				}
				return true;
			});
		return 0;
	}
	static int CControlUI_OnSizeAdd(lua_State* l)
	{
		CControlUI* pControl = nullptr;
		lua::get(l, 1, &pControl);
		lua_pushvalue(*globalLuaEnv, 2);
		int luaFunc = luaL_ref(*globalLuaEnv, LUA_REGISTRYINDEX);
		pControl->OnSize += MakeDelegate(luaFunc, [=](void* param, int luaFunc)->bool
			{
				if (!globalLuaEnv) return true;
				CControlUI* pTargetControl = (CControlUI*)param;
				lua::stack_gurad g(*globalLuaEnv);
				if (globalLuaEnv->doFunc(luaFunc, pTargetControl))
				{
					if (lua_isboolean(*globalLuaEnv, -1))
					{
						bool ret;
						lua::pop(*globalLuaEnv, &ret);
						return ret;
					}
				}
				return true;
			});
		return 0;
	}
	static int CControlUI_OnEventAdd(lua_State* l)
	{
		CControlUI* pControl = nullptr;
		lua::get(l, 1, &pControl);
		lua_pushvalue(*globalLuaEnv, 2);
		int luaFunc = luaL_ref(*globalLuaEnv, LUA_REGISTRYINDEX);
		pControl->OnEvent += MakeDelegate(luaFunc, [=](void* param, int luaFunc)->bool
			{
				if (!globalLuaEnv) return true;
				TEventUI* pEvent = (TEventUI*)param;
				lua::stack_gurad g(*globalLuaEnv);
				if (globalLuaEnv->doFunc(luaFunc, pEvent))
				{
					if (lua_isboolean(*globalLuaEnv, -1))
					{
						bool ret;
						lua::pop(*globalLuaEnv, &ret);
						return ret;
					}
				}
				return true;
			});
		return 0;
	}
	static int CControlUI_OnNotifyAdd(lua_State* l)
	{
		CControlUI* pControl = nullptr;
		lua::get(l, 1, &pControl);
		lua_pushvalue(*globalLuaEnv, 2);
		int luaFunc = luaL_ref(*globalLuaEnv, LUA_REGISTRYINDEX);
		pControl->OnNotify += MakeDelegate(luaFunc, [=](void* param, int luaFunc)->bool
			{
				if (!globalLuaEnv) return true;
				TNotifyUI* pMsg = (TNotifyUI*)param;
				lua::stack_gurad g(*globalLuaEnv);
				if (globalLuaEnv->doFunc(luaFunc, pMsg))
				{
					if (lua_isboolean(*globalLuaEnv, -1))
					{
						bool ret;
						lua::pop(*globalLuaEnv, &ret);
						return ret;
					}
				}
				return true;
			});
		return 0;
	}
	static int CControlUI_OnPaintAdd(lua_State* l)
	{
		CControlUI* pControl = nullptr;
		lua::get(l, 1, &pControl);
		lua_pushvalue(*globalLuaEnv, 2);
		int luaFunc = luaL_ref(*globalLuaEnv, LUA_REGISTRYINDEX);
		pControl->OnPaint += MakeDelegate(luaFunc, [=](void* param, int luaFunc)->bool
			{
				if (!globalLuaEnv) return true;
				CControlUI* pTargetControl = (CControlUI*)param;
				lua::stack_gurad g(*globalLuaEnv);
				if (globalLuaEnv->doFunc(luaFunc, pTargetControl))
				{
					if (lua_isboolean(*globalLuaEnv, -1))
					{
						bool ret;
						lua::pop(*globalLuaEnv, &ret);
						return ret;
					}
				}
				return true;
			});
		return 0;
	}
	static int CControlUI_OnPostPaintAdd(lua_State* l)
	{
		CControlUI* pControl = nullptr;
		lua::get(l, 1, &pControl);
		lua_pushvalue(*globalLuaEnv, 2);
		int luaFunc = luaL_ref(*globalLuaEnv, LUA_REGISTRYINDEX);
		pControl->OnPostPaint += MakeDelegate(luaFunc, [=](void* param, int luaFunc)->bool
			{
				if (!globalLuaEnv) return true;
				CControlUI* pTargetControl = (CControlUI*)param;
				lua::stack_gurad g(*globalLuaEnv);
				if (globalLuaEnv->doFunc(luaFunc, pTargetControl))
				{
					if (lua_isboolean(*globalLuaEnv, -1))
					{
						bool ret;
						lua::pop(*globalLuaEnv, &ret);
						return ret;
					}
				}
				return true;
			});
		return 0;
	}

	///TODO: bug
	static int CControlUI_BindEventTo(lua_State* l)
	{
		CControlUI* pControl = nullptr;
		CControlUI* pOther = nullptr;
		lua::get(l, 1, &pControl, &pOther);
		pControl->OnEvent += MakeDelegate(-1, [pOther](void* param, int r)->bool
		{
			pOther->DoEvent(*(TEventUI*)param);
			return true;
		});
		return 0;
	}
	
	static int CControlUI_SetBorderSize(lua_State* l)
	{
		CControlUI* pControl = nullptr;
		lua::get(l, 1, &pControl);
		int t = lua_type(l, 2);
		if (t == LUA_TNUMBER)
		{
			int iSize;
			lua::get(l, 2, &iSize);
			pControl->SetBorderSize(iSize);
		}
		else
		{
			RECT rect;
			lua::get(l, 2, &rect);
			pControl->SetBorderSize(rect);
		}
		return 0;
	}

	static int CLabelUI_GetShadowOffset(lua_State* l)
	{
		CLabelUI* pLabel = nullptr;
		lua::get(l, 1, &pLabel);
		auto offset = pLabel->GetShadowOffset();
		lua::lua_table_ref_t t(l, (char*)NULL);
		t.set("X", (float)(offset.X));
		t.set("Y", (float)(offset.Y));
		t.set("Width", (float)(offset.Width));
		t.set("Height", (float)(offset.Height));
		lua::push(l, t);
		t.unref();
		return 1;
	}

	static int CTabLayoutUI_SelectItem(lua_State* l)
	{
		CTabLayoutUI* pTab = nullptr;
		lua::get(l, 1, &pTab);
		if (lua_type(l, 2) == LUA_TNUMBER)
		{
			int iIndex = 0;
			bool bTriggerEvent = true;
			lua::get(l, 2, &iIndex);
			if(lua_type(l,3) == LUA_TBOOLEAN) lua::get(l, 3, &bTriggerEvent);
			return lua::push(l, pTab->SelectItem(iIndex, bTriggerEvent));
		}
		else
		{
			CControlUI* pControl = nullptr;
			bool bTriggerEvent = true;
			lua::get(l, 2, &pControl);
			if (lua_type(l, 3) == LUA_TBOOLEAN) lua::get(l, 3, &bTriggerEvent);
			return lua::push(l, pTab->SelectItem(pControl, bTriggerEvent));
		}
	}

	static int CDateTimeUI_GetTime(lua_State* l)
	{
		CDateTimeUI* pDt = nullptr;
		lua::get(l, 1, &pDt);
		SYSTEMTIME st = pDt->GetTime();
		
		lua::lua_table_ref_t t(l, (char*)NULL);
		t.set("wYear", st.wYear);
		t.set("wMonth", st.wMonth);
		t.set("wDayOfWeek", st.wDayOfWeek);
		t.set("wDay", st.wDay);
		t.set("wHour", st.wHour);
		t.set("wMinute", st.wMinute);
		t.set("wSecond", st.wSecond);
		t.set("wMilliseconds", st.wMilliseconds);
		lua::push(l, t);
		t.unref();
		return 1;
	}
	static int CDateTimeUI_SetTime(lua_State* l)
	{
		CDateTimeUI* pDt = nullptr;
		lua::get(l, 1, &pDt);
		lua::error_report(l, lua_istable(l, 2) != 0, LUA_TTABLE, 2, "lua_istable(l, 2) != 0", __FILE__, __LINE__);
		lua::lua_table_ref_t t;
		lua::get(l, 2, &t);
		SYSTEMTIME st;
		t.get("wYear", &(st.wYear));
		t.get("wMonth", &(st.wMonth));
		t.get("wDayOfWeek", &(st.wDayOfWeek));
		t.get("wDay", &(st.wDay));
		t.get("wHour", &(st.wHour));
		t.get("wMinute", &(st.wMinute));
		t.get("wSecond", &(st.wSecond));
		t.get("wMilliseconds", &(st.wMilliseconds));
		t.unref();
		pDt->SetTime(&st);
		return 0;
	}

	static int CEditUI_SetPrompt(lua_State* l)
	{
		CEditUI* pEdit = nullptr;
		CDuiString strText;
		lua::get(l, 1, &pEdit, &strText);
		pEdit->SetPrompt(strText);
		return 0;
	}

	//long AddAt(CTreeNodeUI* pControl, int iIndex);
	//bool AddAt(CTreeNodeUI* pControl, CTreeNodeUI* _IndexNode);
	static int CTreeViewUI_AddTreeNode(lua_State* l)
	{
		CTreeViewUI* pTreeView = nullptr;
		CTreeNodeUI* pControl = nullptr;
		lua::get(l, 1, &pTreeView, &pControl);
		if (lua_type(l, 3) == LUA_TNUMBER)
		{
			int iIndex; lua::get(l, 3, &iIndex);
			return lua::push(l, pTreeView->AddAt(pControl, iIndex));
		}
		else
		{
			CTreeNodeUI* _IndexNode = nullptr;
			lua::get(l, 3, &_IndexNode);
			return lua::push(l, pTreeView->AddAt(pControl, _IndexNode));
		}
	}
	static int CTreeNodeUI_SetItemText(lua_State* l)
	{
		CTreeNodeUI* pTreeNode = nullptr;
		CDuiString strText;
		lua::get(l, 1, &pTreeNode, &strText);
		pTreeNode->SetItemText(strText);
		return 0;
	}
	static int CTreeNodeUI_GetTreeNodes(lua_State* l)
	{
		CTreeNodeUI* pTreeNode = nullptr;
		lua::get(l, 1, &pTreeNode);
		CDuiPtrArray nodes = pTreeNode->GetTreeNodes();
		lua_newtable(l);
		for (int i = 0; i < nodes.GetSize(); ++i)
		{
			CTreeNodeUI* pNode = static_cast<CTreeNodeUI*>(nodes[i]);
			lua::push(l, i + 1);
			lua::push(l, pNode);
			lua_settable(l, -3);
		}
		return 1;
	}
	
	static int RichEdit_SetFont(lua_State* l)
	{
		CRichEditUI* pRichEdit = nullptr;
		lua::get(l, 1, &pRichEdit);
		if (lua_type(l, 2) == LUA_TNUMBER)
		{
			int iIndex; lua::get(l, 3, &iIndex);
			pRichEdit->SetFont(iIndex);
		}
		else
		{
			CDuiString pStrFontName;
			int nSize;
			bool bBold;
			bool bUnderline;
			bool bItalic;
			lua::get(l, 2, &pStrFontName, &nSize, &bBold, &bUnderline, &bItalic);
			pRichEdit->SetFont(pStrFontName, nSize, bBold, bUnderline, bItalic);
		}
		return 0;
	}

	static int RichEdit_GetSel(lua_State* l)
	{
		CRichEditUI* pRichEdit = nullptr;
		lua::get(l, 1, &pRichEdit);
		long nStartChar, nEndChar;
		pRichEdit->GetSel(nStartChar, nEndChar);

		lua::push(l, nStartChar, nEndChar);
		return 2;
	}
	static int RichEdit_SetSel(lua_State* l)
	{
		CRichEditUI* pRichEdit = nullptr;
		long nStartChar, nEndChar;
		lua::get(l, 1, &pRichEdit, &nStartChar, &nEndChar);
		pRichEdit->SetSel(nStartChar, nEndChar);
		return 0;
	}
	static int RichEdit_GetZoom(lua_State* l)
	{
		CRichEditUI* pRichEdit = nullptr;
		lua::get(l, 1, &pRichEdit);
		int nNum, nDen;
		bool b = pRichEdit->GetZoom(nNum, nDen);
		if (b)
			return lua::push(l, b, nNum, nDen);
		else
			return lua::push(l, false);
	}

	static int ListTextElementUI_SetText(lua_State* l)
	{
		CListTextElementUI* pListText = nullptr;
		lua::get(l, 1, &pListText);
		int iIndex;
		CDuiString szText;
		lua::get(l, 2, &iIndex, &szText);
		pListText->SetText(iIndex, szText);
		return 0;
	}

	static int ListTextElementUI_GetText(lua_State* l)
	{
		CListTextElementUI* pListText = nullptr;
		int iIndex;
		lua::get(l, 1, &pListText, &iIndex);
		return lua::push(l, pListText->GetText(iIndex));
	}

	static void RegisterDuiLibControlToLua(lua_State* l)
	{
		//Control
#define WRAP_METHOD(m) .def(#m, &CControlUI::m)
		lua::lua_register_t<CControlUI>(l, "DuiLib.CControlUI", DeleteControl<CControlUI>)
			.def(lua::constructor<>())
			.def("New", NewControl<CControlUI>)
			.def("__tostring", CControlUI_ToString)
			WRAP_METHOD(ToString)
			WRAP_METHOD(Delete)
			WRAP_METHOD(IsClass)
			WRAP_METHOD(GetClass)
			WRAP_METHOD(IsName)
			WRAP_METHOD(GetName)
			WRAP_METHOD(SetName)
			WRAP_METHOD(Activate)
			WRAP_METHOD(GetControlFlags)
			WRAP_METHOD(GetInterface)
			WRAP_METHOD(GetManager)
			WRAP_METHOD(SetManager)
			WRAP_METHOD(GetParent)
			WRAP_METHOD(GetText)
			WRAP_METHOD(SetText)
			LAMBDA_METHOD2("SetText", CControlUI_SetText)
			WRAP_METHOD(GetBkColor)
			WRAP_METHOD(SetBkColor)
			WRAP_METHOD(GetBkColor2)
			WRAP_METHOD(SetBkColor2)
			WRAP_METHOD(GetBkColor3)
			WRAP_METHOD(SetBkColor3)
			WRAP_METHOD(GetBkImage)
			WRAP_METHOD(SetBkImage)
			WRAP_METHOD(GetFocusBorderColor)
			WRAP_METHOD(SetFocusBorderColor)
			WRAP_METHOD(IsColorHSL)
			WRAP_METHOD(SetColorHSL)
			WRAP_METHOD(GetBorderRound)
			WRAP_METHOD(SetBorderRound)
			WRAP_METHOD(GetBorderSize)
			LAMBDA_METHOD2("SetBorderSize", CControlUI_SetBorderSize)
			WRAP_METHOD(GetBorderColor)
			WRAP_METHOD(SetBorderColor)
			WRAP_METHOD(GetBorderStyle)
			WRAP_METHOD(SetBorderStyle)
			WRAP_METHOD(GetPos)
			WRAP_METHOD(GetRelativePos)
			WRAP_METHOD(SetPos)
			WRAP_METHOD(Move)
			WRAP_METHOD(GetWidth)
			WRAP_METHOD(GetHeight)
			WRAP_METHOD(GetX)
			WRAP_METHOD(GetY)
			WRAP_METHOD(GetPadding)
			WRAP_METHOD(SetPadding)
			WRAP_METHOD(GetFixedXY)
			WRAP_METHOD(SetFixedXY)
			//WRAP_METHOD(GetFloatPercent)
			//WRAP_METHOD(SetFloatPercent)
			WRAP_METHOD(GetFixedWidth)
			WRAP_METHOD(SetFixedWidth)
			WRAP_METHOD(GetFixedHeight)
			WRAP_METHOD(SetFixedHeight)
			WRAP_METHOD(GetMinWidth)
			WRAP_METHOD(SetMinWidth)
			WRAP_METHOD(GetMaxWidth)
			WRAP_METHOD(SetMaxWidth)
			WRAP_METHOD(GetMinHeight)
			WRAP_METHOD(SetMinHeight)
			WRAP_METHOD(GetMaxHeight)
			WRAP_METHOD(SetMaxHeight)
			WRAP_METHOD(GetToolTip)
			LAMBDA_METHOD2("SetToolTip", CControlUI_SetToolTip)
			WRAP_METHOD(SetToolTipWidth)
			WRAP_METHOD(GetToolTipWidth)
			WRAP_METHOD(GetShortcut)
			WRAP_METHOD(SetShortcut)
			WRAP_METHOD(IsContextMenuUsed)
			WRAP_METHOD(SetContextMenuUsed)
			WRAP_METHOD(GetUserData)
			LAMBDA_METHOD2("SetUserData", CControlUI_SetUserData)
			WRAP_METHOD(GetTag)
			WRAP_METHOD(SetTag)
			WRAP_METHOD(GetTagCtrl)
			WRAP_METHOD(SetTagCtrl)
			WRAP_METHOD(IsVisible)
			WRAP_METHOD(SetVisible)
			WRAP_METHOD(SetInternVisible)
			WRAP_METHOD(IsEnabled)
			WRAP_METHOD(SetEnabled)
			WRAP_METHOD(IsMouseEnabled)
			WRAP_METHOD(SetMouseEnabled)
			WRAP_METHOD(IsKeyboardEnabled)
			WRAP_METHOD(SetKeyboardEnabled)
			WRAP_METHOD(IsFocused)
			WRAP_METHOD(SetFocus)
			WRAP_METHOD(IsFloat)
			WRAP_METHOD(SetFloat)
			WRAP_METHOD(AddCustomAttribute)
			WRAP_METHOD(GetCustomAttribute)
			WRAP_METHOD(RemoveCustomAttribute)
			WRAP_METHOD(RemoveAllCustomAttribute)
			WRAP_METHOD(Invalidate)
			WRAP_METHOD(IsUpdateNeeded)
			WRAP_METHOD(NeedUpdate)
			WRAP_METHOD(NeedParentUpdate)
			WRAP_METHOD(GetAdjustColor)
			WRAP_METHOD(GetAttribute)
			WRAP_METHOD(SetAttribute)
			WRAP_METHOD(GetAttributeList)
			WRAP_METHOD(SetAttributeList)
			WRAP_METHOD(EstimateSize)
			WRAP_METHOD(SetVirtualWnd)
			WRAP_METHOD(GetVirtualWnd)
			WRAP_METHOD(GetStyleName)
			WRAP_METHOD(SetStyleName)
			WRAP_METHOD(GetSoundNameOver)
			WRAP_METHOD(SetSoundNameOver)
			WRAP_METHOD(GetSoundNameDown)
			WRAP_METHOD(SetSoundNameDown)
			WRAP_METHOD(SetBindEventCtrl)
			LAMBDA_METHOD2("DoEvent", CControlUI_DoEvent)
			LAMBDA_METHOD2("OnInitAdd", CControlUI_OnInitAdd)
			LAMBDA_METHOD2("OnDestroyAdd", CControlUI_OnDestroyAdd)
			LAMBDA_METHOD2("OnSizeAdd", CControlUI_OnSizeAdd)
			LAMBDA_METHOD2("OnEventAdd", CControlUI_OnEventAdd)
			LAMBDA_METHOD2("OnNotifyAdd", CControlUI_OnNotifyAdd)
			LAMBDA_METHOD2("OnPaintAdd", CControlUI_OnPaintAdd)
			LAMBDA_METHOD2("OnPostPaintAdd", CControlUI_OnPostPaintAdd)
			LAMBDA_METHOD2("BindEventTo", CControlUI_BindEventTo);
#undef WRAP_METHOD
		//CContainerUI
#define WRAP_METHOD(m) .def(#m, &CContainerUI::m)
		lua::lua_register_t<CContainerUI>(l, "DuiLib.CContainerUI", DeleteControl<CContainerUI>)
			.extend<CControlUI>()
			.def(lua::constructor<>())
			.def("New", NewControl<CContainerUI>)
			.def("__tostring", CControlUI_ToString)
			WRAP_METHOD(GetItemAt)
			WRAP_METHOD(GetItemIndex)
			WRAP_METHOD(SetItemIndex)
			WRAP_METHOD(SetMultiItemIndex)
			WRAP_METHOD(GetCount)
			WRAP_METHOD(Add)
			WRAP_METHOD(AddAt)
			WRAP_METHOD(Remove)
			WRAP_METHOD(RemoveAt)
			WRAP_METHOD(RemoveAll)
			WRAP_METHOD(GetInset)
			WRAP_METHOD(SetInset)
			WRAP_METHOD(GetChildPadding)
			WRAP_METHOD(SetChildPadding)
			WRAP_METHOD(GetChildAlign)
			WRAP_METHOD(SetChildAlign)
			WRAP_METHOD(GetChildVAlign)
			WRAP_METHOD(SetChildVAlign)
			WRAP_METHOD(IsAutoDestroy)
			WRAP_METHOD(SetAutoDestroy)
			WRAP_METHOD(IsDelayedDestroy)
			WRAP_METHOD(SetDelayedDestroy)
			WRAP_METHOD(IsMouseChildEnabled)
			WRAP_METHOD(SetMouseChildEnabled)
			WRAP_METHOD(FindSelectable)
			WRAP_METHOD(SetSubControlText)
			WRAP_METHOD(SetSubControlFixedHeight)
			WRAP_METHOD(SetSubControlFixedWdith)
			WRAP_METHOD(SetSubControlUserData)
			WRAP_METHOD(GetSubControlText)
			WRAP_METHOD(GetSubControlFixedHeight)
			WRAP_METHOD(GetSubControlFixedWdith)
			WRAP_METHOD(GetSubControlUserData)
			WRAP_METHOD(FindSubControl)
			WRAP_METHOD(GetScrollPos)
			WRAP_METHOD(GetScrollRange)
			WRAP_METHOD(SetScrollPos)
			WRAP_METHOD(LineUp)
			WRAP_METHOD(LineDown)
			WRAP_METHOD(PageUp)
			WRAP_METHOD(PageDown)
			WRAP_METHOD(HomeUp)
			WRAP_METHOD(EndDown)
			WRAP_METHOD(LineLeft)
			WRAP_METHOD(LineRight)
			WRAP_METHOD(PageLeft)
			WRAP_METHOD(PageRight)
			WRAP_METHOD(HomeLeft)
			WRAP_METHOD(EndRight)
			WRAP_METHOD(EnableScrollBar)
			WRAP_METHOD(GetVerticalScrollBar)
			WRAP_METHOD(GetHorizontalScrollBar);
#undef WRAP_METHOD
		//CHorizontalLayoutUI
#define WRAP_METHOD(m) .def(#m, &CHorizontalLayoutUI::m)
		lua::lua_register_t<CHorizontalLayoutUI>(l, "DuiLib.CHorizontalLayoutUI", DeleteControl<CHorizontalLayoutUI>)
			.extend<CContainerUI>()
			.def(lua::constructor<>())
			.def("New", NewControl<CHorizontalLayoutUI>)
			.def("__tostring", CControlUI_ToString)
			WRAP_METHOD(SetSepWidth)
			WRAP_METHOD(GetSepWidth)
			WRAP_METHOD(SetSepImmMode)
			WRAP_METHOD(IsSepImmMode)
			WRAP_METHOD(GetThumbRect);
#undef WRAP_METHOD
		//CVerticalLayoutUI
#define WRAP_METHOD(m) .def(#m, &CVerticalLayoutUI::m)
		lua::lua_register_t<CVerticalLayoutUI>(l, "DuiLib.CVerticalLayoutUI", DeleteControl<CVerticalLayoutUI>)
			.extend<CContainerUI>()
			.def(lua::constructor<>())
			.def("New", NewControl<CVerticalLayoutUI>)
			.def("__tostring", CControlUI_ToString)
			WRAP_METHOD(SetSepHeight)
			WRAP_METHOD(GetSepHeight)
			WRAP_METHOD(SetSepImmMode)
			WRAP_METHOD(IsSepImmMode)
			WRAP_METHOD(GetThumbRect);
#undef WRAP_METHOD
		//CTabLayoutUI
#define WRAP_METHOD(m) .def(#m, &CTabLayoutUI::m)
		lua::lua_register_t<CTabLayoutUI>(l, "DuiLib.CTabLayoutUI", DeleteControl<CTabLayoutUI>)
			.extend<CContainerUI>()
			.def(lua::constructor<>())
			.def("New", NewControl<CTabLayoutUI>)
			.def("__tostring", CControlUI_ToString)
			WRAP_METHOD(GetCurSel)
			LAMBDA_METHOD2("SelectItem", CTabLayoutUI_SelectItem);
#undef WRAP_METHOD
		//CTileLayoutUI
#define WRAP_METHOD(m) .def(#m, &CTileLayoutUI::m)
		lua::lua_register_t<CTileLayoutUI>(l, "DuiLib.CTileLayoutUI", DeleteControl<CTileLayoutUI>)
			.extend<CContainerUI>()
			.def(lua::constructor<>())
			.def("New", NewControl<CTileLayoutUI>)
			.def("__tostring", CControlUI_ToString)
			WRAP_METHOD(GetFixedColumns)
			WRAP_METHOD(SetFixedColumns)
			WRAP_METHOD(GetChildVPadding)
			WRAP_METHOD(SetChildVPadding)
			WRAP_METHOD(GetItemSize)
			WRAP_METHOD(SetItemSize)
			WRAP_METHOD(GetColumns)
			WRAP_METHOD(GetRows);
#undef WRAP_METHOD
		//CChildLayoutUI
#define WRAP_METHOD(m) .def(#m, &CChildLayoutUI::m)
		lua::lua_register_t<CChildLayoutUI>(l, "DuiLib.CChildLayoutUI", DeleteControl<CChildLayoutUI>)
			.extend<CContainerUI>()
			.def(lua::constructor<>())
			.def("New", NewControl<CChildLayoutUI>)
			.def("__tostring", CControlUI_ToString)
			WRAP_METHOD(Init)
			WRAP_METHOD(SetChildLayoutXML)
			WRAP_METHOD(GetChildLayoutXML);
#undef WRAP_METHOD
		//CActiveXUI
#define WRAP_METHOD(m) .def(#m, &CActiveXUI::m)
		lua::lua_register_t<CActiveXUI>(l, "DuiLib.CActiveXUI", DeleteControl<CActiveXUI>)
			.extend<CControlUI>()
			.def(lua::constructor<>())
			.def("New", NewControl<CActiveXUI>)
			.def("__tostring", CControlUI_ToString)
			WRAP_METHOD(Init)
			WRAP_METHOD(GetNativeWindow)
			WRAP_METHOD(IsDelayCreate)
			WRAP_METHOD(SetDelayCreate)
			WRAP_METHOD(GetModuleName)
			WRAP_METHOD(SetModuleName);
#undef WRAP_METHOD
		//CComboUI
#define WRAP_METHOD(m) .def(#m, &CComboUI::m)
		lua::lua_register_t<CComboUI>(l, "DuiLib.CComboUI", DeleteControl<CComboUI>)
			.extend<CContainerUI>()
			.def(lua::constructor<>())
			.def("New", NewControl<CComboUI>)
			.def("__tostring", CControlUI_ToString)
			WRAP_METHOD(GetDropBoxAttributeList)
			WRAP_METHOD(SetDropBoxAttributeList)
			WRAP_METHOD(GetDropBoxSize)
			WRAP_METHOD(SetDropBoxSize)
			WRAP_METHOD(GetCurSel)
			WRAP_METHOD(SelectItem)
			WRAP_METHOD(GetSelectCloseFlag)
			WRAP_METHOD(SetSelectCloseFlag)
			WRAP_METHOD(ExpandItem)
			WRAP_METHOD(GetExpandedItem)
			WRAP_METHOD(SetItemIndex)
			WRAP_METHOD(SetMultiItemIndex)
			WRAP_METHOD(GetShowText)
			WRAP_METHOD(SetShowText)
			WRAP_METHOD(GetTextPadding)
			WRAP_METHOD(SetTextPadding)
			WRAP_METHOD(GetNormalImage)
			WRAP_METHOD(SetNormalImage)
			WRAP_METHOD(GetHotImage)
			WRAP_METHOD(SetHotImage)
			WRAP_METHOD(GetPushedImage)
			WRAP_METHOD(SetPushedImage)
			WRAP_METHOD(GetFocusedImage)
			WRAP_METHOD(SetFocusedImage)
			WRAP_METHOD(GetDisabledImage)
			WRAP_METHOD(SetDisabledImage)
			WRAP_METHOD(GetItemFixedHeight)
			WRAP_METHOD(SetItemFixedHeight)
			WRAP_METHOD(GetItemFont)
			WRAP_METHOD(SetItemFont)
			WRAP_METHOD(GetItemTextStyle)
			WRAP_METHOD(SetItemTextStyle)
			WRAP_METHOD(GetItemTextPadding)
			WRAP_METHOD(SetItemTextPadding)
			WRAP_METHOD(GetItemTextColor)
			WRAP_METHOD(SetItemTextColor)
			WRAP_METHOD(GetItemBkColor)
			WRAP_METHOD(SetItemBkColor)
			WRAP_METHOD(GetItemBkImage)
			WRAP_METHOD(SetItemBkImage)
			WRAP_METHOD(IsAlternateBk)
			WRAP_METHOD(SetAlternateBk)
			WRAP_METHOD(GetSelectedItemTextColor)
			WRAP_METHOD(SetSelectedItemTextColor)
			WRAP_METHOD(GetSelectedItemBkColor)
			WRAP_METHOD(SetSelectedItemBkColor)
			WRAP_METHOD(GetSelectedItemImage)
			WRAP_METHOD(SetSelectedItemImage)
			WRAP_METHOD(GetHotItemTextColor)
			WRAP_METHOD(SetHotItemTextColor)
			WRAP_METHOD(GetHotItemBkColor)
			WRAP_METHOD(SetHotItemBkColor)
			WRAP_METHOD(GetHotItemImage)
			WRAP_METHOD(SetHotItemImage)
			WRAP_METHOD(GetDisabledItemTextColor)
			WRAP_METHOD(SetDisabledItemTextColor)
			WRAP_METHOD(GetDisabledItemBkColor)
			WRAP_METHOD(SetDisabledItemBkColor)
			WRAP_METHOD(GetDisabledItemImage)
			WRAP_METHOD(SetDisabledItemImage)
			WRAP_METHOD(GetItemHLineSize)
			WRAP_METHOD(SetItemHLineSize)
			WRAP_METHOD(GetItemHLineColor)
			WRAP_METHOD(SetItemHLineColor)
			WRAP_METHOD(GetItemVLineSize)
			WRAP_METHOD(SetItemVLineSize)
			WRAP_METHOD(GetItemVLineColor)
			WRAP_METHOD(SetItemVLineColor)
			WRAP_METHOD(IsItemShowHtml)
			WRAP_METHOD(SetItemShowHtml);

#undef WRAP_METHOD
		//CLabelUI
#define WRAP_METHOD(m) .def(#m, &CLabelUI::m)
		lua::lua_register_t<CLabelUI>(l, "DuiLib.CLabelUI", DeleteControl<CLabelUI>)
			.extend<CControlUI>()
			.def(lua::constructor<>())
			.def("New", NewControl<CLabelUI>)
			.def("__tostring", CControlUI_ToString)
			WRAP_METHOD(SetTextStyle)
			WRAP_METHOD(GetTextStyle)
			WRAP_METHOD(IsMultiLine)
			WRAP_METHOD(SetMultiLine)
			WRAP_METHOD(SetTextColor)
			WRAP_METHOD(GetTextColor)
			WRAP_METHOD(SetDisabledTextColor)
			WRAP_METHOD(GetDisabledTextColor)
			WRAP_METHOD(SetFont)
			WRAP_METHOD(GetFont)
			WRAP_METHOD(GetTextPadding)
			WRAP_METHOD(SetTextPadding)
			WRAP_METHOD(IsShowHtml)
			WRAP_METHOD(SetShowHtml)

			WRAP_METHOD(SetEnabledEffect)
			WRAP_METHOD(GetEnabledEffect)
			WRAP_METHOD(SetEnabledLuminous)
			WRAP_METHOD(GetEnabledLuminous)
			WRAP_METHOD(SetLuminousFuzzy)
			WRAP_METHOD(GetLuminousFuzzy)
			WRAP_METHOD(SetGradientLength)
			WRAP_METHOD(GetGradientLength)
			WRAP_METHOD(SetShadowOffset)
			LAMBDA_METHOD2("GetShadowOffset", CLabelUI_GetShadowOffset)
			WRAP_METHOD(SetTextColor1)
			WRAP_METHOD(GetTextColor1)
			WRAP_METHOD(SetTextShadowColorA)
			WRAP_METHOD(GetTextShadowColorA)
			WRAP_METHOD(SetTextShadowColorB)
			WRAP_METHOD(GetTextShadowColorB)
			WRAP_METHOD(SetStrokeColor)
			WRAP_METHOD(GetStrokeColor)
			WRAP_METHOD(SetGradientAngle)
			WRAP_METHOD(GetGradientAngle)
			WRAP_METHOD(SetEnabledStroke)
			WRAP_METHOD(GetEnabledStroke)
			WRAP_METHOD(SetEnabledShadow)
			WRAP_METHOD(GetEnabledShadow);

#undef WRAP_METHOD
		//CDateTimeUI
#define WRAP_METHOD(m) .def(#m, &CDateTimeUI::m)
		lua::lua_register_t<CDateTimeUI>(l, "DuiLib.CDateTimeUI", DeleteControl<CDateTimeUI>)
			.extend<CLabelUI>()
			.def(lua::constructor<>())
			.def("New", NewControl<CDateTimeUI>)
			.def("__tostring", CControlUI_ToString)
			WRAP_METHOD(SetReadOnly)
			WRAP_METHOD(IsReadOnly)
			WRAP_METHOD(UpdateText)
			LAMBDA_METHOD2("GetTime", CDateTimeUI_GetTime)
			LAMBDA_METHOD2("SetTime", CDateTimeUI_SetTime)
			WRAP_METHOD(GetNativeWindow);
#undef WRAP_METHOD
		//CTextUI
#define WRAP_METHOD(m) .def(#m, &CTextUI::m)
		lua::lua_register_t<CTextUI>(l, "DuiLib.CTextUI", DeleteControl<CTextUI>)
			.extend<CLabelUI>()
			.def(lua::constructor<>())
			.def("New", NewControl<CTextUI>)
			.def("__tostring", CControlUI_ToString)
			WRAP_METHOD(GetLinkContent);
#undef WRAP_METHOD
		//CButtonUI
#define WRAP_METHOD(m) .def(#m, &CButtonUI::m)
		lua::lua_register_t<CButtonUI>(l, "DuiLib.CButtonUI", DeleteControl<CButtonUI>)
			.extend<CLabelUI>()
			.def(lua::constructor<>())
			.def("New", NewControl<CButtonUI>)
			.def("__tostring", CControlUI_ToString)
			WRAP_METHOD(GetNormalImage)
			WRAP_METHOD(SetNormalImage)
			WRAP_METHOD(GetHotImage)
			WRAP_METHOD(SetHotImage)
			WRAP_METHOD(GetPushedImage)
			WRAP_METHOD(SetPushedImage)
			WRAP_METHOD(GetFocusedImage)
			WRAP_METHOD(SetFocusedImage)
			WRAP_METHOD(GetDisabledImage)
			WRAP_METHOD(SetDisabledImage)
			WRAP_METHOD(GetForeImage)
			WRAP_METHOD(SetForeImage)
			WRAP_METHOD(GetHotForeImage)
			WRAP_METHOD(SetHotForeImage)
			WRAP_METHOD(SetFiveStatusImage)
			WRAP_METHOD(SetFadeAlphaDelta)
			WRAP_METHOD(SetHotBkColor)
			WRAP_METHOD(GetHotBkColor)
			WRAP_METHOD(SetHotTextColor)
			WRAP_METHOD(GetHotTextColor)
			WRAP_METHOD(SetPushedTextColor)
			WRAP_METHOD(GetPushedTextColor)
			WRAP_METHOD(SetFocusedTextColor)
			WRAP_METHOD(GetFocusedTextColor);
#undef WRAP_METHOD
		//COptionUI
#define WRAP_METHOD(m) .def(#m, &COptionUI::m)
		lua::lua_register_t<COptionUI>(l, "DuiLib.COptionUI", DeleteControl<COptionUI>)
			.extend<CButtonUI>()
			.def(lua::constructor<>())
			.def("New", NewControl<COptionUI>)
			.def("__tostring", CControlUI_ToString)
			WRAP_METHOD(GetSelectedImage)
			WRAP_METHOD(SetSelectedImage)
			WRAP_METHOD(GetSelectedHotImage)
			WRAP_METHOD(GetSelectedHotImage)
			WRAP_METHOD(SetSelectedTextColor)
			WRAP_METHOD(GetSelectedTextColor)
			WRAP_METHOD(SetSelectedBkColor)
			WRAP_METHOD(GetSelectedBkColor)
			WRAP_METHOD(GetGroup)
			WRAP_METHOD(SetGroup)
			WRAP_METHOD(IsSelected)
			WRAP_METHOD(Selected)
			WRAP_METHOD(SetSelectedToolTip)
			WRAP_METHOD(GetSelectedToolTip)
			WRAP_METHOD(SetNormalTooltip)
			WRAP_METHOD(GetNormalTooltip);
#undef WRAP_METHOD
		//CCheckBoxUI
#define WRAP_METHOD(m) .def(#m, &CCheckBoxUI::m)
		lua::lua_register_t<CCheckBoxUI>(l, "DuiLib.CCheckBoxUI", DeleteControl<CCheckBoxUI>)
			.extend<COptionUI>()
			.def(lua::constructor<>())
			.def("New", NewControl<CCheckBoxUI>)
			.def("__tostring", CControlUI_ToString)
			WRAP_METHOD(SetCheck)
			WRAP_METHOD(GetCheck);
#undef WRAP_METHOD
		//CEditUI
#define WRAP_METHOD(m) .def(#m, &CEditUI::m)
		lua::lua_register_t<CEditUI>(l, "DuiLib.CEditUI", DeleteControl<CEditUI>)
			.extend<CLabelUI>()
			.def(lua::constructor<>())
			.def("New", NewControl<CEditUI>)
			.def("__tostring", CControlUI_ToString)
			WRAP_METHOD(GetNativeWindow)
			WRAP_METHOD(SetMaxChar)
			WRAP_METHOD(GetMaxChar)
			WRAP_METHOD(SetReadOnly)
			WRAP_METHOD(IsReadOnly)
			WRAP_METHOD(SetPasswordMode)
			WRAP_METHOD(IsPasswordMode)
			WRAP_METHOD(SetPasswordChar)
			WRAP_METHOD(GetPasswordChar)
			WRAP_METHOD(IsAutoSelAll)
			WRAP_METHOD(SetAutoSelAll)
			WRAP_METHOD(SetNumberOnly)
			WRAP_METHOD(IsNumberOnly)
			WRAP_METHOD(GetWindowStyls)
			WRAP_METHOD(GetNativeEditHWND)
			WRAP_METHOD(GetNormalImage)
			WRAP_METHOD(SetNormalImage)
			WRAP_METHOD(GetHotImage)
			WRAP_METHOD(SetHotImage)
			WRAP_METHOD(GetFocusedImage)
			WRAP_METHOD(SetFocusedImage)
			WRAP_METHOD(GetDisabledImage)
			WRAP_METHOD(SetDisabledImage)
			WRAP_METHOD(SetNativeEditBkColor)
			WRAP_METHOD(GetNativeEditBkColor)
			WRAP_METHOD(SetSel)
			WRAP_METHOD(SetSelAll)
			LAMBDA_METHOD2("SetPrompt", CEditUI_SetPrompt)
			WRAP_METHOD(GetPrompt);
#undef WRAP_METHOD
		//CGifAnimUI
#define WRAP_METHOD(m) .def(#m, &CGifAnimUI::m)
		lua::lua_register_t<CGifAnimUI>(l, "DuiLib.CGifAnimUI", DeleteControl<CGifAnimUI>)
			.extend<CControlUI>()
			.def(lua::constructor<>())
			.def("New", NewControl<CGifAnimUI>)
			.def("__tostring", CControlUI_ToString)
			WRAP_METHOD(SetAutoPlay)
			WRAP_METHOD(IsAutoPlay)
			WRAP_METHOD(SetAutoSize)
			WRAP_METHOD(IsAutoSize)
			WRAP_METHOD(PlayGif)
			WRAP_METHOD(PauseGif)
			WRAP_METHOD(StopGif);
#undef WRAP_METHOD
		//CProgressUI
#define WRAP_METHOD(m) .def(#m, &CProgressUI::m)
		lua::lua_register_t<CProgressUI>(l, "DuiLib.CProgressUI", DeleteControl<CProgressUI>)
			.extend<CLabelUI>()
			.def(lua::constructor<>())
			.def("New", NewControl<CProgressUI>)
			.def("__tostring", CControlUI_ToString)
			WRAP_METHOD(IsHorizontal)
			WRAP_METHOD(SetHorizontal)
			WRAP_METHOD(GetMinValue)
			WRAP_METHOD(SetMinValue)
			WRAP_METHOD(GetMaxValue)
			WRAP_METHOD(SetMaxValue)
			WRAP_METHOD(GetValue)
			WRAP_METHOD(SetValue)
			WRAP_METHOD(GetForeImage)
			WRAP_METHOD(SetForeImage);
#undef WRAP_METHOD
		//CSliderUI
#define WRAP_METHOD(m) .def(#m, &CSliderUI::m)
		lua::lua_register_t<CSliderUI>(l, "DuiLib.CSliderUI", DeleteControl<CSliderUI>)
			.extend<CProgressUI>()
			.def(lua::constructor<>())
			.def("New", NewControl<CSliderUI>)
			.def("__tostring", CControlUI_ToString)
			WRAP_METHOD(GetChangeStep)
			WRAP_METHOD(SetChangeStep)
			WRAP_METHOD(SetThumbSize)
			WRAP_METHOD(GetThumbRect)
			WRAP_METHOD(IsImmMode)
			WRAP_METHOD(SetImmMode)
			WRAP_METHOD(GetThumbImage)
			WRAP_METHOD(SetThumbImage)
			WRAP_METHOD(GetThumbHotImage)
			WRAP_METHOD(SetThumbHotImage)
			WRAP_METHOD(GetThumbPushedImage)
			WRAP_METHOD(SetThumbPushedImage);
#undef WRAP_METHOD
		//CScrollBarUI
#define WRAP_METHOD(m) .def(#m, &CScrollBarUI::m)
		lua::lua_register_t<CScrollBarUI>(l, "DuiLib.CScrollBarUI", DeleteControl<CScrollBarUI>)
			.extend<CControlUI>()
			.def(lua::constructor<>())
			.def("New", NewControl<CScrollBarUI>)
			.def("__tostring", CControlUI_ToString)
			WRAP_METHOD(GetOwner)
			WRAP_METHOD(SetOwner)
			WRAP_METHOD(IsHorizontal)
			WRAP_METHOD(SetHorizontal)
			WRAP_METHOD(GetScrollRange)
			WRAP_METHOD(SetScrollRange)
			WRAP_METHOD(GetScrollPos)
			WRAP_METHOD(SetScrollPos)
			WRAP_METHOD(GetLineSize)
			WRAP_METHOD(SetLineSize)
			WRAP_METHOD(GetScrollUnit)
			WRAP_METHOD(SetScrollUnit)
			WRAP_METHOD(GetShowButton1)
			WRAP_METHOD(SetShowButton1)
			WRAP_METHOD(GetButton1Color)
			WRAP_METHOD(SetButton1Color)
			WRAP_METHOD(GetButton1NormalImage)
			WRAP_METHOD(SetButton1NormalImage)
			WRAP_METHOD(GetButton1HotImage)
			WRAP_METHOD(SetButton1HotImage)
			WRAP_METHOD(GetButton1PushedImage)
			WRAP_METHOD(SetButton1PushedImage)
			WRAP_METHOD(GetButton1DisabledImage)
			WRAP_METHOD(SetButton1DisabledImage)
			WRAP_METHOD(GetShowButton2)
			WRAP_METHOD(SetShowButton2)
			WRAP_METHOD(GetButton2Color)
			WRAP_METHOD(SetButton2Color)
			WRAP_METHOD(GetButton2NormalImage)
			WRAP_METHOD(SetButton2NormalImage)
			WRAP_METHOD(GetButton2HotImage)
			WRAP_METHOD(SetButton2HotImage)
			WRAP_METHOD(GetButton2PushedImage)
			WRAP_METHOD(SetButton2PushedImage)
			WRAP_METHOD(GetButton2DisabledImage)
			WRAP_METHOD(SetButton2DisabledImage)
			WRAP_METHOD(GetThumbColor)
			WRAP_METHOD(SetThumbColor)
			WRAP_METHOD(GetThumbNormalImage)
			WRAP_METHOD(SetThumbNormalImage)
			WRAP_METHOD(GetThumbHotImage)
			WRAP_METHOD(SetThumbHotImage)
			WRAP_METHOD(GetThumbPushedImage)
			WRAP_METHOD(SetThumbPushedImage)
			WRAP_METHOD(GetThumbDisabledImage)
			WRAP_METHOD(SetThumbDisabledImage)
			WRAP_METHOD(GetRailNormalImage)
			WRAP_METHOD(SetRailNormalImage)
			WRAP_METHOD(GetRailHotImage)
			WRAP_METHOD(SetRailHotImage)
			WRAP_METHOD(GetRailPushedImage)
			WRAP_METHOD(SetRailPushedImage)
			WRAP_METHOD(GetRailDisabledImage)
			WRAP_METHOD(SetRailDisabledImage)
			WRAP_METHOD(GetBkNormalImage)
			WRAP_METHOD(SetBkNormalImage)
			WRAP_METHOD(GetBkHotImage)
			WRAP_METHOD(SetBkHotImage)
			WRAP_METHOD(GetBkPushedImage)
			WRAP_METHOD(SetBkPushedImage)
			WRAP_METHOD(GetBkDisabledImage)
			WRAP_METHOD(SetBkDisabledImage);
#undef WRAP_METHOD
		//CListUI
#define WRAP_METHOD(m) .def(#m, &CListUI::m)
		lua::lua_register_t<CListUI>(l, "DuiLib.CListUI", DeleteControl<CListUI>)
			.extend<CVerticalLayoutUI>()
			.def(lua::constructor<>())
			.def("New", NewControl<CListUI>)
			.def("__tostring", CControlUI_ToString)
			WRAP_METHOD(GetScrollSelect)
			WRAP_METHOD(SetScrollSelect)
			WRAP_METHOD(SetTextCallback)
			WRAP_METHOD(GetCurSel)
			WRAP_METHOD(SelectItem)
			WRAP_METHOD(GetSelCount)
			WRAP_METHOD(SetMultiSelect)
			WRAP_METHOD(IsMultiSelected)
			WRAP_METHOD(EnsureVisible)
			WRAP_METHOD(GetItemFixedHeight)
			WRAP_METHOD(SetItemFixedHeight)
			WRAP_METHOD(GetItemFont)
			WRAP_METHOD(SetItemFont)
			WRAP_METHOD(GetItemTextStyle)
			WRAP_METHOD(SetItemTextStyle)
			WRAP_METHOD(GetItemTextPadding)
			WRAP_METHOD(SetItemTextPadding)
			WRAP_METHOD(GetItemTextColor)
			WRAP_METHOD(SetItemTextColor)
			WRAP_METHOD(GetItemBkColor)
			WRAP_METHOD(SetItemBkColor)
			WRAP_METHOD(GetItemBkImage)
			WRAP_METHOD(SetItemBkImage)
			WRAP_METHOD(IsAlternateBk)
			WRAP_METHOD(SetAlternateBk)
			WRAP_METHOD(GetSelectedItemTextColor)
			WRAP_METHOD(SetSelectedItemTextColor)
			WRAP_METHOD(GetSelectedItemBkColor)
			WRAP_METHOD(SetSelectedItemBkColor)
			WRAP_METHOD(GetSelectedItemImage)
			WRAP_METHOD(SetSelectedItemImage)
			WRAP_METHOD(GetHotItemTextColor)
			WRAP_METHOD(SetHotItemTextColor)
			WRAP_METHOD(GetHotItemBkColor)
			WRAP_METHOD(SetHotItemBkColor)
			WRAP_METHOD(GetHotItemImage)
			WRAP_METHOD(SetHotItemImage)
			WRAP_METHOD(GetDisabledItemTextColor)
			WRAP_METHOD(SetDisabledItemTextColor)
			WRAP_METHOD(GetDisabledItemBkColor)
			WRAP_METHOD(SetDisabledItemBkColor)
			WRAP_METHOD(GetDisabledItemImage)
			WRAP_METHOD(SetDisabledItemImage)
			WRAP_METHOD(GetItemHLineSize)
			WRAP_METHOD(SetItemHLineSize)
			WRAP_METHOD(GetItemHLineColor)
			WRAP_METHOD(SetItemHLineColor)
			WRAP_METHOD(GetItemVLineSize)
			WRAP_METHOD(SetItemVLineSize)
			WRAP_METHOD(GetItemVLineColor)
			WRAP_METHOD(SetItemVLineColor)
			WRAP_METHOD(IsItemShowHtml)
			WRAP_METHOD(SetItemShowHtml)
			WRAP_METHOD(SetMultiExpanding)
			WRAP_METHOD(GetExpandedItem)
			WRAP_METHOD(ExpandItem)
			WRAP_METHOD(EnableScrollBar);
#undef WRAP_METHOD
		//CListBodyUI
#define WRAP_METHOD(m) .def(#m, &CListBodyUI::m)
		lua::lua_register_t<CListBodyUI>(l, "DuiLib.CListBodyUI", DeleteControl<CListBodyUI>)
			.extend<CVerticalLayoutUI>()
			.def("__tostring", CControlUI_ToString);
#undef WRAP_METHOD
		//CListHeaderUI
#define WRAP_METHOD(m) .def(#m, &CListHeaderUI::m)
		lua::lua_register_t<CListHeaderUI>(l, "DuiLib.CListHeaderUI", DeleteControl<CListHeaderUI>)
			.extend<CHorizontalLayoutUI>()
			.def(lua::constructor<>())
			.def("New", NewControl<CListHeaderUI>)
			.def("__tostring", CControlUI_ToString);
#undef WRAP_METHOD
		//CListHeaderItemUI
#define WRAP_METHOD(m) .def(#m, &CListHeaderItemUI::m)
		lua::lua_register_t<CListHeaderItemUI>(l, "DuiLib.CListHeaderItemUI", DeleteControl<CListHeaderItemUI>)
			.extend<CControlUI>()
			.def(lua::constructor<>())
			.def("New", NewControl<CListHeaderItemUI>)
			.def("__tostring", CControlUI_ToString)
			WRAP_METHOD(IsDragable)
			WRAP_METHOD(SetDragable)
			WRAP_METHOD(GetSepWidth)
			WRAP_METHOD(SetSepWidth)
			WRAP_METHOD(SetTextStyle)
			WRAP_METHOD(GetTextStyle)
			WRAP_METHOD(GetTextColor)
			WRAP_METHOD(SetTextColor)
			WRAP_METHOD(GetSepColor)
			WRAP_METHOD(SetSepColor)
			WRAP_METHOD(SetTextPadding)
			WRAP_METHOD(GetTextPadding)
			WRAP_METHOD(SetFont)
			WRAP_METHOD(IsShowHtml)
			WRAP_METHOD(SetShowHtml)
			WRAP_METHOD(GetNormalImage)
			WRAP_METHOD(SetNormalImage)
			WRAP_METHOD(GetHotImage)
			WRAP_METHOD(SetHotImage)
			WRAP_METHOD(GetPushedImage)
			WRAP_METHOD(SetPushedImage)
			WRAP_METHOD(GetFocusedImage)
			WRAP_METHOD(SetFocusedImage)
			WRAP_METHOD(GetSepImage)
			WRAP_METHOD(SetSepImage)
			WRAP_METHOD(GetThumbRect);
#undef WRAP_METHOD
		//CListElementUI
#define WRAP_METHOD(m) .def(#m, &CListElementUI::m)
		lua::lua_register_t<CListElementUI>(l, "DuiLib.CListElementUI", DeleteControl<CListElementUI>)
			.extend<CControlUI>()
			.def(lua::constructor<>())
			.def("New", NewControl<CListElementUI>)
			.def("__tostring", CControlUI_ToString)
			WRAP_METHOD(GetIndex)
			WRAP_METHOD(SetIndex)
			WRAP_METHOD(GetDrawIndex)
			WRAP_METHOD(SetDrawIndex)
			WRAP_METHOD(GetOwner)
			WRAP_METHOD(SetOwner)
			WRAP_METHOD(IsSelected)
			WRAP_METHOD(Select)
			WRAP_METHOD(IsExpanded)
			WRAP_METHOD(Expand);
#undef WRAP_METHOD
		//CListLabelElementUI
#define WRAP_METHOD(m) .def(#m, &CListLabelElementUI::m)
		lua::lua_register_t<CListLabelElementUI>(l, "DuiLib.CListLabelElementUI", DeleteControl<CListLabelElementUI>)
			.extend<CListElementUI>()
			.def(lua::constructor<>())
			.def("New", NewControl<CListLabelElementUI>)
			.def("__tostring", CControlUI_ToString);
#undef WRAP_METHOD
		//CListTextElementUI
#define WRAP_METHOD(m) .def(#m, &CListTextElementUI::m)
		lua::lua_register_t<CListTextElementUI>(l, "DuiLib.CListTextElementUI", DeleteControl<CListTextElementUI>)
			.extend<CListLabelElementUI>()
			.def(lua::constructor<>())
			.def("New", NewControl<CListTextElementUI>)
			.def("__tostring", CControlUI_ToString)
			LAMBDA_METHOD2("SetText2", ListTextElementUI_SetText)
			LAMBDA_METHOD2("GetText2", ListTextElementUI_GetText);
#undef WRAP_METHOD
		//CListContainerElementUI
#define WRAP_METHOD(m) .def(#m, &CListContainerElementUI::m)
		lua::lua_register_t<CListContainerElementUI>(l, "DuiLib.CListContainerElementUI", DeleteControl<CListContainerElementUI>)
			.extend<CContainerUI>()
			.def(lua::constructor<>())
			.def("New", NewControl<CListContainerElementUI>)
			.def("__tostring", CControlUI_ToString)
			WRAP_METHOD(GetOwner)
			WRAP_METHOD(SetOwner)
			WRAP_METHOD(IsSelected)
			WRAP_METHOD(Select)
			WRAP_METHOD(IsExpanded)
			WRAP_METHOD(Expand)
			WRAP_METHOD(IsExpandable)
			WRAP_METHOD(SetExpandable);
#undef WRAP_METHOD
		//CTreeViewUI
#define WRAP_METHOD(m) .def(#m, &CTreeViewUI::m)
		lua::lua_register_t<CTreeViewUI>(l, "DuiLib.CTreeViewUI", DeleteControl<CTreeViewUI>)
			.extend<CListUI>()
			.def(lua::constructor<>())
			.def("New", NewControl<CTreeViewUI>)
			.def("__tostring", CControlUI_ToString)
			LAMBDA_METHOD2("AddTreeNode", CTreeViewUI_AddTreeNode)
			WRAP_METHOD(SetItemCheckBox)
			WRAP_METHOD(SetItemExpand)
			WRAP_METHOD(SetVisibleFolderBtn)
			WRAP_METHOD(GetVisibleFolderBtn)
			WRAP_METHOD(SetVisibleCheckBtn)
			WRAP_METHOD(GetVisibleCheckBtn)
			WRAP_METHOD(SetItemMinWidth)
			WRAP_METHOD(GetItemMinWidth)
			WRAP_METHOD(SetItemTextColor)
			WRAP_METHOD(SetItemHotTextColor)
			WRAP_METHOD(SetSelItemTextColor)
			WRAP_METHOD(SetSelItemHotTextColor);
#undef WRAP_METHOD
		//CTreeNodeUI
#define WRAP_METHOD(m) .def(#m, &CTreeNodeUI::m)
		lua::lua_register_t<CTreeNodeUI>(l, "DuiLib.CTreeNodeUI", DeleteControl<CTreeNodeUI>)
			.extend<CListContainerElementUI>()
			.def(lua::constructor<>())
			.def("New", NewControl<CTreeNodeUI>)
			.def("__tostring", CControlUI_ToString)
			WRAP_METHOD(SetVisibleTag)
			WRAP_METHOD(GetVisibleTag)
			LAMBDA_METHOD2("SetItemText", CTreeNodeUI_SetItemText)
			WRAP_METHOD(GetItemText)
			WRAP_METHOD(CheckBoxSelected)
			WRAP_METHOD(IsCheckBoxSelected)
			WRAP_METHOD(IsHasChild)
			//WRAP_METHOD(GetTreeLevel)
			WRAP_METHOD(AddChildNode)
			WRAP_METHOD(SetParentNode)
			WRAP_METHOD(GetParentNode)
			WRAP_METHOD(GetCountChild)
			WRAP_METHOD(SetTreeView)
			WRAP_METHOD(GetTreeView)
			WRAP_METHOD(GetChildNode)
			WRAP_METHOD(SetVisibleFolderBtn)
			WRAP_METHOD(GetVisibleFolderBtn)			
			WRAP_METHOD(SetVisibleCheckBtn)
			WRAP_METHOD(GetVisibleCheckBtn)
			WRAP_METHOD(SetItemTextColor)
			WRAP_METHOD(GetItemTextColor)
			WRAP_METHOD(SetItemHotTextColor)
			WRAP_METHOD(GetItemHotTextColor)		
			WRAP_METHOD(SetSelItemTextColor)
			WRAP_METHOD(GetSelItemTextColor)
			WRAP_METHOD(SetSelItemHotTextColor)
			WRAP_METHOD(GetSelItemHotTextColor)
			LAMBDA_METHOD2("GetTreeNodes", CTreeNodeUI_GetTreeNodes)
			WRAP_METHOD(GetTreeIndex)
			WRAP_METHOD(GetNodeIndex)
			WRAP_METHOD(GetTreeNodeHoriznotal)
			WRAP_METHOD(GetFolderButton)
			WRAP_METHOD(GetDottedLine)
			WRAP_METHOD(GetCheckBox)
			WRAP_METHOD(GetItemButton);
#undef WRAP_METHOD
		//CWebBrowserUI
#define WRAP_METHOD(m) .def(#m, &CWebBrowserUI::m)
		lua::lua_register_t<CWebBrowserUI>(l, "DuiLib.CWebBrowserUI", DeleteControl<CWebBrowserUI>)
			.extend<CActiveXUI>()
			.def(lua::constructor<>())
			.def("New", NewControl<CWebBrowserUI>)
			.def("__tostring", CControlUI_ToString)
			WRAP_METHOD(SetHomePage)
			WRAP_METHOD(GetHomePage)
			WRAP_METHOD(SetAutoNavigation)
			WRAP_METHOD(IsAutoNavigation)
			WRAP_METHOD(Navigate2)
			WRAP_METHOD(Refresh)
			WRAP_METHOD(Refresh2)
			WRAP_METHOD(GoBack)
			WRAP_METHOD(GoForward)
			WRAP_METHOD(NavigateHomePage)
			WRAP_METHOD(NavigateUrl);
#undef WRAP_METHOD
		//CRichEditUI
#define WRAP_METHOD(m) .def(#m, &CRichEditUI::m)
		lua::lua_register_t<CRichEditUI>(l, "DuiLib.CRichEditUI", DeleteControl<CRichEditUI>)
			.extend<CContainerUI>()
			.def(lua::constructor<>())
			.def("New", NewControl<CRichEditUI>)
			.def("__tostring", CControlUI_ToString)
			WRAP_METHOD(IsWantTab)
			WRAP_METHOD(SetWantTab)
			WRAP_METHOD(IsWantReturn)
			WRAP_METHOD(SetWantReturn)
			WRAP_METHOD(IsWantCtrlReturn)
			WRAP_METHOD(SetWantCtrlReturn)
			WRAP_METHOD(IsTransparent)
			WRAP_METHOD(SetTransparent)
			WRAP_METHOD(IsRich)
			WRAP_METHOD(SetRich)
			WRAP_METHOD(IsReadOnly)
			WRAP_METHOD(SetReadOnly)
			WRAP_METHOD(IsWordWrap)
			WRAP_METHOD(SetWordWrap)
			LAMBDA_METHOD2("SetFont", RichEdit_SetFont)
			WRAP_METHOD(GetWinStyle)
			WRAP_METHOD(SetWinStyle)
			WRAP_METHOD(GetTextColor)
			WRAP_METHOD(SetTextColor)
			WRAP_METHOD(GetLimitText)
			WRAP_METHOD(SetLimitText)
			WRAP_METHOD(GetTextLength)
			WRAP_METHOD(IsModify)
			WRAP_METHOD(SetModify)
			LAMBDA_METHOD2("GetSel", RichEdit_GetSel)
			LAMBDA_METHOD2("SetSel", RichEdit_SetSel)
			WRAP_METHOD(ReplaceSel)
			//WRAP_METHOD(ReplaceSelW)
			WRAP_METHOD(GetSelText)
			WRAP_METHOD(SetSelAll)
			WRAP_METHOD(SetSelNone)
			WRAP_METHOD(GetSelectionType)
			LAMBDA_METHOD2("GetZoom", RichEdit_GetZoom)
			WRAP_METHOD(SetZoom)
			WRAP_METHOD(SetZoomOff)
			WRAP_METHOD(GetAutoURLDetect)
			WRAP_METHOD(SetAutoURLDetect)
			WRAP_METHOD(GetEventMask)
			WRAP_METHOD(SetEventMask)
			WRAP_METHOD(GetTextRange)
			WRAP_METHOD(HideSelection)
			WRAP_METHOD(ScrollCaret)
			WRAP_METHOD(InsertText)
			WRAP_METHOD(AppendText)
			WRAP_METHOD(Redo)
			WRAP_METHOD(Undo)
			WRAP_METHOD(Clear)
			WRAP_METHOD(Copy)
			WRAP_METHOD(Cut)
			WRAP_METHOD(Paste)
			WRAP_METHOD(GetLineCount)
			WRAP_METHOD(GetLine)
			WRAP_METHOD(LineIndex)
			WRAP_METHOD(LineLength)
			WRAP_METHOD(LineScroll)
			WRAP_METHOD(GetCharPos)
			WRAP_METHOD(LineFromChar)
			WRAP_METHOD(PosFromChar)
			WRAP_METHOD(CharFromPos)
			WRAP_METHOD(EmptyUndoBuffer)
			WRAP_METHOD(SetUndoLimit)
			WRAP_METHOD(GetTextPadding)
			WRAP_METHOD(SetTextPadding)
			WRAP_METHOD(SetDropAcceptFile);
#undef WRAP_METHOD
	}
}

namespace DuiLib
{
	static CDuiString TEventUI_ToString(TEventUI*)
	{
		return _T("TEventUI");
	}
	static CDuiString TNotifyUI_ToString(TNotifyUI*)
	{
		return _T("TNotifyUI");
	}
	static CDuiString TFontInfo_ToString(TFontInfo*)
	{
		return _T("TFontInfo");
	}
	static CDuiString TImageInfo_ToString(TImageInfo*)
	{
		return _T("TImageInfo");
	}
	static CDuiString TDrawInfo_ToString(TDrawInfo*)
	{
		return _T("TDrawInfo");
	}
	static int PaintManager_Invalidate(lua_State* l)
	{
		CPaintManagerUI* pManager = nullptr;
		lua::get(l, 1, &pManager);
		if (lua_istable(l, 2))
		{
			RECT rtItem;
			lua::get(l, 2, &rtItem);
			pManager->Invalidate(rtItem);
			return lua::push(l, rtItem);
		}
		else
		{
			pManager->Invalidate();
			return 0;
		}
	}
	static int PaintManager_SetResourceZip(lua_State* l)
	{
		if (lua_gettop(l) == 1)
		{
			if (lua_isstring(l, 1))
			{
				CDuiString pstrZip;
				lua::get(l, 1, &pstrZip);
				CPaintManagerUI::SetResourceZip(pstrZip, false);
			}
			else
			{
				HANDLE hz = nullptr;
				lua::get(l, 1, &hz);
				CPaintManagerUI::SetResourceZip(hz);
			}
		}
		else if (lua_isstring(l, 1))
		{
			CDuiString pstrZip;
			lua::get(l, 1, &pstrZip);
			bool bCachedResourceZip = false;
			lua::get(l, 2, &bCachedResourceZip);
			CPaintManagerUI::SetResourceZip(pstrZip, bCachedResourceZip);
		}
		else
		{
			LPVOID pVoid = nullptr;
			unsigned int len = 0;
			lua::get(l, 1, &pVoid, &len);
			CPaintManagerUI::SetResourceZip(pVoid, len);
		}
		return 0;
	}
	static int PaintManager_GetHSL(lua_State* l)
	{
		short H, S, L;
		CPaintManagerUI::GetHSL(&H, &S, &L);
		return lua::push(l, H, S, L);
	}
	static int PaintManager_GetFont(lua_State* l)
	{
		HFONT hFont = nullptr;
		CPaintManagerUI* pManager = nullptr;
		lua::get(l, 1, &pManager);
		if (lua_gettop(l) == 2)
		{
			int id;
			lua::get(l, 2, &id);
			hFont = pManager->GetFont(id);
		}
		else
		{
			CDuiString pStrFontName;
			int nSize;
			bool bBold, bUnderline, bItalic;
			lua::get(l, 2, &pStrFontName, &nSize, &bBold, &bUnderline, &bItalic);
			hFont = pManager->GetFont(pStrFontName, nSize, bBold, bUnderline, bItalic);
		}
		return lua::push(l, hFont);
	}
	static int PaintManager_GetFontIndex(lua_State* l)
	{
		int nIndex = -1;
		CPaintManagerUI* pManager = nullptr;
		lua::get(l, 1, &pManager);
		if (lua_isstring(l, 2))
		{
			CDuiString pStrFontName;
			int nSize;
			bool bBold, bUnderline, bItalic, bShared;
			lua::get(l, 2, &pStrFontName, &nSize, &bBold, &bUnderline, &bItalic, &bShared);
			nIndex = pManager->GetFontIndex(pStrFontName, nSize, bBold, bUnderline, bItalic, bShared);
		}
		else
		{
			HFONT hFont;
			bool bShared;
			lua::get(l, 2, &hFont, &bShared);
			nIndex = pManager->GetFontIndex(hFont, bShared);
		}
		return lua::push(l, nIndex);
	}
	static int PaintManager_RemoveFont(lua_State* l)
	{
		CPaintManagerUI* pManager = nullptr;
		lua::get(l, 1, &pManager);
		if (lua_isnumber(l, 2))
		{
			int id; bool bShared;
			lua::get(l, 2, &id, &bShared);
			pManager->RemoveFont(id, bShared);
		}
		else
		{
			HFONT hFont; bool bShared;
			lua::get(l, 2, &hFont, &bShared);
			pManager->RemoveFont(hFont, bShared);
		}
		return 0;
	}
	static int PaintManager_GetFontInfo(lua_State* l)
	{
		TFontInfo* pFontInfo = nullptr;
		CPaintManagerUI* pManager = nullptr;
		lua::get(l, 1, &pManager);
		if (lua_isnumber(l, 2))
		{
			int id;
			lua::get(l, 2, &id);
			pFontInfo = pManager->GetFontInfo(id);
		}
		else
		{
			HFONT hFont;
			lua::get(l, 2, &hFont);
			pFontInfo = pManager->GetFontInfo(hFont);
		}
		return lua::push(l, pFontInfo);
	}
	static int PaintManager_AddImage(lua_State* l)
	{
		CPaintManagerUI* pManager = nullptr;
		lua::get(l, 1, &pManager);
		if (lua_gettop(l) == 2)
		{
			CDuiString bitmap;
			lua::get(l, 2, &bitmap);
			const TImageInfo* pImageInfo = pManager->AddImage(bitmap);
			return lua::push(l, pImageInfo);
		}
		else if (lua_isstring(l, 3))
		{
			CDuiString bitmap, type;
			DWORD mask = 0;
			bool bUseHSL = false, bShared = false;
			lua::get(l, 2, &bitmap, &type, &mask, &bUseHSL, &bShared);
			const TImageInfo* pImageInfo = pManager->AddImage(bitmap, type, mask, bUseHSL, bShared);
			return lua::push(l, pImageInfo);
		}
		else
		{
			CDuiString bitmap;
			HBITMAP hBitmap = nullptr;
			int iWidth, iHeight;
			bool bAlpha, bShared;
			lua::get(l, 2, &bitmap, &hBitmap, &iWidth, &iHeight, &bAlpha, &bShared);
			const TImageInfo* pImageInfo = pManager->AddImage(bitmap, hBitmap, iWidth, iHeight, bAlpha, bShared);
			return lua::push(l, pImageInfo);
		}
	}
	static int PaintManager_ProcessMultiLanguageTokens(lua_State* l)
	{
		CDuiString pStrMultiLanguage;
		lua::get(l, 1, &pStrMultiLanguage);
		CPaintManagerUI::ProcessMultiLanguageTokens(pStrMultiLanguage);
		return lua::push(l, pStrMultiLanguage);
	}
	static int PaintManager_KillTimer(lua_State* l)
	{
		CPaintManagerUI* pManager = nullptr;
		CControlUI* pControl = nullptr;
		lua::get(l, 1, &pManager, &pControl);
		if (lua_gettop(l) == 3)
		{
			UINT nTimerID;
			lua::get(l, 3, &nTimerID);
			return lua::push(l, pManager->KillTimer(pControl, nTimerID));
		}
		pManager->KillTimer(pControl);
		return 0;
	}
	static int PaintManager_SendNotify(lua_State* l)
	{
		CPaintManagerUI* pManager = nullptr;
		lua::get(l, 1, &pManager);
		if (lua_isstring(l, 3))
		{
			TNotifyUI* pMsg = nullptr;
			bool bAsync = false;
			bool bEnableRepeat = true;
			lua::get(l, 2, &pMsg);
			int top = lua_gettop(l);
			if (top > 2) lua::get(l, 3, &bAsync);
			if (top > 3) lua::get(l, 4, &bEnableRepeat);
			pManager->SendNotify(*pMsg, bAsync, bEnableRepeat);
		}
		else
		{
			CControlUI* pControl = nullptr;
			CDuiString pstrMessage;
			WPARAM wParam = 0;
			LPARAM lParam = 0;
			bool bAsync = false;
			bool bEnableRepeat = true;
			lua::get(l, 2, &pControl, &pstrMessage);
			int top = lua_gettop(l);
			if (top > 3) lua::get(l, 4, &wParam);
			if (top > 4) lua::get(l, 5, &lParam);
			if (top > 5) lua::get(l, 6, &bAsync);
			if (top > 6) lua::get(l, 7, &bEnableRepeat);
			pManager->SendNotify(pControl, pstrMessage, wParam, lParam, bAsync, bEnableRepeat);
		}
		return 0;
	}
	static int PaintManager_FindControl(lua_State* l)
	{
		CPaintManagerUI* pManager = nullptr;
		lua::get(l, 1, &pManager);
		if (lua_isstring(l, 2))
		{
			CDuiString pstrName;
			lua::get(l, 2, &pstrName);
			return lua::push(l, pManager->FindControl(pstrName));
		}
		else
		{
			POINT pt;
			lua::get(l, 2, &pt);
			return lua::push(l, pManager->FindControl(pt));
		}
	}
	
	static int RenderEngine_LoadImage(lua_State* l)
	{
		STRINGorID bitmap(_T(""));
		if (lua_isstring(l, 1))
		{
			CDuiString pstrText;
			lua::get(l, 1, &pstrText);
			bitmap = STRINGorID(pstrText.GetData());
		}
		else if (lua_isnumber(l, 1))
			bitmap = STRINGorID(lua_tointeger(l, 1));
		
		LPCTSTR type = NULL;
		if (lua_isstring(l, 2))
		{
			CDuiString pstrType;
			lua::get(l, 2, &pstrType);
			type = pstrType.GetData();
		}
		DWORD mask = 0;
		if (lua_isnumber(l, 3)) mask = (DWORD)lua_tonumber(l, 3);
		
		return lua::push(l, CRenderEngine::LoadImage(bitmap, type, mask));
	}
	static int RenderEngine_DrawImage(lua_State* l)
	{
		HDC hDC = nullptr;
		HBITMAP hBitmap = nullptr;
		RECT rc{ 0,0,0,0 };
		RECT rcPaint{ 0,0,0,0 };
		RECT rcBmpPart{ 0,0,0,0 };
		RECT rcScale9{ 0,0,0,0 };
		bool alphaChannel = false;
		BYTE uFade = 255;
		bool hole = false;
		bool xtiled = false;
		bool ytiled = false;
		lua::get(l, 1, &hDC, &hBitmap, &rc, &rcPaint, &rcBmpPart, &rcScale9);
		if (!lua_isboolean(l, 7))
			alphaChannel = lua_toboolean(l, 7);
		if (!lua_isnoneornil(l, 8))
			uFade = (BYTE)luaL_checkint(l, 8);
		if(!lua_isboolean(l, 9))
			hole = lua_toboolean(l, 9);
		if (!lua_isboolean(l, 10))
			xtiled = lua_toboolean(l, 10);
		if (!lua_isboolean(l, 11))
			ytiled = lua_toboolean(l, 11);

		CRenderEngine::DrawImage(hDC, hBitmap, rc, rcPaint, rcBmpPart, rcScale9, alphaChannel, uFade, hole, xtiled, ytiled);

		return 0;
	}
	static int RenderEngine_DrawImage2(lua_State* l)
	{
		HDC hDC = nullptr;
		CPaintManagerUI* pManager = nullptr;
		RECT rcItem{ 0,0,0,0 };
		RECT rcPaint{ 0,0,0,0 };
		TDrawInfo* drawInfo = nullptr;
		lua::get(l, 1, &hDC, &pManager, &rcItem, &rcPaint, &drawInfo);

		return lua::push(l, CRenderEngine::DrawImage(hDC, pManager, rcItem, rcPaint, *drawInfo));
	}
	static int RenderEngine_DrawHtmlText(lua_State* l)
	{
		HDC hDC = nullptr;
		CPaintManagerUI* pManager = nullptr;
		RECT rc{ 0,0,0,0 };
		CDuiString pstrText;
		DWORD dwTextColor;
		std::vector<RECT> pLinks;
		std::vector<CDuiString> sLinks;
		int nLinkRects;
		int iDefaultFont;
		UINT uStyle;

		lua::get(l, 1, &hDC, &pManager, &rc, &pstrText, &dwTextColor, &pLinks, &sLinks, &nLinkRects, &iDefaultFont, &uStyle);
		CRenderEngine::DrawHtmlText(hDC, pManager, rc, pstrText, dwTextColor, &(pLinks[0]), &(sLinks[0]), nLinkRects, iDefaultFont, uStyle);

		return lua::push(l, pLinks, sLinks, nLinkRects);
	}
	static int RenderEngine_GenerateBitmap(lua_State* l)
	{
		CPaintManagerUI* pManager = nullptr;
		lua::get(l, 1, &pManager);
		if (lua_istable(l, 2))
		{
			RECT rc;
			CControlUI* pStopControl = NULL;
			DWORD dwFilterColor = 0;
			lua::get(l, 2, &rc);
			if (lua_isuserdata(l, 3))
				lua::get(l, 3, &pStopControl);
			if (lua_isnumber(l, 4))
				lua::get(l, 4, &dwFilterColor);
			HBITMAP hBitmap = CRenderEngine::GenerateBitmap(pManager, rc, pStopControl, dwFilterColor);
			return lua::push(l, hBitmap);
		}
		else
		{
			CControlUI* pControl;
			RECT rc;
			DWORD dwFilterColor = 0;
			lua::get(l, 2, &pControl, &rc);
			if (lua_isnumber(l, 4))
				lua::get(l, 4, &dwFilterColor);
			HBITMAP hBitmap = CRenderEngine::GenerateBitmap(pManager, pControl, rc, dwFilterColor);
			return lua::push(l, hBitmap);
		}
	}

	static int WindowWnd_Create(lua_State* l)
	{
		CWindowWnd* pWin = nullptr;
		HWND hwndParent = nullptr;
		CDuiString pstrName;
		DWORD dwStyle, dwExStyle;
		lua::get(l, 1, &pWin, &hwndParent, &pstrName, &dwStyle, &dwExStyle);
		if (lua_isnoneornil(l, 6))
		{
			return lua::push(l, pWin->Create(hwndParent, pstrName, dwStyle, dwExStyle));
		}
		else if (lua_isnumber(l, 6))
		{
			int x = CW_USEDEFAULT, y = CW_USEDEFAULT, cx = CW_USEDEFAULT, cy = CW_USEDEFAULT;
			HMENU hMenu = nullptr;
			lua::get(l, 6, &x);
			if (lua_isnumber(l, 7)) lua::get(l, 7, &y);
			if (lua_isnumber(l, 8)) lua::get(l, 8, &cy);
			if (lua_isnumber(l, 9)) lua::get(l, 9, &cy);
			lua::get(l, 10, &hMenu);
			return lua::push(l, pWin->Create(hwndParent, pstrName, dwStyle, dwExStyle, x, y, cx, cy, hMenu));
		}
		else
		{
			RECT rt;
			HMENU hMenu = nullptr;
			lua::get(l, 6, &rt, &hMenu);
			HWND hWnd = pWin->Create(hwndParent, pstrName, dwStyle, dwExStyle, rt, hMenu);
			return lua::push(l, hWnd);
		}
	}
	static int WindowWnd_SendMessage(lua_State* l)
	{
		UINT uMsg; WPARAM wParam = 0; LPARAM lParam = 0;
		CWindowWnd* pWin = nullptr;
		lua::get(l, 1, &pWin, &uMsg);
		if (lua_isnumber(l, 3))
			wParam = (WPARAM)(lua_tonumber(l, 3));
		else if (lua_isboolean(l, 3))
			wParam = (WPARAM)(lua_toboolean(l, 3));
		else if (lua_isuserdata(l, 3))
			wParam = (WPARAM)(lua_touserdata(l, 3));
		else if (lua_isstring(l, 3))
		{
			CDuiString str;
			lua::get(l, 3, &str);
			wParam = (WPARAM)str.GetData();
		}
		if (lua_isnumber(l, 4))
			lParam = (LPARAM)(lua_tonumber(l, 4));
		else if (lua_isboolean(l, 4))
			lParam = (LPARAM)(lua_toboolean(l,43));
		else if (lua_isuserdata(l, 4))
			lParam = (LPARAM)(lua_touserdata(l, 4));
		else if (lua_isstring(l, 4))
		{
			CDuiString str;
			lua::get(l, 4, &str);
			lParam = (LPARAM)str.GetData();
		}
		pWin->SendMessage(uMsg, wParam, lParam);
		return 0;
	}
	static int WindowWnd_PostMessage(lua_State* l)
	{
		UINT uMsg; WPARAM wParam = 0; LPARAM lParam = 0;
		CWindowWnd* pWin = nullptr;
		lua::get(l, 1, &pWin, &uMsg);
		if (lua_isnumber(l, 3))
			wParam = (WPARAM)(lua_tonumber(l, 3));
		else if (lua_isboolean(l, 3))
			wParam = (WPARAM)(lua_toboolean(l, 3));
		else if (lua_isuserdata(l, 3))
			wParam = (WPARAM)(lua_touserdata(l, 3));
		else if (lua_isstring(l, 3))
		{
			CDuiString str;
			lua::get(l, 3, &str);
			wParam = (WPARAM)str.GetData();
		}
		if (lua_isnumber(l, 4))
			lParam = (LPARAM)(lua_tonumber(l, 4));
		else if (lua_isboolean(l, 4))
			lParam = (LPARAM)(lua_toboolean(l, 43));
		else if (lua_isuserdata(l, 4))
			lParam = (LPARAM)(lua_touserdata(l, 4));
		else if (lua_isstring(l, 4))
		{
			CDuiString str;
			lua::get(l, 4, &str);
			lParam = (LPARAM)str.GetData();
		}
		pWin->PostMessage(uMsg, wParam, lParam);
		return 0;
	}
	static int WindowWnd_SetIconFromFile(lua_State* l)
	{
		CWindowWnd* pWin = nullptr;
		lua::get(l, 1, &pWin);
		HINSTANCE hInstance = nullptr;
		CDuiString pstrFileName;
		lua::get(l, 2, &pstrFileName, &hInstance);

		HICON hIcon = (HICON)::LoadImage(hInstance, pstrFileName, IMAGE_ICON, (::GetSystemMetrics(SM_CXICON) + 15) & ~15, (::GetSystemMetrics(SM_CYICON) + 15) & ~15, LR_DEFAULTCOLOR | LR_LOADFROMFILE);
		ASSERT(hIcon);
		if (!hIcon) return 0;
		::SendMessage(*pWin, WM_SETICON, (WPARAM)TRUE, (LPARAM)hIcon);
		// UMU: 防止某些 DPI 设置下图标模糊
		hIcon = (HICON)::LoadImage(hInstance, pstrFileName, IMAGE_ICON, (::GetSystemMetrics(SM_CXSMICON) + 15) & ~15, (::GetSystemMetrics(SM_CYSMICON) + 15) & ~15, LR_DEFAULTCOLOR | LR_LOADFROMFILE);
		ASSERT(hIcon);
		if (!hIcon) return 0;
		::SendMessage(*pWin, WM_SETICON, (WPARAM)FALSE, (LPARAM)hIcon);
		return 0;
	}

	static int CWin_MoveWindow(lua_State* l)
	{
		CWin* pWin = nullptr;
		lua::get(l, 1, &pWin);
		if (lua_istable(l, 2))
		{
			RECT rt;
			BOOL bRepaint = TRUE;
			lua::get(l, 2, &rt);
			if (lua_isboolean(l, 3)) lua::get(l, 3, &bRepaint);
			return lua::push(l, pWin->MoveWindow(&rt, bRepaint));
		}
		else
		{
			int x, y, nWidth, nHeight;
			lua::get(l, 2, &x, &y, &nWidth, &nHeight);
			BOOL bRepaint = TRUE;
			if (lua_isboolean(l, 3)) lua::get(l, 3, &bRepaint);
			return lua::push(l, pWin->MoveWindow(x, y, nWidth, nHeight, bRepaint));
		}
	}
	static int CWin_SetWindowPos(lua_State* l)
	{
		CWin* pWin = nullptr;
		HWND hWndInsertAfter = nullptr;
		lua::get(l, 1, &pWin, &hWndInsertAfter);
		if (lua_istable(l, 3))
		{
			RECT rt;
			UINT nFlags;
			lua::get(l, 3, &rt, &nFlags);
			return lua::push(l, pWin->SetWindowPos(hWndInsertAfter, &rt, nFlags));
		}
		else
		{
			int x, y, nWidth, nHeight;
			UINT nFlags;
			lua::get(l, 3, &x, &y, &nWidth, &nHeight, &nFlags);
			return lua::push(l, pWin->SetWindowPos(hWndInsertAfter, x, y, nWidth, nHeight, nFlags));
		}
	}
	static RECT CWin_GetWindowRect(CWin* pWindow)
	{
		RECT rc;
		pWindow->GetWindowRect(&rc);
		return rc;
	}
	static RECT CWin_GetClientRect(CWin* pWindow)
	{
		RECT rc;
		pWindow->GetClientRect(&rc);
		return rc;
	}
	static int CWin_ClientToScreen(lua_State* l)
	{
		CWin* pWin = nullptr;
		lua::get(l, 1, &pWin);
		lua::lua_table_ref_t t;
		lua::get(l, 2, &t);
		if (t.has("left"))
		{
			RECT rt;
			lua::get(l, 2, &rt);
			bool b = pWin->ClientToScreen(&rt);
			b ? lua::push(l, rt) : lua::push(l);
		}
		else
		{
			POINT pt;
			lua::get(l, 2, &pt);
			bool b = pWin->ClientToScreen(&pt);
			b ? lua::push(l, pt) : lua::push(l);
		}
		t.unref();
		return 1;
	}
	static int CWin_ScreenToClient(lua_State* l)
	{
		CWin* pWin = nullptr;
		lua::get(l, 1, &pWin);
		lua::lua_table_ref_t t;
		lua::get(l, 2, &t);
		if (t.has("left"))
		{
			RECT rt;
			lua::get(l, 2, &rt);
			bool b = pWin->ScreenToClient(&rt);
			b ? lua::push(l, rt) : lua::push(l);
		}
		else
		{
			POINT pt;
			lua::get(l, 2, &pt);
			bool b = pWin->ScreenToClient(&pt);
			b ? lua::push(l, pt) : lua::push(l);
		}
		t.unref();
		return 1;
	}
	static CControlUI* CWin_FindControl(CWin* pWindow, CDuiString pstrText)
	{
		CControlUI* pControl = nullptr;
		pWindow->FindControl(pControl, pstrText);
		return pControl;
	}
	static CDuiString CWin_GetWindowText(CWin* pWindow)
	{
		TCHAR szBuf[256] = { 0 };
		pWindow->GetWindowText(szBuf, 256);
		return szBuf;
	}
	static int CWin_MsgBox(lua_State* l)
	{
		CLuaWindow* pWin = nullptr;
		CDuiString pstrText;
		lua::get(l, 1, &pWin, &pstrText);
		CDuiString lpCaption;
		if (lua_isstring(l, 3)) lua::get(l, 3, &lpCaption);
		UINT nType = MBBT_OKCANCLE;
		if (lua_isnumber(l, 4)) nType = (UINT)lua_tonumber(l, 4);
		int lua_callback = -1;
		if (lua_isfunction(l, 5)) lua_callback = luaL_ref(l, LUA_REGISTRYINDEX);
		LPCTSTR type = nullptr;
		if (lua_isstring(l, 6)) {
			CDuiString p;
			lua::get(l, 6, &p);
			type = p;
		}
		if (CResourceManager::Instance()->FindAsset(_T("xml/msgbox.xml"), type))
		{
			UINT nRet = pWin->MsgBox(pstrText, lpCaption, nType);
			if (lua_callback != -1)
			{
				lua_rawgeti(l, LUA_REGISTRYINDEX, lua_callback);
				lua_unref(l, lua_callback);
				lua_pushnumber(l, nRet);
				lua_call(l, 1, 0);
			}
		}
		else
		{
			int Type = MB_OK;
			if ((nType & MBT_OK) > 0)
				Type |= MB_OK;
			if ((nType & MBT_INFO) > 0)
				Type |= MB_ICONINFORMATION;
			if ((nType & MBT_WARN) > 0)
				Type |= MB_ICONWARNING;
			if ((nType & MBBT_CANCEL) >0)
				Type |= MB_OKCANCEL & (~MB_OK);
			if ((nType & MBBT_OK) >0)
				Type |= MB_OK;
			if ((nType & MBBT_OKCANCLE) > 0)
				Type |= MB_OKCANCEL;

			UINT nRet = ::MessageBox(*pWin, pstrText, lpCaption, Type);
			if (lua_callback != -1)
			{
				lua_rawgeti(l, LUA_REGISTRYINDEX, lua_callback);
				lua_unref(l, lua_callback);
				lua_pushnumber(l, nRet);
				lua_call(l, 1, 0);
				lua_pop(l, 1);
			}
		}
		return 0;
	}

	static int LuaWindow_RegisterSkin(lua_State* l)
	{
		CLuaWindow* pWin = nullptr;
		lua::get(l, 1, &pWin);

		STRINGorID xml(_T(""));
		if (lua_isstring(l, 2))
		{
			CDuiString pstrText;
			lua::get(l, 2, &pstrText);
			xml = STRINGorID(pstrText.GetData());
		}
		else if (lua_isnumber(l, 2))
			xml = STRINGorID(lua_tointeger(l, 2));

		LPCTSTR type = nullptr;
		lua::get(l, 3, &type);
		CControlUI* pParent = nullptr;
		lua::get(l, 4, &pParent);

		return lua::push(l, pWin->RegisterSkin(xml, type, pWin, pParent));
	}
	static int LuaWindow_New(lua_State* l)
	{
		CDuiString szName;
		lua::get(l, 1, &szName);
		UINT nStyle = CS_DBLCLKS;
		if (lua_isnumber(l, 2)) lua::get(l, 2, &nStyle);
		CLuaWindow* pWin = new CLuaWindow(szName, nStyle);
		return lua::push(l, pWin);
	}
	static int LuaWindow_BindLuaScript(lua_State* l)
	{
		CLuaWindow* pWin = nullptr;
		lua::get(l, 1, &pWin);

		pWin->BindLuaScript();

		return 0;
	}
	static void LuaWindow_SetListCallback(CLuaWindow* pWin, CListUI* pList)
	{
		pList->SetTextCallback(pWin);
	}
	static void LuaWindow_AddPreMessageFilter(CLuaWindow* pWin)
	{
		pWin->PaintMgr()->AddPreMessageFilter(pWin);
	}
	static void LuaWindow_AddMessageFilter(CLuaWindow* pWin)
	{
		pWin->PaintMgr()->AddMessageFilter(pWin);
	}
	static void LuaWindow_AddNotifier(CLuaWindow* pWin)
	{
		pWin->PaintMgr()->AddNotifier(pWin);
	}
	static void LuaWindow_ListenUIEvent(CLuaWindow* pWin, CControlUI* pControl)
	{
		pControl->OnEvent += MakeDelegate(pWin, &CLuaWindow::onUIEvent);
	}
	static void LuaWindow_ListenUIDestroy(CLuaWindow* pWin, CControlUI* pControl)
	{
		pControl->OnDestroy += MakeDelegate(pWin, &CLuaWindow::onUIDestroy);
	}
	static void LuaWindow_ListenUISize(CLuaWindow* pWin, CControlUI* pControl)
	{
		pControl->OnSize += MakeDelegate(pWin, &CLuaWindow::onUISize);
	}
	static void LuaWindow_ListenUINotify(CLuaWindow* pWin, CControlUI* pControl)
	{
		pControl->OnNotify += MakeDelegate(pWin, &CLuaWindow::onUINotify);
	}
	static void LuaWindow_ListenUIPaint(CLuaWindow* pWin, CControlUI* pControl)
	{
		pControl->OnPaint += MakeDelegate(pWin, &CLuaWindow::onUIPaint);
	}

	//CResourceManager
	static bool ResourceManager_FindAsset(lua_State* l)
	{
		CDuiString name;
		LPCTSTR type = nullptr;
		lua::get(l, 1, &name);
		if (lua_isstring(l, 2))
		{
			CDuiString s; lua::get(l, 2, &s);
			type = s;
		}
		bool b = CResourceManager::Instance()->FindAsset(name, type);
		return lua::push(l, b);
	}
	static int ResourceManager_LoadAsset(lua_State* l)
	{
		CDuiString name;
		LPCTSTR type = nullptr;
		lua::get(l, 1, &name);
		if (lua_isstring(l, 2))
		{
			CDuiString s; lua::get(l, 2, &s);
			type = s;
		}

		LPBYTE pData = NULL;
		DWORD dwSize = 0;
		bool success = CResourceManager::Instance()->LoadAsset(name, type, &pData, &dwSize);
		if (success)
		{
			std::vector<unsigned char> ByteBuffer(dwSize);
			memcpy(&ByteBuffer[0], pData, dwSize);
			lua_pushlstring(l, (char*)&ByteBuffer[0], ByteBuffer.size());
			lua_pushinteger(l, dwSize);
			CResourceManager::Instance()->FreeAsset(pData);
			return 2;
		}
		else
		{
			lua_pushnil(l);
			return 1;
		}
	}

	static int CFileDialog_GetAllFileList(lua_State* l)
	{
		CFileDialog* pFileDalog = nullptr;
		lua::get(l, 1, &pFileDalog);
		std::vector<CDuiString> filelist;
		auto pos = pFileDalog->GetStartPosition();
		while (pos)
		{
			filelist.push_back(pFileDalog->GetNextPathName(pos));
		}
		return lua::push(l, filelist);
	}
	static int CFileDialog_SetFilter(lua_State* l)
	{
		CFileDialog* pFileDalog = nullptr;
		CDuiString strText;
		lua::get(l, 1, &pFileDalog, &strText);
		//strText.Replace(_T("$"), _T("\0"));
		pFileDalog->SetFilter(strText);
		return 0;
	}

	static int CTrayIconUI_DeleteTrayIcon(lua_State* l)
	{
		CTrayIconUI::Instance()->DeleteTrayIcon();
		return 0;
	}
	static int CTrayIconUI_CreateTrayIcon(lua_State* l)
	{
		HWND _RecvHwnd = nullptr;
		UINT _IconIDResource = 0;
		LPCTSTR _ToolTipText = NULL;
		UINT _Message = NULL/*(UIEVENT_TRAYICON)*/;
		CPaintManagerUI* pManager = NULL;
		lua::get(l, 1, &_RecvHwnd, &_IconIDResource);
		if (lua_isstring(l, 3))
		{
			CDuiString s; 
			lua::get(l, 3, &s);
			_ToolTipText = s.GetData();
		}
		if (lua_isnumber(l, 4))
			lua::get(l, 4, &_Message);
		if (lua_isuserdata(l, 5))
			lua::get(l, 5, &pManager);

		return lua::push(l, CTrayIconUI::Instance()->CreateTrayIcon(_RecvHwnd, _IconIDResource, _ToolTipText, _Message, pManager));
	}
	static int CTrayIconUI_CreateTrayIconFromIcon(lua_State* l)
	{
		HWND _RecvHwnd = nullptr;
		UINT _IconIDResource = 0;
		HICON hIcon = NULL;
		LPCTSTR _ToolTipText = NULL;
		UINT _Message = NULL/*(UIEVENT_TRAYICON)*/;
		CPaintManagerUI* pManager = NULL;
		lua::get(l, 1, &_RecvHwnd, &_IconIDResource, &hIcon);
		if (lua_isstring(l, 4))
		{
			CDuiString ss;
			lua::get(l, 4, &ss);
			_ToolTipText = ss.GetData();
		}
		if (lua_isnumber(l, 5))
			lua::get(l, 5, &_Message);
		if (lua_isuserdata(l, 6))
			lua::get(l, 6, &pManager);

		return lua::push(l, CTrayIconUI::Instance()->CreateTrayIconFromIcon(_RecvHwnd, _IconIDResource, hIcon, _ToolTipText, _Message, pManager));
	}
	static int CTrayIconUI_CreateTrayIconFromFile(lua_State* l)
	{
		HWND _RecvHwnd = nullptr;
		UINT _IconIDResource = 0;
		CDuiString lpstrFileName;
		LPCTSTR _ToolTipText = NULL;
		UINT _Message = NULL/*(UIEVENT_TRAYICON)*/;
		CPaintManagerUI* pManager = NULL;
		lua::get(l, 1, &_RecvHwnd, &_IconIDResource, &lpstrFileName);
		if (lua_isstring(l, 4))
		{
			CDuiString ss;
			lua::get(l, 4, &ss);
			_ToolTipText = ss.GetData();
		}
		if (lua_isnumber(l, 5))
			lua::get(l, 5, &_Message);
		if (lua_isuserdata(l, 6))
			lua::get(l, 6, &pManager);

		return lua::push(l, CTrayIconUI::Instance()->CreateTrayIconFromFile(_RecvHwnd, _IconIDResource, lpstrFileName, _ToolTipText, _Message, pManager));
	}
	static int CTrayIconUI_SetTooltipText(lua_State* l)
	{
		UINT _IDResource = 0;
		if (lua_isstring(l, 1))
		{
			CDuiString _ToolTipText;
			lua::get(l, 1, &_ToolTipText);
			return lua::push(l, CTrayIconUI::Instance()->SetTooltipText(_ToolTipText));
		}
		else
		{
			lua::get(l, 1, &_IDResource);
			return lua::push(l, CTrayIconUI::Instance()->SetTooltipText(_IDResource));
		}
	}
	static int CTrayIconUI_GetTooltipText(lua_State* l)
	{
		return lua::push(l, CTrayIconUI::Instance()->GetTooltipText());
	}
	static int CTrayIconUI_ShowInfotip(lua_State* l)
	{
		CDuiString szMsg;
		CDuiString szTitle;
		DWORD dwInfoFlags = NIIF_INFO;
		UINT uTimeout = 1000 * 2;
		lua::get(l, 1, &szMsg, &szTitle);
		if (lua_isnumber(l, 3))
			lua::get(l, 3, &dwInfoFlags);
		if (lua_isnumber(l, 4))
			lua::get(l, 4, &uTimeout);
		return lua::push(l, CTrayIconUI::Instance()->ShowInfotip(szMsg, szTitle, dwInfoFlags, uTimeout));
	}
	static int CTrayIconUI_SetIcon(lua_State* l)
	{
		HICON hIcon = nullptr;
		UINT _IDResource;
		if (lua_isstring(l, 1))
		{
			CDuiString _IconFile;
			lua::get(l, 1, &_IconFile);
			return lua::push(l, CTrayIconUI::Instance()->SetIcon(_IconFile));
		}
		else if (lua_isuserdata(l, 1))
		{
			lua::get(l, 1, &hIcon);
			return lua::push(l, CTrayIconUI::Instance()->SetIcon(hIcon));
		}
		else if (lua_isnumber(l, 1))
		{
			lua::get(l, 1, &_IDResource);
			return lua::push(l, CTrayIconUI::Instance()->SetIcon(_IDResource));
		}
		return 0;
	}
	static int CTrayIconUI_GetIcon(lua_State* l)
	{
		return lua::push(l, CTrayIconUI::Instance()->GetIcon());
	}
	static int CTrayIconUI_SetHideIcon(lua_State* l)
	{
		CTrayIconUI::Instance()->SetHideIcon();
		return 0;
	}
	static int CTrayIconUI_SetShowIcon(lua_State* l)
	{
		CTrayIconUI::Instance()->SetShowIcon();
		return 0;
	}
	static int CTrayIconUI_RemoveIcon(lua_State* l)
	{
		CTrayIconUI::Instance()->RemoveIcon();
		return 0;
	}
	static int CTrayIconUI_StartTwinkling(lua_State* l)
	{
		return lua::push(l, CTrayIconUI::Instance()->StartTwinkling());
	}
	static int CTrayIconUI_StopTwinkling(lua_State* l)
	{
		CTrayIconUI::Instance()->StopTwinkling();
		return 0;
	}
	static int CTrayIconUI_Enabled(lua_State* l)
	{
		return lua::push(l, CTrayIconUI::Instance()->Enabled());
	}
	static int CTrayIconUI_IsVisible(lua_State* l)
	{
		return lua::push(l, CTrayIconUI::Instance()->IsVisible());
	}
	static int CTrayIconUI_IsTwinkling(lua_State* l)
	{
		return lua::push(l, CTrayIconUI::Instance()->IsTwinkling());
	}
	static int CTrayIconUI_IsMouseHover(lua_State* l)
	{
		return lua::push(l, CTrayIconUI::Instance()->IsMouseHover());
	}

	static int CMenuWnd_New(lua_State* l)
	{
		HWND hParent = nullptr;
		if (lua_isuserdata(l, 1))
			lua::get(l, 1, &hParent);
		CMenuWnd* pMenu = new CMenuWnd(hParent);
		return lua::push(l, pMenu);
	}
	static int CMenuWnd_Init(lua_State* l)
	{
		CMenuWnd* pMenu = nullptr;
		CMenuElementUI* pOwner = nullptr;
		STRINGorID xml(_T(""));
		CDuiString SkinType;
		POINT point;
		lua::get(l, 1, &pMenu, &pOwner);
		if (lua_isstring(l, 3))
		{
			CDuiString pstrText;
			lua::get(l, 3, &pstrText);
			xml = STRINGorID(pstrText.GetData());
		}
		else if (lua_isnumber(l, 3))
			xml = STRINGorID(lua_tointeger(l, 3));

		lua::get(l, 4, &SkinType, &point);
		pMenu->Init(pOwner, xml, SkinType, point);
		return 0;
	}

	static void RegisterDuiLibCoreToLua(lua_State* l)
	{
		//DuiLib
#define WRAP_METHOD(m) .readonly(#m, m)
		lua::lua_register_t<void>(l, "DuiLib")
			WRAP_METHOD(UI_WNDSTYLE_CONTAINER)
			WRAP_METHOD(UI_WNDSTYLE_FRAME)
			WRAP_METHOD(WS_VISIBLE)
			WRAP_METHOD(WS_OVERLAPPEDWINDOW)
			WRAP_METHOD(WS_EX_OVERLAPPEDWINDOW)
			WRAP_METHOD(UI_WNDSTYLE_CHILD)
			WRAP_METHOD(WS_CHILD)
			WRAP_METHOD(WS_CLIPSIBLINGS)
			WRAP_METHOD(WS_CLIPCHILDREN)
			WRAP_METHOD(UI_WNDSTYLE_DIALOG)
			WRAP_METHOD(WS_POPUPWINDOW)
			WRAP_METHOD(WS_CAPTION)
			WRAP_METHOD(WS_DLGFRAME)
			WRAP_METHOD(UI_WNDSTYLE_EX_FRAME)
			WRAP_METHOD(WS_EX_WINDOWEDGE)
			WRAP_METHOD(UI_WNDSTYLE_EX_DIALOG)
			WRAP_METHOD(WS_EX_TOOLWINDOW)
			WRAP_METHOD(WS_EX_DLGMODALFRAME)
			WRAP_METHOD(UI_CLASSSTYLE_CONTAINER)
			WRAP_METHOD(UI_CLASSSTYLE_FRAME)
			WRAP_METHOD(UI_CLASSSTYLE_CHILD)
			WRAP_METHOD(UI_CLASSSTYLE_DIALOG)
			WRAP_METHOD(WS_EX_STATICEDGE)
			WRAP_METHOD(WS_EX_APPWINDOW)
			WRAP_METHOD(SC_MINIMIZE)
			WRAP_METHOD(SC_MAXIMIZE)
			WRAP_METHOD(SC_RESTORE)
			WRAP_METHOD(SC_CLOSE)
			WRAP_METHOD(WM_SETICON);

#undef WRAP_METHOD
		//MsgArgs
#define WRAP_METHOD(m) .readonly(#m, m)
		lua::lua_register_t<void>(l, "DuiLib.MsgArgs")
			WRAP_METHOD(WM_USER)
			WRAP_METHOD(WM_CREATE)
			WRAP_METHOD(WM_CLOSE)
			WRAP_METHOD(WM_DESTROY)
			WRAP_METHOD(WM_QUIT)
#if defined(WIN32) && !defined(UNDER_CE)
			WRAP_METHOD(WM_NCCREATE)
			WRAP_METHOD(WM_NCMOUSEMOVE)
			WRAP_METHOD(WM_NCMOUSELEAVE)
			WRAP_METHOD(WM_NCACTIVATE)
			WRAP_METHOD(WM_NCCALCSIZE)
			WRAP_METHOD(WM_NCPAINT)
			WRAP_METHOD(WM_NCHITTEST)
			WRAP_METHOD(WM_GETMINMAXINFO)
			WRAP_METHOD(WM_MOUSEWHEEL)
#endif
			WRAP_METHOD(WM_TIMER)
			WRAP_METHOD(WM_MOVING)
			WRAP_METHOD(WM_SIZING)
			WRAP_METHOD(WM_SYSCOMMAND)
			WRAP_METHOD(WM_MENUCOMMAND)
			WRAP_METHOD(WM_KEYDOWN)
			WRAP_METHOD(WM_KEYUP);
#undef WRAP_METHOD

		//UILIB_RESOURCETYPE
#define WRAP_METHOD(m).readonly(#m, (int)(DuiLib::EVENTTYPE_UI::m))
		lua::lua_register_t<void>(l, "DuiLib.EVENTTYPE_UI")
			WRAP_METHOD(UIEVENT__FIRST)
			WRAP_METHOD(UIEVENT__KEYBEGIN)
			WRAP_METHOD(UIEVENT_KEYDOWN)
			WRAP_METHOD(UIEVENT_KEYUP)
			WRAP_METHOD(UIEVENT_CHAR)
			WRAP_METHOD(UIEVENT_SYSKEY)
			WRAP_METHOD(UIEVENT__KEYEND)
			WRAP_METHOD(UIEVENT__MOUSEBEGIN)
			WRAP_METHOD(UIEVENT_MOUSEMOVE)
			WRAP_METHOD(UIEVENT_MOUSELEAVE)
			WRAP_METHOD(UIEVENT_MOUSEENTER)
			WRAP_METHOD(UIEVENT_MOUSEHOVER)
			WRAP_METHOD(UIEVENT_BUTTONDOWN)
			WRAP_METHOD(UIEVENT_BUTTONUP)
			WRAP_METHOD(UIEVENT_RBUTTONDOWN)
			WRAP_METHOD(UIEVENT_RBUTTONUP)
			WRAP_METHOD(UIEVENT_DBLCLICK)
			WRAP_METHOD(UIEVENT_RDBLCLICK)
			WRAP_METHOD(UIEVENT_CONTEXTMENU)
			WRAP_METHOD(UIEVENT_SCROLLWHEEL)
			WRAP_METHOD(UIEVENT__MOUSEEND)
			WRAP_METHOD(UIEVENT_KILLFOCUS)
			WRAP_METHOD(UIEVENT_SETFOCUS)
			WRAP_METHOD(UIEVENT_WINDOWSIZE)
			WRAP_METHOD(UIEVENT_SETCURSOR)
			WRAP_METHOD(UIEVENT_TIMER)
			WRAP_METHOD(UIEVENT_NOTIFY)
			WRAP_METHOD(UIEVENT_COMMAND)
			WRAP_METHOD(UIEVENT__CUSTOMBEGIN)
			WRAP_METHOD(UIEVENT_MOUSEINTRAYICON)
			WRAP_METHOD(UIEVENT_MOUSEOUTTRAYICON)
			WRAP_METHOD(UIEVENT__CUSTOMEND)
			WRAP_METHOD(UIEVENT__LAST);
#undef WRAP_METHOD

		//UILIB_RESOURCETYPE
#define WRAP_METHOD(m).readonly(#m, m)
			lua::lua_register_t<void>(l, "DuiLib.UILIB_RESOURCETYPE")
			WRAP_METHOD(UILIB_FILE)
			WRAP_METHOD(UILIB_ZIP)
			WRAP_METHOD(UILIB_RESOURCE)
			WRAP_METHOD(UILIB_ZIPRESOURCE);
#undef WRAP_METHOD

		//TEventUI
#define WRAP_METHOD(m) .def(#m, &TEventUI::m)
		lua::lua_register_t<TEventUI>(l, "DuiLib.TEventUI")
			.def(lua::constructor<>())
			.def(lua::destructor())
			LAMBDA_METHOD2("__tostring", TEventUI_ToString)
			WRAP_METHOD(Type)
			WRAP_METHOD(pSender)
			WRAP_METHOD(dwTimestamp)
			WRAP_METHOD(ptMouse)
			WRAP_METHOD(chKey)
			WRAP_METHOD(wKeyState)
			WRAP_METHOD(wParam)
			WRAP_METHOD(lParam);
#undef WRAP_METHOD
		//TNotifyUI
#define WRAP_METHOD(m) .def(#m, &TNotifyUI::m)
		lua::lua_register_t<TNotifyUI>(l, "DuiLib.TNotifyUI")
			.def(lua::constructor<>())
			.def(lua::destructor())
			LAMBDA_METHOD2("__tostring", TNotifyUI_ToString)
			WRAP_METHOD(sType)
			WRAP_METHOD(sVirtualWnd)
			WRAP_METHOD(pSender)
			WRAP_METHOD(dwTimestamp)
			WRAP_METHOD(ptMouse)
			WRAP_METHOD(wParam)
			WRAP_METHOD(lParam);
#undef WRAP_METHOD
		//TFontInfo
#define WRAP_METHOD(m) .def(#m, &TFontInfo::m)
		lua::lua_register_t<TFontInfo>(l, "DuiLib.TFontInfo")
			.def(lua::constructor<>())
			.def(lua::destructor())
			LAMBDA_METHOD2("__tostring", TFontInfo_ToString)
			WRAP_METHOD(hFont)
			WRAP_METHOD(sFontName)
			WRAP_METHOD(iSize)
			WRAP_METHOD(bBold)
			WRAP_METHOD(bUnderline)
			WRAP_METHOD(bItalic);
#undef WRAP_METHOD
		//TImageInfo
#define WRAP_METHOD(m) .def(#m, &TImageInfo::m)
		lua::lua_register_t<TImageInfo>(l, "DuiLib.TImageInfo")
			.def(lua::constructor<>())
			.def(lua::destructor())
			LAMBDA_METHOD2("__tostring", TImageInfo_ToString)
			WRAP_METHOD(hBitmap)
			WRAP_METHOD(pBits)
			WRAP_METHOD(pSrcBits)
			WRAP_METHOD(nX)
			WRAP_METHOD(nY)
			WRAP_METHOD(bAlpha)
			WRAP_METHOD(bUseHSL)
			WRAP_METHOD(sResType)
			WRAP_METHOD(dwMask);
#undef WRAP_METHOD
		//TDrawInfo
#define WRAP_METHOD(m) .def(#m, &TDrawInfo::m)
		lua::lua_register_t<TDrawInfo>(l, "DuiLib.TDrawInfo")
			.def(lua::constructor<>())
			.def(lua::destructor())
			LAMBDA_METHOD2("__tostring", TDrawInfo_ToString)
			WRAP_METHOD(Clear)
			WRAP_METHOD(sDrawString)
			WRAP_METHOD(sImageName)
			WRAP_METHOD(bLoaded)
			.readonly("pImageInfo", &TDrawInfo::pImageInfo)
			WRAP_METHOD(rcDestOffset)
			WRAP_METHOD(rcBmpPart)
			WRAP_METHOD(rcScale9)
			WRAP_METHOD(uFade)
			WRAP_METHOD(bHole)
			WRAP_METHOD(bTiledX)
			WRAP_METHOD(bTiledY);
#undef WRAP_METHOD
		
		//CDialogBuilder
#define WRAP_METHOD(m) .def(#m, &CDialogBuilder::m)
		lua::lua_register_t<CDialogBuilder>(l, "DuiLib.CDialogBuilder")
			.def(lua::constructor<>())
			.def(lua::destructor())
			WRAP_METHOD(GetMarkup);
#undef WRAP_METHOD
		//CMarkup
#define WRAP_METHOD(m) .def(#m, &CMarkup::m)
		lua::lua_register_t<CMarkup>(l, "DuiLib.CMarkup")
			.def(lua::constructor<>())
			.def(lua::destructor())
			WRAP_METHOD(Load)
			WRAP_METHOD(Release)
			WRAP_METHOD(IsValid);
#undef WRAP_METHOD
		//CMarkupNode
#define WRAP_METHOD(m) .def(#m, &CMarkupNode::m)
		lua::lua_register_t<CMarkupNode>(l, "DuiLib.CMarkupNode")
			.def(lua::destructor())
			WRAP_METHOD(HasSiblings)
			WRAP_METHOD(GetName)
			WRAP_METHOD(GetValue)
			WRAP_METHOD(HasAttributes)
			WRAP_METHOD(HasAttribute)
			WRAP_METHOD(GetAttributeCount)
			WRAP_METHOD(GetAttributeName);
#undef WRAP_METHOD
		//CRenderEngine
#define WRAP_METHOD(m) .def(#m, &CRenderEngine::m)
		lua::lua_register_t<CRenderEngine>(l, "DuiLib.CRenderEngine", nullptr)
			WRAP_METHOD(AdjustColor)
			WRAP_METHOD(CreateARGB32Bitmap)
			WRAP_METHOD(AdjustImage)
			LAMBDA_METHOD2("LoadImage", RenderEngine_LoadImage)
			WRAP_METHOD(FreeImage)
			LAMBDA_METHOD2("DrawImage", RenderEngine_DrawImage)
			LAMBDA_METHOD2("DrawImage2", RenderEngine_DrawImage2)
			WRAP_METHOD(DrawColor)
			WRAP_METHOD(DrawGradient)
			WRAP_METHOD(DrawLine)
			WRAP_METHOD(DrawRect)
			WRAP_METHOD(DrawRoundRect)
			WRAP_METHOD(DrawText)
			LAMBDA_METHOD2("DrawHtmlText", RenderEngine_DrawHtmlText)
			LAMBDA_METHOD2("GenerateBitmap", RenderEngine_GenerateBitmap)
			WRAP_METHOD(GetTextSize);
#undef WRAP_METHOD

		//CPaintManagerUI
#define WRAP_METHOD(m) .def(#m, &CPaintManagerUI::m)
		lua::lua_register_t<CPaintManagerUI>(l, "DuiLib.CPaintManagerUI")
			.def(lua::constructor<>())
			.def(lua::destructor())
			WRAP_METHOD(Init)
			WRAP_METHOD(NeedUpdate)
			WRAP_METHOD(IsUpdateNeeded)
			LAMBDA_METHOD2("Invalidate", PaintManager_Invalidate)
			WRAP_METHOD(GetName)
			WRAP_METHOD(GetPaintDC)
			WRAP_METHOD(GetPaintOffscreenBitmap)
			WRAP_METHOD(GetPaintWindow)
			WRAP_METHOD(GetTooltipWindow)
			WRAP_METHOD(GetTooltipWindowWidth)
			WRAP_METHOD(SetTooltipWindowWidth)
			WRAP_METHOD(GetHoverTime)
			WRAP_METHOD(SetHoverTime)
			WRAP_METHOD(GetMousePos)
			WRAP_METHOD(GetClientSize)
			WRAP_METHOD(GetInitSize)
			WRAP_METHOD(SetInitSize)
			WRAP_METHOD(GetSizeBox)
			WRAP_METHOD(SetSizeBox)
			WRAP_METHOD(GetCaptionRect)
			WRAP_METHOD(SetCaptionRect)
			WRAP_METHOD(GetRoundCorner)
			WRAP_METHOD(SetRoundCorner)
			WRAP_METHOD(GetMinInfo)
			WRAP_METHOD(SetMinInfo)
			WRAP_METHOD(GetMaxInfo)
			WRAP_METHOD(SetMaxInfo)
			WRAP_METHOD(IsShowUpdateRect)
			WRAP_METHOD(SetShowUpdateRect)
			WRAP_METHOD(IsNoActivate)
			WRAP_METHOD(SetNoActivate)
			WRAP_METHOD(GetOpacity)
			WRAP_METHOD(SetOpacity)
			WRAP_METHOD(IsLayered)
			WRAP_METHOD(SetLayered)
			WRAP_METHOD(GetLayeredInset)
			WRAP_METHOD(SetLayeredInset)
			WRAP_METHOD(GetLayeredOpacity)
			WRAP_METHOD(SetLayeredOpacity)
			WRAP_METHOD(GetLayeredImage)
			WRAP_METHOD(SetLayeredImage)
			WRAP_METHOD(GetInstance)
			WRAP_METHOD(GetInstancePath)
			WRAP_METHOD(GetCurrentPath)
			WRAP_METHOD(GetResourceDll)
			WRAP_METHOD(GetResourcePath)
			WRAP_METHOD(GetResourceZip)
			WRAP_METHOD(IsCachedResourceZip)
			WRAP_METHOD(GetResourceZipHandle)
			WRAP_METHOD(SetInstance)
			WRAP_METHOD(SetCurrentPath)
			WRAP_METHOD(SetResourceDll)
			WRAP_METHOD(SetResourcePath)
			LAMBDA_METHOD2("SetResourceZip", PaintManager_SetResourceZip)
			WRAP_METHOD(GetResourceType)
			WRAP_METHOD(SetResourceType)
			WRAP_METHOD(GetResourceID)
			WRAP_METHOD(SetResourceID)
			WRAP_METHOD(SetHSL)
			LAMBDA_METHOD2("GetHSL", PaintManager_GetHSL)
			WRAP_METHOD(ReloadSkin)
			WRAP_METHOD(GetPaintManager)
			//WRAP_METHOD(GetPaintManagers)
			WRAP_METHOD(LoadPlugin)
			//WRAP_METHOD(GetPlugins)
			WRAP_METHOD(IsForceUseSharedRes)
			WRAP_METHOD(SetForceUseSharedRes)		
			WRAP_METHOD(GetDefaultDisabledColor)
			WRAP_METHOD(SetDefaultDisabledColor)
			WRAP_METHOD(GetDefaultFontColor)
			WRAP_METHOD(SetDefaultFontColor)
			WRAP_METHOD(GetDefaultLinkFontColor)
			WRAP_METHOD(SetDefaultLinkFontColor)
			WRAP_METHOD(GetDefaultLinkHoverFontColor)
			WRAP_METHOD(SetDefaultLinkHoverFontColor)
			WRAP_METHOD(GetDefaultSelectedBkColor)
			WRAP_METHOD(SetDefaultSelectedBkColor)			
			WRAP_METHOD(GetDefaultFontInfo)
			WRAP_METHOD(SetDefaultFont)
			WRAP_METHOD(GetCustomFontCount)
			WRAP_METHOD(AddFont)
			LAMBDA_METHOD2("GetFont", PaintManager_GetFont)
			LAMBDA_METHOD2("GetFontIndex", PaintManager_GetFontIndex)
			LAMBDA_METHOD2("RemoveFont", PaintManager_RemoveFont)
			LAMBDA_METHOD2("GetFontInfo", PaintManager_GetFontInfo)
			LAMBDA_METHOD2("AddImage", PaintManager_AddImage)
			WRAP_METHOD(RemoveAllFonts)
			WRAP_METHOD(GetImage)
			WRAP_METHOD(GetImageEx)
			WRAP_METHOD(RemoveImage)
			WRAP_METHOD(RemoveAllImages)
			WRAP_METHOD(ReloadSharedImages)
			WRAP_METHOD(ReloadImages)
			//WRAP_METHOD(AddStyleAttributeList)
			WRAP_METHOD(GetStyleAttribute)
			WRAP_METHOD(RemoveStyleAttributeList)
			WRAP_METHOD(RemoveAllStyleAttributeList)	
			WRAP_METHOD(AddDefaultAttributeList)
			WRAP_METHOD(GetDefaultAttributeList)
			WRAP_METHOD(RemoveDefaultAttributeList)
			WRAP_METHOD(RemoveAllDefaultAttributeList)
			WRAP_METHOD(AddWindowCustomAttribute)
			WRAP_METHOD(GetWindowCustomAttribute)
			WRAP_METHOD(RemoveWindowCustomAttribute)
			WRAP_METHOD(RemoveAllWindowCustomAttribute)
			WRAP_METHOD(GetWindowAttribute)
			WRAP_METHOD(SetWindowAttribute)
			WRAP_METHOD(GetWindowAttributeList)
			WRAP_METHOD(SetWindowAttributeList)
			WRAP_METHOD(RemoveWindowAttribute)
			WRAP_METHOD(GetWindowXML)
			WRAP_METHOD(AddMultiLanguageString)
			WRAP_METHOD(GetMultiLanguageString)
			WRAP_METHOD(RemoveMultiLanguageString)
			WRAP_METHOD(RemoveAllMultiLanguageString)
			LAMBDA_METHOD2("ProcessMultiLanguageTokens", PaintManager_ProcessMultiLanguageTokens)
			WRAP_METHOD(AttachDialog)
			WRAP_METHOD(InitControls)
			WRAP_METHOD(RenameControl)
			WRAP_METHOD(ReapObjects)
			WRAP_METHOD(AddOptionGroup)
			WRAP_METHOD(GetOptionGroup)//
			WRAP_METHOD(RemoveOptionGroup)
			WRAP_METHOD(RemoveAllOptionGroups)
			WRAP_METHOD(GetFocus)
			WRAP_METHOD(SetFocus)
			WRAP_METHOD(SetFocusNeeded)
			WRAP_METHOD(SetNextTabControl)
			WRAP_METHOD(SetTimer)
			LAMBDA_METHOD2("KillTimer", PaintManager_KillTimer)
			WRAP_METHOD(RemoveAllTimers)
			WRAP_METHOD(SetCapture)
			WRAP_METHOD(ReleaseCapture)
			WRAP_METHOD(IsCaptured)
			WRAP_METHOD(IsPainting)
			WRAP_METHOD(SetPainting)			
			WRAP_METHOD(AddNotifier)
			WRAP_METHOD(RemoveNotifier)
			LAMBDA_METHOD2("SendNotify", PaintManager_SendNotify)
			WRAP_METHOD(AddPreMessageFilter)
			WRAP_METHOD(RemovePreMessageFilter)
			WRAP_METHOD(AddMessageFilter)
			WRAP_METHOD(RemoveMessageFilter)			
			WRAP_METHOD(GetPostPaintCount)
			WRAP_METHOD(AddPostPaint)
			WRAP_METHOD(RemovePostPaint)
			WRAP_METHOD(SetPostPaintIndex)
			WRAP_METHOD(GetNativeWindowCount)
			WRAP_METHOD(GetNativeWindowRect)
			WRAP_METHOD(AddNativeWindow)
			WRAP_METHOD(RemoveNativeWindow)
			WRAP_METHOD(AddDelayedCleanup)
			WRAP_METHOD(AddMouseLeaveNeeded)
			WRAP_METHOD(RemoveMouseLeaveNeeded)				
			WRAP_METHOD(GetRoot)
			LAMBDA_METHOD2("FindControl", PaintManager_FindControl)
			WRAP_METHOD(FindSubControlByPoint)
			WRAP_METHOD(FindSubControlByName)
			WRAP_METHOD(FindSubControlByClass)
			WRAP_METHOD(FindSubControlsByClass)
			WRAP_METHOD(MessageLoop)
			WRAP_METHOD(Term)
			WRAP_METHOD(UsedVirtualWnd)
			WRAP_METHOD(PlaySound);
#undef WRAP_METHOD

//			//CNotifyPump
//#define WRAP_METHOD(m) .def(#m, &CNotifyPump::m))
//			lua::lua_register_t<CNotifyPump>(l, "DuiLib.CNotifyPump")
//				WRAP_METHOD(AddVirtualWnd)
//				WRAP_METHOD(RemoveVirtualWnd)
//				WRAP_METHOD(NotifyPump)
//				WRAP_METHOD(LoopDispatch);
//#undef WRAP_METHOD

			//CWindowWnd
#define WRAP_METHOD(m) .def(#m, &CWindowWnd::m)
			lua::lua_register_t<CWindowWnd>(l, "DuiLib.CWindowWnd")
				WRAP_METHOD(GetHWND)
				WRAP_METHOD(RegisterWindowClass)
				WRAP_METHOD(RegisterSuperclass)
				LAMBDA_METHOD2("Create", WindowWnd_Create)
				WRAP_METHOD(CreateDuiWindow)
				WRAP_METHOD(Subclass)
				WRAP_METHOD(Unsubclass)
				WRAP_METHOD(ShowWindow)
				WRAP_METHOD(ShowModal)
				WRAP_METHOD(Close)
				WRAP_METHOD(CenterWindow)
				WRAP_METHOD(SetIcon)
				LAMBDA_METHOD2("SetIconFromFile", WindowWnd_SetIconFromFile)
				LAMBDA_METHOD2("SendMessage", WindowWnd_SendMessage)
				LAMBDA_METHOD2("PostMessage", WindowWnd_PostMessage)
				WRAP_METHOD(ResizeClient);
#undef WRAP_METHOD

			//CWin
#define WRAP_METHOD(m) .def(#m, &CWin::m)
			lua::lua_register_t<CWin>(l, "DuiLib.CWin")
				.extend<CWindowWnd>()
				.def(lua::destructor())
				WRAP_METHOD(GetWindowClassName)
				WRAP_METHOD(GetClassStyle)
				WRAP_METHOD(GetExStyle)
				WRAP_METHOD(ModifyStyle)
				WRAP_METHOD(ModifyStyleEx)
				LAMBDA_METHOD2("MoveWindow", CWin_MoveWindow)
				LAMBDA_METHOD2("SetWindowPos", CWin_SetWindowPos)
				WRAP_METHOD(DestroyWindow)
				WRAP_METHOD(MoveWindowToPoint)
				WRAP_METHOD(ArrangeIconicWindows)
				WRAP_METHOD(ForegroundWindow)
				WRAP_METHOD(BringWindowToTop)
				LAMBDA_METHOD2("GetWindowRect", CWin_GetWindowRect)
				LAMBDA_METHOD2("GetClientRect", CWin_GetClientRect)
				LAMBDA_METHOD2("ClientToScreen", CWin_ClientToScreen)
				LAMBDA_METHOD2("ScreenToClient", CWin_ScreenToClient)
				WRAP_METHOD(UpdateWindow)
				WRAP_METHOD(Invalidate)
				WRAP_METHOD(InvalidateRect)
				WRAP_METHOD(ValidateRect)
				WRAP_METHOD(InvalidateRgn)
				WRAP_METHOD(ValidateRgn)
				WRAP_METHOD(IsWindow)
				WRAP_METHOD(IsIconic)
				WRAP_METHOD(IsZoomed)
				WRAP_METHOD(IsWindowEnabled)
				WRAP_METHOD(EnableWindow)
				WRAP_METHOD(IsWindowVisible)
				WRAP_METHOD(IsWindowUnicode)
				WRAP_METHOD(IsParentDialog)
				WRAP_METHOD(SetTimer)
				WRAP_METHOD(KillTimer)
				WRAP_METHOD(SetActiveWindow)
				WRAP_METHOD(SetCapture)
				WRAP_METHOD(SetFocus)
				WRAP_METHOD(IsChild)
				WRAP_METHOD(GetTopLevelWindow)		
				WRAP_METHOD(GetTopLevelParent)
				WRAP_METHOD(GetParent)
				WRAP_METHOD(SetParent)
				WRAP_METHOD(FlashWindow)
				WRAP_METHOD(AnimateWindow)
				WRAP_METHOD(FadeInOut)
				WRAP_METHOD(ShakeWindow)
				LAMBDA_METHOD2("FindControl", CWin_FindControl)
				WRAP_METHOD(SetHotKey)
				WRAP_METHOD(GetHotKey)
				WRAP_METHOD(IsMainThread)
				WRAP_METHOD(GetWindowThreadID)
				WRAP_METHOD(GetWindowProcessID)
				WRAP_METHOD(GetFont)
				WRAP_METHOD(SetFont)
				WRAP_METHOD(SetWindowText)
				WRAP_METHOD(GetWindowTextLength)
				LAMBDA_METHOD2("GetWindowText", CWin_GetWindowText)
				WRAP_METHOD(UnRegisterSkin)
				WRAP_METHOD(GetTag)
				WRAP_METHOD(SetTag)
				WRAP_METHOD(IsValid)
				LAMBDA_METHOD2("MsgBox", CWin_MsgBox);
#undef WRAP_METHOD

			//CLuaWindow
#define WRAP_METHOD(m) .def(#m, &CLuaWindow::m)
			lua::lua_register_t<CLuaWindow>(l, "DuiLib.CLuaWindow")
				.extend<CWin>()
				.def(lua::destructor())
				LAMBDA_METHOD2("New", LuaWindow_New)
				WRAP_METHOD(PaintMgr)
				LAMBDA_METHOD2("__tostring", &CLuaWindow::ToString)
				LAMBDA_METHOD2("RegisterSkin", LuaWindow_RegisterSkin)
				LAMBDA_METHOD2("BindLuaScript", LuaWindow_BindLuaScript)
				LAMBDA_METHOD2("ListenUIEvent", LuaWindow_ListenUIEvent)
				LAMBDA_METHOD2("ListenUIDestroy", LuaWindow_ListenUIDestroy)
				LAMBDA_METHOD2("AddPreMessageFilter", LuaWindow_AddPreMessageFilter)
				LAMBDA_METHOD2("AddMessageFilter", LuaWindow_AddMessageFilter)
				LAMBDA_METHOD2("AddNotifier", LuaWindow_AddNotifier)
				LAMBDA_METHOD2("SetListCallback", LuaWindow_SetListCallback);
#undef WRAP_METHOD

			//CResourceManager
			lua::lua_register_t<void>(l, "DuiLib.CResourceManager")
				LAMBDA_METHOD2("FindAsset", ResourceManager_FindAsset)
				LAMBDA_METHOD2("LoadAsset", ResourceManager_LoadAsset);

			//CFileDialog
#define WRAP_METHOD(m) .def(#m, &CFileDialog::m)
			lua::lua_register_t<CFileDialog>(l, "DuiLib.CFileDialog")
				.def(lua::constructor<const CDuiString>())
				.def(lua::destructor())
				WRAP_METHOD(SetDefExt)
				WRAP_METHOD(SetFileName)
				WRAP_METHOD(SetFlags)
				LAMBDA_METHOD2("SetFilter", CFileDialog_SetFilter)
				WRAP_METHOD(SetMultiSel)
				WRAP_METHOD(SetParentWnd)
				WRAP_METHOD(SetTitle)
				WRAP_METHOD(ShowOpenFileDlg)
				WRAP_METHOD(ShowSaveFileDlg)
				WRAP_METHOD(GetPathName)
				WRAP_METHOD(GetFileName)
				WRAP_METHOD(GetFileExt)
				WRAP_METHOD(GetFileTitle)
				WRAP_METHOD(GetFolderPath)
				LAMBDA_METHOD2("__tostring", &CFileDialog::ToString)
				LAMBDA_METHOD2("GetAllFileList", CFileDialog_GetAllFileList);
#undef WRAP_METHOD

			//CTrayIconUI
			lua::lua_register_t<void>(l, "DuiLib.CTrayIconUI")
				LAMBDA_METHOD2("CreateTrayIcon", CTrayIconUI_CreateTrayIcon)
				LAMBDA_METHOD2("CreateTrayIconFromFile", CTrayIconUI_CreateTrayIconFromFile)
				LAMBDA_METHOD2("CreateTrayIconFromIcon", CTrayIconUI_CreateTrayIconFromIcon)
				LAMBDA_METHOD2("DeleteTrayIcon", CTrayIconUI_DeleteTrayIcon)
				LAMBDA_METHOD2("SetTooltipText", CTrayIconUI_SetTooltipText)
				LAMBDA_METHOD2("GetTooltipText", CTrayIconUI_GetTooltipText)
				LAMBDA_METHOD2("ShowInfotip", CTrayIconUI_ShowInfotip)
				LAMBDA_METHOD2("SetIcon", CTrayIconUI_SetIcon)
				LAMBDA_METHOD2("GetIcon", CTrayIconUI_GetIcon)
				LAMBDA_METHOD2("SetHideIcon", CTrayIconUI_SetHideIcon)
				LAMBDA_METHOD2("SetShowIcon", CTrayIconUI_SetShowIcon)
				LAMBDA_METHOD2("RemoveIcon", CTrayIconUI_RemoveIcon)
				LAMBDA_METHOD2("StartTwinkling", CTrayIconUI_StartTwinkling)
				LAMBDA_METHOD2("StopTwinkling", CTrayIconUI_StopTwinkling)
				LAMBDA_METHOD2("IsEnabled", CTrayIconUI_Enabled)
				LAMBDA_METHOD2("IsVisible", CTrayIconUI_IsVisible)
				LAMBDA_METHOD2("IsTwinkling", CTrayIconUI_IsTwinkling)
				LAMBDA_METHOD2("IsMouseHover", CTrayIconUI_IsMouseHover);

			//CMenuElementUI
#define WRAP_METHOD(m) .def(#m, &CMenuElementUI::m)
			lua::lua_register_t<CMenuElementUI>(l, "DuiLib.CMenuElementUI")
				.extend<CListContainerElementUI>()
				.def(lua::constructor<>())
				.def("New", NewControl<CMenuElementUI>)
				.def("__tostring", CControlUI_ToString)
				WRAP_METHOD(Activate);
#undef WRAP_METHOD

			//CMenuUI
#define WRAP_METHOD(m) .def(#m, &CMenuUI::m)
			lua::lua_register_t<CMenuUI>(l, "DuiLib.CMenuUI")
				.extend<CListUI>()
				.def(lua::constructor<>())
				.def("New", NewControl<CMenuUI>)
				.def("__tostring", CControlUI_ToString)
				WRAP_METHOD(Remove);
#undef WRAP_METHOD

			//CMenuUI
#define WRAP_METHOD(m) .def(#m, &CMenuWnd::m)
			lua::lua_register_t<CMenuWnd>(l, "DuiLib.CMenuWnd")
				.extend<CWindowWnd>()
				.def(lua::destructor())
				LAMBDA_METHOD2("New", CMenuWnd_New)
				LAMBDA_METHOD2("Init", CMenuWnd_Init)
				WRAP_METHOD(SetTag)
				WRAP_METHOD(GetTag)
				WRAP_METHOD(SetUserData)
				WRAP_METHOD(GetUserData)
				WRAP_METHOD(GetPaintManager)
				WRAP_METHOD(IsEnableItem)
				WRAP_METHOD(EnableItem);
#undef WRAP_METHOD

			//MenuCmdMsg
#define WRAP_METHOD(m) .def(#m, &MenuCmdMsg::m)
			lua::lua_register_t<MenuCmdMsg>(l, "DuiLib.MenuCmdMsg")
				WRAP_METHOD(nMenuTag)
				WRAP_METHOD(nMenuUserData)
				WRAP_METHOD(nItemTag)
				WRAP_METHOD(sItemText)
				WRAP_METHOD(sItemName);
#undef WRAP_METHOD

	}
}

#undef LAMBDA_METHOD
#undef LAMBDA_METHOD2

namespace DuiLib
{
	bool register_lua_scripts(lua_State* l)
	{
		RegisterDuiLibControlToLua(l);
		RegisterDuiLibCoreToLua(l);

		return true;
	}
}


