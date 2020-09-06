#ifndef lua_duilib_wrapper_hpp
#define lua_duilib_wrapper_hpp
#include "lua_op_define.hpp"
#include "base/lua_script.hpp"

namespace lua
{
	template<>
	struct lua_op_t<wchar_t>
	{
		static int push_stack(lua_State* L, wchar_t value)
		{
			lua_pushnumber(L, (lua_Number)value);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, wchar_t* value)
		{
			if (lua_isnil(L, pos))
			{
				return;
			}
			LUA_CHECK_PARAM(lua_isnumber(L, pos), type::number, pos);
			*value = (char)luaL_checknumber(L, pos);
		}
		static bool try_get(lua_State * L, int pos, wchar_t* value)
		{
			if (lua_isnumber(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t<const wchar_t*>
	{
		static int push_stack(lua_State* L, const wchar_t* value)
		{
			if (value)
			{
				std::string s = DuiLib::Convert::UnicodeToUTF8(value);
				lua_pushstring(L, s.c_str());
			}
			else
				lua_pushnil(L);
			
			return 1;
		}
		static void from_stack(lua_State* L, int pos, wchar_t** value)
		{
			if (lua_isnil(L, pos))
			{
				*value = 0;
				return;
			}
			LUA_CHECK_PARAM(lua_isstring(L, pos), type::string, pos);
			const char* s = luaL_checkstring(L, pos);
			std::wstring suni = DuiLib::Convert::UTF8ToUnicode(s);
			wcscpy(*value, suni.c_str());
		}
		static bool try_get(lua_State * L, int pos, wchar_t** value)
		{
			if (lua_isnil(L, pos) || lua_isstring(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t<wchar_t*>
	{
		static int push_stack(lua_State* L, wchar_t* value)
		{
			return lua_op_t<const wchar_t*>::push_stack(L, value);
		}
		static void from_stack(lua_State* L, int pos, wchar_t** value)
		{
			lua_op_t<const wchar_t*>::from_stack(L, pos, value);
		}
		static bool try_get(lua_State * L, int pos, wchar_t** value)
		{
			return lua_op_t<const wchar_t*>::try_get(L, pos, value);
		}
	};

	template<int N>
	struct lua_op_t<const wchar_t[N]>
	{
		static int push_stack(lua_State* L, const wchar_t* value)
		{
			return lua_op_t<const wchar_t*>::push_stack(L, value);
		}
		static void from_stack(lua_State* L, int pos, const wchar_t** value)
		{
			lua_op_t<const wchar_t*>::from_stack(L, pos, value);
		}
		static bool try_get(lua_State * L, int pos, const wchar_t** value)
		{
			return lua_op_t<const wchar_t*>::try_get(L, pos, value);
		}
	};

	template<int N>
	struct lua_op_t<wchar_t[N]>
	{
		static int push_stack(lua_State* L, const wchar_t* value)
		{
			return lua_op_t<const wchar_t*>::push_stack(L, value);
		}
		static void from_stack(lua_State* L, int pos, const wchar_t** value)
		{
			lua_op_t<const wchar_t*>::from_stack(L, pos, value);
		}
		static bool try_get(lua_State * L, int pos, const wchar_t** value)
		{
			return lua_op_t<const wchar_t*>::try_get(L, pos, value);
		}
	};

	template<>
	struct lua_op_t<DuiLib::UILIB_RESOURCETYPE>
	{
		static int push_stack(lua_State* L, const DuiLib::UILIB_RESOURCETYPE& arg_)
		{
			lua_pushinteger(L, (int)arg_);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, DuiLib::UILIB_RESOURCETYPE* value)
		{
			LUA_CHECK_PARAM(lua_isnumber(L, pos), type::number, pos);
			int v = luaL_checkinteger(L, pos);
			*value = (DuiLib::UILIB_RESOURCETYPE)v;
		}
		static bool try_get(lua_State * L, int pos, DuiLib::UILIB_RESOURCETYPE* value)
		{
			if (lua_isnumber(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t <DuiLib::CDuiString >
	{
		static int push_stack(lua_State* L, const DuiLib::CDuiString& arg_)
		{
			std::string s = DuiLib::Convert::ToUTF8(arg_.GetData());
			lua_pushstring(L, s.c_str());
			return 1;
		}
		static void from_stack(lua_State* L, int pos, DuiLib::CDuiString* value)
		{
			LUA_CHECK_PARAM(lua_isstring(L, pos), type::string, pos);
			size_t len = 0;
			const char* src = lua_tolstring(L, pos, &len);
#ifdef UNICODE
			std::wstring puni = DuiLib::Convert::UTF8ToUnicode(src, len);
			value->Assign((LPCTSTR)puni.c_str(), puni.size());
#else
			std::string pmb = DuiLib::Convert::UTF8ToMB(src, len);
			value->Assign((LPCTSTR)pmb.c_str(), pmb.size());
#endif
		}
		static bool try_get(lua_State * L, int pos, DuiLib::CDuiString* value)
		{
			if (lua_isstring(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t <const DuiLib::CDuiString >
	{
		static int push_stack(lua_State* L, const DuiLib::CDuiString& arg_)
		{
			return lua_op_t <DuiLib::CDuiString >::push_stack(L, arg_);
		}
		static void from_stack(lua_State* L, int pos, DuiLib::CDuiString* value)
		{
			lua_op_t <DuiLib::CDuiString >::from_stack(L, pos, value);
		}

		static bool try_get(lua_State * L, int pos, DuiLib::CDuiString* value)
		{
			return lua_op_t <DuiLib::CDuiString >::try_get(L, pos, value);
		}
	};

	template<>
	struct lua_op_t < DuiLib::CDuiString& >
	{
		static int push_stack(lua_State* L, DuiLib::CDuiString& arg_)
		{
			return lua_op_t<DuiLib::CDuiString>::push_stack(L, arg_);
		}
		static void from_stack(lua_State* L, int pos, DuiLib::CDuiString* value)
		{
			lua_op_t<DuiLib::CDuiString>::from_stack(L, pos, value);
		}

		static bool try_get(lua_State * L, int pos, DuiLib::CDuiString* value)
		{
			return lua_op_t<DuiLib::CDuiString>::try_get(L, pos, value);
		}
	};

	template<>
	struct lua_op_t <const DuiLib::CDuiString& >
	{
		static int push_stack(lua_State* L, const DuiLib::CDuiString& arg_)
		{
			return lua_op_t<DuiLib::CDuiString>::push_stack(L, arg_);
		}
		static void from_stack(lua_State* L, int pos, DuiLib::CDuiString* value)
		{
			lua_op_t<DuiLib::CDuiString>::from_stack(L, pos, value);
		}
		static bool try_get(lua_State * L, int pos, DuiLib::CDuiString* value)
		{
			return lua_op_t<DuiLib::CDuiString>::try_get(L, pos, value);
		}
	};

	template<>
	struct lua_op_t < POINT >
	{
		static int push_stack(lua_State* L, const POINT& arg_)
		{
			table t(L, (const char*)NULL);
			t.set("x", arg_.x);
			t.set("y", arg_.y);
			lua::push(L, t);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, POINT* value)
		{
			LUA_CHECK_PARAM(lua_istable(L, pos) != 0, type::table, pos);
			table t;
			lua::get(L, pos, &t);
			int x, y;
			t.get("x", &x);
			t.get("y", &y);
			value->x = x;
			value->y = y;
		}
		static bool try_get(lua_State * L, int pos, POINT* value)
		{
			if (lua_istable(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t < DuiLib::CDuiPoint >
	{
		static int push_stack(lua_State* L, const DuiLib::CDuiPoint& arg_)
		{
			table t(L, (char*)NULL);
			t.set("x", arg_.x);
			t.set("y", arg_.y);
			lua::push(L, t);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, DuiLib::CDuiPoint* value)
		{
			LUA_CHECK_PARAM(lua_istable(L, pos) != 0, type::table, pos);
			table t;
			lua::get(L, pos, &t);
			int x, y;
			t.get("x", &x);
			t.get("y", &y);
			value->x = x;
			value->y = y;
		}
		static bool try_get(lua_State * L, int pos, DuiLib::CDuiPoint* value)
		{
			if (lua_istable(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t < DuiLib::CDuiPoint& >
	{
		static int push_stack(lua_State* L, const DuiLib::CDuiPoint& arg_)
		{
			return lua_op_t<DuiLib::CDuiPoint>::push_stack(L, arg_);
		}
		static void from_stack(lua_State* L, int pos, DuiLib::CDuiPoint* value)
		{
			lua_op_t<DuiLib::CDuiPoint>::from_stack(L, pos, value);
		}
		static bool try_get(lua_State * L, int pos, DuiLib::CDuiPoint* value)
		{
			return lua_op_t<DuiLib::CDuiPoint>::try_get(L, pos, value);
		}
	};

	template<>
	struct lua_op_t < SIZE >
	{
		static int push_stack(lua_State* L, const SIZE& arg_)
		{
			table t(L, (char*)NULL);
			t.set("cx", arg_.cx);
			t.set("cy", arg_.cy);
			lua::push(L, t);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, SIZE* value)
		{
			LUA_CHECK_PARAM(lua_istable(L, pos) != 0, type::table, pos);
			table t;
			lua::get(L, pos, &t);
			int cx, cy;
			t.get("cx", &cx);
			t.get("cy", &cy);
			value->cx = cx;
			value->cy = cy;
		}
		static bool try_get(lua_State * L, int pos, SIZE* value)
		{
			if (lua_istable(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t < DuiLib::CDuiSize >
	{
		static int push_stack(lua_State* L, const DuiLib::CDuiSize& arg_)
		{
			table t(L, (char*)NULL);
			t.set("cx", arg_.cx);
			t.set("cy", arg_.cy);
			lua::push(L, t);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, DuiLib::CDuiSize* value)
		{
			LUA_CHECK_PARAM(lua_istable(L, pos) != 0, type::table, pos);
			table t;
			lua::get(L, pos, &t);
			int cx, cy;
			t.get("cx", &cx);
			t.get("cy", &cy);
			value->cx = cx;
			value->cy = cy;
		}
		static bool try_get(lua_State * L, int pos, DuiLib::CDuiSize* value)
		{
			if (lua_istable(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t < DuiLib::CDuiSize& >
	{
		static int push_stack(lua_State* L, const DuiLib::CDuiSize& arg_)
		{
			return lua_op_t<DuiLib::CDuiSize>::push_stack(L, arg_);
		}
		static void from_stack(lua_State* L, int pos, DuiLib::CDuiSize* value)
		{
			lua_op_t<DuiLib::CDuiSize>::from_stack(L, pos, value);
		}
		static bool try_get(lua_State * L, int pos, DuiLib::CDuiSize* value)
		{
			return lua_op_t<DuiLib::CDuiSize>::try_get(L, pos, value);
		}
	};

	template<>
	struct lua_op_t < RECT >
	{
		static int push_stack(lua_State* L, const RECT& arg_)
		{
			table t(L, (char*)NULL);
			t.set("left", arg_.left);
			t.set("top", arg_.top);
			t.set("right", arg_.right);
			t.set("bottom", arg_.bottom);
			lua::push(L, t);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, RECT* value)
		{
			LUA_CHECK_PARAM(lua_istable(L, pos) != 0, type::table, pos);
			table t;
			lua::get(L, pos, &t);
			int left, top, right, bottom;
			t.get("left", &left);
			t.get("top", &top);
			t.get("right", &right);
			t.get("bottom", &bottom);
			value->left = left;
			value->top = top;
			value->right = right;
			value->bottom = bottom;
		}
		static bool try_get(lua_State * L, int pos, RECT* value)
		{
			if (lua_istable(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t < DuiLib::CDuiRect >
	{
		static int push_stack(lua_State* L, const DuiLib::CDuiRect& arg_)
		{
			table t(L, (char*)NULL);
			t.set("left", arg_.left);
			t.set("top", arg_.top);
			t.set("right", arg_.right);
			t.set("bottom", arg_.bottom);
			lua::push(L, t);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, DuiLib::CDuiRect* value)
		{
			LUA_CHECK_PARAM(lua_istable(L, pos) != 0, type::table, pos);
			table t;
			lua::get(L, pos, &t);
			int left, top, right, bottom;
			t.get("left", &left);
			t.get("top", &top);
			t.get("right", &right);
			t.get("bottom", &bottom);
			value->left = left;
			value->top = top;
			value->right = right;
			value->bottom = bottom;
		}
		static bool try_get(lua_State * L, int pos, DuiLib::CDuiRect* value)
		{
			if (lua_istable(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t < DuiLib::CDuiRect& >
	{
		static int push_stack(lua_State* L, const DuiLib::CDuiRect& arg_)
		{
			return lua_op_t<DuiLib::CDuiRect>::push_stack(L, arg_);
		}
		static void from_stack(lua_State* L, int pos, DuiLib::CDuiRect* value)
		{
			lua_op_t<DuiLib::CDuiRect>::from_stack(L, pos, value);
		}
		static bool try_get(lua_State * L, int pos, DuiLib::CDuiRect* value)
		{
			return lua_op_t<DuiLib::CDuiRect>::try_get(L, pos, value);
		}
	};

	template<>
	struct lua_op_t < DuiLib::CDuiStringPtrMap >
	{
		static int push_stack(lua_State* L, const DuiLib::CDuiStringPtrMap& arg_)
		{
			lua_newtable(L);
			for (int i = 0; i < arg_.GetSize(); ++i)
			{
				DuiLib::CDuiString szKey = arg_[i];
				LPVOID data = arg_.Find(szKey);
				lua::push(L, szKey);
				lua::push(L, data);
				lua_settable(L, -3);
			}
			return 1;
		}
		static void from_stack(lua_State* L, int pos, DuiLib::CDuiStringPtrMap* value)
		{
			if (lua_isnoneornil(L, pos))
			{
				return;
			}
			LUA_CHECK_PARAM(0 != lua_istable(L, pos), type::table, pos);
			lua_pushnil(L);
			int real_pos = pos;
			if (pos < 0) real_pos = real_pos - 1;
			while (lua_next(L, real_pos) != 0)
			{
				DuiLib::CDuiString key;
				LPVOID val;
				lua::get(L, -2, &key);
				lua::get(L, -1, &val);
				value->Set(key, val);
				lua_pop(L, 1);
			}
		}
		static bool try_get(lua_State * L, int pos, DuiLib::CDuiStringPtrMap* value)
		{
			if (!lua_istable(L, pos)) return false;
			lua_op_t<DuiLib::CDuiStringPtrMap>::from_stack(L, pos, value);
			return true;
		}
	};

	template<>
	struct lua_op_t < DuiLib::CDuiStringPtrMap* >
	{
		static int push_stack(lua_State* L, const DuiLib::CDuiStringPtrMap* arg_)
		{
			return lua_op_t<DuiLib::CDuiStringPtrMap>::push_stack(L, *arg_);
		}
		static void from_stack(lua_State* L, int pos, DuiLib::CDuiStringPtrMap** value)
		{
		}
		static bool try_get(lua_State * L, int pos, DuiLib::CDuiStringPtrMap** value)
		{
			if (!lua_istable(L, pos)) return false;
			return true;
		}
	};
	
	template<>
	struct lua_op_t < DuiLib::CDuiPtrArray >
	{
		static int push_stack(lua_State* L, const DuiLib::CDuiPtrArray& arg_)
		{
			lua_newtable(L);
			for (int i=0; i < arg_.GetSize(); ++i)
			{
				LPVOID data = arg_.GetAt(i);
				lua::push(L, i+1);
				lua::push(L, data);
				lua_settable(L, -3);
			}
			return 1;
		}
		static void from_stack(lua_State* L, int pos, DuiLib::CDuiPtrArray* value)
		{
		}
		static bool try_get(lua_State * L, int pos, DuiLib::CDuiPtrArray* value)
		{
			if (!lua_istable(L, pos)) return false;
			lua_op_t<DuiLib::CDuiPtrArray>::from_stack(L, pos, value);
			return true;
		}
	};
	
}

LUA_DUI_CLASS_OP_T(DuiLib::CControlUI)
LUA_DUI_CLASS_OP_T(DuiLib::CContainerUI)
LUA_DUI_CLASS_OP_T(DuiLib::CActiveXUI)
LUA_DUI_CLASS_OP_T(DuiLib::CButtonUI)
LUA_DUI_CLASS_OP_T(DuiLib::CCheckBoxUI)
LUA_DUI_CLASS_OP_T(DuiLib::CComboUI)
LUA_DUI_CLASS_OP_T(DuiLib::CDateTimeUI)
LUA_DUI_CLASS_OP_T(DuiLib::CEditUI)
LUA_DUI_CLASS_OP_T(DuiLib::CFlashUI)
LUA_DUI_CLASS_OP_T(DuiLib::CGifAnimUI)
LUA_DUI_CLASS_OP_T(DuiLib::CHotKeyUI)
LUA_DUI_CLASS_OP_T(DuiLib::CLabelUI)
LUA_DUI_CLASS_OP_T(DuiLib::CListUI)
LUA_DUI_CLASS_OP_T(DuiLib::CListBodyUI)
LUA_DUI_CLASS_OP_T(DuiLib::CListHeaderUI)
LUA_DUI_CLASS_OP_T(DuiLib::CListHeaderItemUI)
LUA_DUI_CLASS_OP_T(DuiLib::CListElementUI)
LUA_DUI_CLASS_OP_T(DuiLib::CListLabelElementUI)
LUA_DUI_CLASS_OP_T(DuiLib::CListTextElementUI)
LUA_DUI_CLASS_OP_T(DuiLib::CListContainerElementUI)
LUA_DUI_CLASS_OP_T(DuiLib::CListHBoxElementUI)
LUA_DUI_CLASS_OP_T(DuiLib::COptionUI)
LUA_DUI_CLASS_OP_T(DuiLib::CProgressUI)
LUA_DUI_CLASS_OP_T(DuiLib::CRichEditUI)
LUA_DUI_CLASS_OP_T(DuiLib::CScrollBarUI)
LUA_DUI_CLASS_OP_T(DuiLib::CSliderUI)
LUA_DUI_CLASS_OP_T(DuiLib::CTextUI)
LUA_DUI_CLASS_OP_T(DuiLib::CTreeNodeUI)
LUA_DUI_CLASS_OP_T(DuiLib::CTreeViewUI)
LUA_DUI_CLASS_OP_T(DuiLib::CWebBrowserUI)
LUA_DUI_CLASS_OP_T(DuiLib::CChildLayoutUI)
LUA_DUI_CLASS_OP_T(DuiLib::CHorizontalLayoutUI)
LUA_DUI_CLASS_OP_T(DuiLib::CTabLayoutUI)
LUA_DUI_CLASS_OP_T(DuiLib::CTileLayoutUI)
LUA_DUI_CLASS_OP_T(DuiLib::CVerticalLayoutUI)
LUA_DUI_CLASS_OP_T(DuiLib::CMenuElementUI)
LUA_DUI_CLASS_OP_T(DuiLib::CMenuUI)
LUA_DUI_CLASS_OP_T(DuiLib::CMenuWnd)
LUA_DUI_CLASS_OP_T(DuiLib::CWindowWnd)
LUA_DUI_CLASS_OP_T(DuiLib::CDialogBuilder)
LUA_DUI_CLASS_OP_T(DuiLib::CPaintManagerUI)
LUA_DUI_CLASS_OP_T(DuiLib::CMarkup)
LUA_DUI_CLASS_OP_T(DuiLib::CMarkupNode)
LUA_DUI_CLASS_OP_T(DuiLib::CTrayIconUI)
LUA_DUI_CLASS_OP_T(DuiLib::CWin)
LUA_DUI_CLASS_OP_T(DuiLib::CWndShadow)


#endif//lua_duilib_wrapper_hpp