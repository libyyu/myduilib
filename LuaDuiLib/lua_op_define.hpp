#ifndef LUA_OP_DEFINE_HPP
#define LUA_OP_DEFINE_HPP
#pragma once

#define LUA_DUI_CLASS_OP_T(TYPE) \
namespace lua { \
	template<> \
	struct lua_op_t < TYPE* > \
	{ \
		static int push_stack(lua_State* l, const TYPE* value) \
		{ \
			if (!value) \
			{ \
				lua_pushnil(l); \
				return 1; \
			} \
			std::string szClass = DuiLib::Convert::ToUTF8(value->GetClassName()); \
			std::string meta = "DuiLib."; \
			meta += szClass; \
			meta += "_meta"; \
			pushobject(l, (void*)(value), meta.c_str()); \
			return 1; \
		} \
		static void from_stack(lua_State* l, int pos, TYPE** value) \
		{ \
			if (lua_isnoneornil(l, pos)) \
			{ \
				*value = 0; \
				return; \
			} \
			LUA_CHECK_ERROR(lua_isuserdata(l, pos) != 0, LUA_TUSERDATA, pos); \
			*value = checkobject<TYPE>(l, pos); \
		} \
		static bool try_get(lua_State* l, int pos, TYPE** value) \
		{ \
			if (lua_isnoneornil(l, pos) || lua_isuserdata(l, pos)) \
			{ \
				if (lua_isnoneornil(l, pos)) \
				{ \
					*value = 0; \
					return true; \
				} \
				*value = checkobject<TYPE>(l, pos, false); \
				return true; \
			} \
			else \
				return false; \
		} \
	}; \
}
#endif//LUA_OP_DEFINE_HPP