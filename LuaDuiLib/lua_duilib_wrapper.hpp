#ifndef lua_duilib_wrapper_hpp
#define lua_duilib_wrapper_hpp
#include "lua_op_define.hpp"

namespace lua
{
	template<>
	struct lua_op_t<wchar_t>
	{
		static int push_stack(lua_State* l, wchar_t value)
		{
			lua_pushnumber(l, (lua_Number)value);
			return 1;
		}
		static void from_stack(lua_State* l, int pos, wchar_t* value)
		{
			if (lua_isnil(l, pos))
			{
				return;
			}
			LUA_CHECK_ERROR(0 != lua_isnumber(l, pos), LUA_TNUMBER, pos);
			*value = (char)luaL_checknumber(l, pos);
		}
		static bool try_get(lua_State * l, int pos, wchar_t* value)
		{
			if (lua_isnumber(l, pos))
			{
				from_stack(l, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t<const wchar_t*>
	{
		static int push_stack(lua_State* l, const wchar_t* value)
		{
			if (value)
			{
				std::string s = DuiLib::Convert::UnicodeToUTF8(value);
				lua_pushstring(l, s.c_str());
			}
			else
				lua_pushnil(l);
			
			return 1;
		}
		static void from_stack(lua_State* l, int pos, wchar_t** value)
		{
			if (lua_isnil(l, pos))
			{
				*value = 0;
				return;
			}
			LUA_CHECK_ERROR(0 != lua_isstring(l, pos), LUA_TSTRING, pos);
			const char* s = luaL_checkstring(l, pos);
			std::wstring suni = DuiLib::Convert::UTF8ToUnicode(s);
			wcscpy(*value, suni.c_str());
		}
		static bool try_get(lua_State * l, int pos, wchar_t** value)
		{
			if (lua_isnil(l, pos) || lua_isstring(l, pos))
			{
				from_stack(l, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t<wchar_t*>
	{
		static int push_stack(lua_State* l, wchar_t* value)
		{
			return lua_op_t<const wchar_t*>::push_stack(l, value);
		}
		static void from_stack(lua_State* l, int pos, wchar_t** value)
		{
			lua_op_t<const wchar_t*>::from_stack(l, pos, value);
		}
		static bool try_get(lua_State * l, int pos, wchar_t** value)
		{
			return lua_op_t<const wchar_t*>::try_get(l, pos, value);
		}
	};

	template<int N>
	struct lua_op_t<const wchar_t[N]>
	{
		static int push_stack(lua_State* l, const wchar_t* value)
		{
			return lua_op_t<const wchar_t*>::push_stack(l, value);
		}
		static void from_stack(lua_State* l, int pos, const wchar_t** value)
		{
			lua_op_t<const wchar_t*>::from_stack(l, pos, value);
		}
		static bool try_get(lua_State * l, int pos, const wchar_t** value)
		{
			return lua_op_t<const wchar_t*>::try_get(l, pos, value);
		}
	};

	template<int N>
	struct lua_op_t<wchar_t[N]>
	{
		static int push_stack(lua_State* l, const wchar_t* value)
		{
			return lua_op_t<const wchar_t*>::push_stack(l, value);
		}
		static void from_stack(lua_State* l, int pos, const wchar_t** value)
		{
			lua_op_t<const wchar_t*>::from_stack(l, pos, value);
		}
		static bool try_get(lua_State * l, int pos, const wchar_t** value)
		{
			return lua_op_t<const wchar_t*>::try_get(l, pos, value);
		}
	};

	template<>
	struct lua_op_t<DuiLib::UILIB_RESOURCETYPE>
	{
		static int push_stack(lua_State* l, const DuiLib::UILIB_RESOURCETYPE& arg_)
		{
			lua_pushinteger(l, (int)arg_);
			return 1;
		}
		static void from_stack(lua_State* l, int pos, DuiLib::UILIB_RESOURCETYPE* value)
		{
			LUA_CHECK_ERROR(0 != lua_isnumber(l, pos), LUA_TNUMBER, pos);
			int v = luaL_checkinteger(l, pos);
			*value = (DuiLib::UILIB_RESOURCETYPE)v;
		}
		static bool try_get(lua_State * l, int pos, DuiLib::UILIB_RESOURCETYPE* value)
		{
			if (lua_isnumber(l, pos))
			{
				from_stack(l, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t <DuiLib::CDuiString >
	{
		static int push_stack(lua_State* l, const DuiLib::CDuiString& arg_)
		{
			std::string s = DuiLib::Convert::ToUTF8(arg_.GetData());
//			lua_pushlstring(l, s.c_str(), s.size());
			lua_pushstring(l, s.c_str());
			return 1;
		}
		static void from_stack(lua_State* l, int pos, DuiLib::CDuiString* value)
		{
			LUA_CHECK_ERROR(0 != lua_isstring(l, pos), LUA_TSTRING, pos);
			size_t len = 0;
			const char* src = lua_tolstring(l, pos, &len);
#ifdef UNICODE
			std::wstring puni = DuiLib::Convert::UTF8ToUnicode(src, len);
			value->Assign((LPCTSTR)puni.c_str(), puni.size());
#else
			std::string pmb = DuiLib::Convert::UTF8ToMB(src, len);
			value->Assign((LPCTSTR)pmb.c_str(), pmb.size());
#endif
		}
		static bool try_get(lua_State * l, int pos, DuiLib::CDuiString* value)
		{
			if (lua_isstring(l, pos))
			{
				from_stack(l, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t <const DuiLib::CDuiString >
	{
		static int push_stack(lua_State* l, const DuiLib::CDuiString& arg_)
		{
			return lua_op_t <DuiLib::CDuiString >::push_stack(l, arg_);
		}
		static void from_stack(lua_State* l, int pos, DuiLib::CDuiString* value)
		{
			lua_op_t <DuiLib::CDuiString >::from_stack(l, pos, value);
		}

		static bool try_get(lua_State * l, int pos, DuiLib::CDuiString* value)
		{
			return lua_op_t <DuiLib::CDuiString >::try_get(l, pos, value);
		}
	};

	template<>
	struct lua_op_t < DuiLib::CDuiString& >
	{
		static int push_stack(lua_State* l, DuiLib::CDuiString& arg_)
		{
			return lua_op_t<DuiLib::CDuiString>::push_stack(l, arg_);
		}
		static void from_stack(lua_State* l, int pos, DuiLib::CDuiString* value)
		{
			lua_op_t<DuiLib::CDuiString>::from_stack(l, pos, value);
		}

		static bool try_get(lua_State * l, int pos, DuiLib::CDuiString* value)
		{
			return lua_op_t<DuiLib::CDuiString>::try_get(l, pos, value);
		}
	};

	template<>
	struct lua_op_t <const DuiLib::CDuiString& >
	{
		static int push_stack(lua_State* l, const DuiLib::CDuiString& arg_)
		{
			return lua_op_t<DuiLib::CDuiString>::push_stack(l, arg_);
		}
		static void from_stack(lua_State* l, int pos, DuiLib::CDuiString* value)
		{
			lua_op_t<DuiLib::CDuiString>::from_stack(l, pos, value);
		}
		static bool try_get(lua_State * l, int pos, DuiLib::CDuiString* value)
		{
			return lua_op_t<DuiLib::CDuiString>::try_get(l, pos, value);
		}
	};

	template<>
	struct lua_op_t < POINT >
	{
		static int push_stack(lua_State* l, const POINT& arg_)
		{
			lua_table_ref_t t(l, (char*)NULL);
			t.set("x", arg_.x);
			t.set("y", arg_.y);
			lua::push(l, t);
			t.unref();
			return 1;
		}
		static void from_stack(lua_State* l, int pos, POINT* value)
		{
			LUA_CHECK_ERROR(lua_istable(l, pos) != 0, LUA_TTABLE, pos);
			lua_table_ref_t t;
			lua::get(l, pos, &t);
			int x, y;
			t.get("x", &x);
			t.get("y", &y);
			value->x = x;
			value->y = y;
			t.unref();
		}
		static bool try_get(lua_State * l, int pos, POINT* value)
		{
			if (lua_istable(l, pos))
			{
				from_stack(l, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t < DuiLib::CDuiPoint >
	{
		static int push_stack(lua_State* l, const DuiLib::CDuiPoint& arg_)
		{
			lua_table_ref_t t(l, (char*)NULL);
			t.set("x", arg_.x);
			t.set("y", arg_.y);
			lua::push(l, t);
			t.unref();
			return 1;
		}
		static void from_stack(lua_State* l, int pos, DuiLib::CDuiPoint* value)
		{
			LUA_CHECK_ERROR(lua_istable(l, pos) != 0, LUA_TTABLE, pos);
			lua_table_ref_t t;
			lua::get(l, pos, &t);
			int x, y;
			t.get("x", &x);
			t.get("y", &y);
			value->x = x;
			value->y = y;
			t.unref();
		}
		static bool try_get(lua_State * l, int pos, DuiLib::CDuiPoint* value)
		{
			if (lua_istable(l, pos))
			{
				from_stack(l, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t < DuiLib::CDuiPoint& >
	{
		static int push_stack(lua_State* l, const DuiLib::CDuiPoint& arg_)
		{
			return lua_op_t<DuiLib::CDuiPoint>::push_stack(l, arg_);
		}
		static void from_stack(lua_State* l, int pos, DuiLib::CDuiPoint* value)
		{
			lua_op_t<DuiLib::CDuiPoint>::from_stack(l, pos, value);
		}
		static bool try_get(lua_State * l, int pos, DuiLib::CDuiPoint* value)
		{
			return lua_op_t<DuiLib::CDuiPoint>::try_get(l, pos, value);
		}
	};

	template<>
	struct lua_op_t < SIZE >
	{
		static int push_stack(lua_State* l, const SIZE& arg_)
		{
			lua_table_ref_t t(l, (char*)NULL);
			t.set("cx", arg_.cx);
			t.set("cy", arg_.cy);
			lua::push(l, t);
			t.unref();
			return 1;
		}
		static void from_stack(lua_State* l, int pos, SIZE* value)
		{
			LUA_CHECK_ERROR(lua_istable(l, pos) != 0, LUA_TTABLE, pos);
			lua_table_ref_t t;
			lua::get(l, pos, &t);
			int cx, cy;
			t.get("cx", &cx);
			t.get("cy", &cy);
			value->cx = cx;
			value->cy = cy;
			t.unref();
		}
		static bool try_get(lua_State * l, int pos, SIZE* value)
		{
			if (lua_istable(l, pos))
			{
				from_stack(l, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t < DuiLib::CDuiSize >
	{
		static int push_stack(lua_State* l, const DuiLib::CDuiSize& arg_)
		{
			lua_table_ref_t t(l, (char*)NULL);
			t.set("cx", arg_.cx);
			t.set("cy", arg_.cy);
			lua::push(l, t);
			t.unref();
			return 1;
		}
		static void from_stack(lua_State* l, int pos, DuiLib::CDuiSize* value)
		{
			LUA_CHECK_ERROR(lua_istable(l, pos) != 0, LUA_TTABLE, pos);
			lua_table_ref_t t;
			lua::get(l, pos, &t);
			int cx, cy;
			t.get("cx", &cx);
			t.get("cy", &cy);
			value->cx = cx;
			value->cy = cy;
			t.unref();
		}
		static bool try_get(lua_State * l, int pos, DuiLib::CDuiSize* value)
		{
			if (lua_istable(l, pos))
			{
				from_stack(l, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t < DuiLib::CDuiSize& >
	{
		static int push_stack(lua_State* l, const DuiLib::CDuiSize& arg_)
		{
			return lua_op_t<DuiLib::CDuiSize>::push_stack(l, arg_);
		}
		static void from_stack(lua_State* l, int pos, DuiLib::CDuiSize* value)
		{
			lua_op_t<DuiLib::CDuiSize>::from_stack(l, pos, value);
		}
		static bool try_get(lua_State * l, int pos, DuiLib::CDuiSize* value)
		{
			return lua_op_t<DuiLib::CDuiSize>::try_get(l, pos, value);
		}
	};

	template<>
	struct lua_op_t < RECT >
	{
		static int push_stack(lua_State* l, const RECT& arg_)
		{
			lua_table_ref_t t(l, (char*)NULL);
			t.set("left", arg_.left);
			t.set("top", arg_.top);
			t.set("right", arg_.right);
			t.set("bottom", arg_.bottom);
			lua::push(l, t);
			t.unref();
			return 1;
		}
		static void from_stack(lua_State* l, int pos, RECT* value)
		{
			LUA_CHECK_ERROR(lua_istable(l, pos) != 0, LUA_TTABLE, pos);
			lua_table_ref_t t;
			lua::get(l, pos, &t);
			int left, top, right, bottom;
			t.get("left", &left);
			t.get("top", &top);
			t.get("right", &right);
			t.get("bottom", &bottom);
			value->left = left;
			value->top = top;
			value->right = right;
			value->bottom = bottom;
			t.unref();
		}
		static bool try_get(lua_State * l, int pos, RECT* value)
		{
			if (lua_istable(l, pos))
			{
				from_stack(l, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t < DuiLib::CDuiRect >
	{
		static int push_stack(lua_State* l, const DuiLib::CDuiRect& arg_)
		{
			lua_table_ref_t t(l, (char*)NULL);
			t.set("left", arg_.left);
			t.set("top", arg_.top);
			t.set("right", arg_.right);
			t.set("bottom", arg_.bottom);
			lua::push(l, t);
			t.unref();
			return 1;
		}
		static void from_stack(lua_State* l, int pos, DuiLib::CDuiRect* value)
		{
			LUA_CHECK_ERROR(lua_istable(l, pos) != 0, LUA_TTABLE, pos);
			lua_table_ref_t t;
			lua::get(l, pos, &t);
			int left, top, right, bottom;
			t.get("left", &left);
			t.get("top", &top);
			t.get("right", &right);
			t.get("bottom", &bottom);
			value->left = left;
			value->top = top;
			value->right = right;
			value->bottom = bottom;
			t.unref();
		}
		static bool try_get(lua_State * l, int pos, DuiLib::CDuiRect* value)
		{
			if (lua_istable(l, pos))
			{
				from_stack(l, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t < DuiLib::CDuiRect& >
	{
		static int push_stack(lua_State* l, const DuiLib::CDuiRect& arg_)
		{
			return lua_op_t<DuiLib::CDuiRect>::push_stack(l, arg_);
		}
		static void from_stack(lua_State* l, int pos, DuiLib::CDuiRect* value)
		{
			lua_op_t<DuiLib::CDuiRect>::from_stack(l, pos, value);
		}
		static bool try_get(lua_State * l, int pos, DuiLib::CDuiRect* value)
		{
			return lua_op_t<DuiLib::CDuiRect>::try_get(l, pos, value);
		}
	};

	template<>
	struct lua_op_t < DuiLib::CDuiStringPtrMap >
	{
		static int push_stack(lua_State* l, const DuiLib::CDuiStringPtrMap& arg_)
		{
			lua_newtable(l);
			for (int i = 0; i < arg_.GetSize(); ++i)
			{
				DuiLib::CDuiString szKey = arg_[i];
				LPVOID data = arg_.Find(szKey);
				lua::push(l, szKey);
				lua::push(l, data);
				lua_settable(l, -3);
			}
			return 1;
		}
		static void from_stack(lua_State* l, int pos, DuiLib::CDuiStringPtrMap* value)
		{
			if (lua_isnoneornil(l, pos))
			{
				return;
			}
			LUA_CHECK_ERROR(0 != lua_istable(l, pos), LUA_TTABLE, pos);
			lua_pushnil(l);
			int real_pos = pos;
			if (pos < 0) real_pos = real_pos - 1;
			while (lua_next(l, real_pos) != 0)
			{
				DuiLib::CDuiString key;
				LPVOID val;
				lua::get(l, -2, &key);
				lua::get(l, -1, &val);
				value->Set(key, val);
				lua_pop(l, 1);
			}
		}
		static bool try_get(lua_State * l, int pos, DuiLib::CDuiStringPtrMap* value)
		{
			if (!lua_istable(l, pos)) return false;
			lua_op_t<DuiLib::CDuiStringPtrMap>::from_stack(l, pos, value);
			return true;
		}
	};

	template<>
	struct lua_op_t < DuiLib::CDuiStringPtrMap* >
	{
		static int push_stack(lua_State* l, const DuiLib::CDuiStringPtrMap* arg_)
		{
			return lua_op_t<DuiLib::CDuiStringPtrMap>::push_stack(l, *arg_);
		}
		static void from_stack(lua_State* l, int pos, DuiLib::CDuiStringPtrMap** value)
		{
		}
		static bool try_get(lua_State * l, int pos, DuiLib::CDuiStringPtrMap** value)
		{
			if (!lua_istable(l, pos)) return false;
			return true;
		}
	};
	
	template<>
	struct lua_op_t < DuiLib::CDuiPtrArray >
	{
		static int push_stack(lua_State* l, const DuiLib::CDuiPtrArray& arg_)
		{
			lua_newtable(l);
			for (int i=0; i < arg_.GetSize(); ++i)
			{
				LPVOID data = arg_.GetAt(i);
				lua::push(l, i+1);
				lua::push(l, data);
				lua_settable(l, -3);
			}
			return 1;
		}
		static void from_stack(lua_State* l, int pos, DuiLib::CDuiPtrArray* value)
		{
		}
		static bool try_get(lua_State * l, int pos, DuiLib::CDuiPtrArray* value)
		{
			if (!lua_istable(l, pos)) return false;
			lua_op_t<DuiLib::CDuiPtrArray>::from_stack(l, pos, value);
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
LUA_DUI_CLASS_OP_T(DuiLib::CWindowWnd)
LUA_DUI_CLASS_OP_T(DuiLib::CDialogBuilder)
LUA_DUI_CLASS_OP_T(DuiLib::CPaintManagerUI)
LUA_DUI_CLASS_OP_T(DuiLib::CMarkup)
LUA_DUI_CLASS_OP_T(DuiLib::CMarkupNode)
LUA_DUI_CLASS_OP_T(DuiLib::CTrayIconUI)
LUA_DUI_CLASS_OP_T(DuiLib::CWin)
LUA_DUI_CLASS_OP_T(DuiLib::CWndShadow)

namespace lua
{
#ifdef LUA_OBJECT_EXTERN
	template<typename T>
	struct lua_op_t < T* >
	{
		static int push_stack(lua_State* l, T* value)
		{
			if (value)
			{
				if (class_name_t<T>::name_.empty())
				{
					pushobject(l, (void*)(value), NULL);
				}
				else
				{
					std::string meta = class_name_t<T>::meta();
					pushobject(l, (void*)(value), meta.c_str());
				}
			}
			else
				lua_pushnil(l);
			return 1;
		}
		static void from_stack(lua_State* l, int pos, T** value)
		{
			if (lua_isnoneornil(l, pos))
			{
				*value = 0;
				return;
			}
			LUA_CHECK_ERROR(lua_isuserdata(l, pos) != 0, LUA_TUSERDATA, pos);
			*value = checkobject<T>(l, pos);
		}
		static bool try_get(lua_State* l, int pos, T** value)
		{
			if (lua_isnil(l, pos) || lua_isuserdata(l, pos))
			{
				if (lua_isnoneornil(l, pos))
				{
					*value = 0;
					return true;
				}
				*value = checkobject<T>(l, pos, false);
				return true;
			}
			else
				return false;
		}
	};
#endif
}
#endif//lua_duilib_wrapper_hpp