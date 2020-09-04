#ifndef LUA_OP_DEFINE_HPP
#define LUA_OP_DEFINE_HPP
#pragma once

#define LUA_DUI_CLASS_OP_T(TYPE) \
namespace lua { \
	template<> \
	struct lua_op_t < TYPE* > \
	{ \
		static int push_stack(lua_State* L, const TYPE* value) \
		{ \
			if (!value) \
			{ \
				lua_pushnil(L); \
				return 1; \
			} \
			std::string szClass = DuiLib::Convert::ToUTF8(value->GetClassName()); \
			std::string meta = "DuiLib."; \
			meta += szClass; \
			meta += "_meta"; \
			push_object(L, (void*)(value), meta.c_str()); \
			record_object_to_lua_global_table(L, (void*)(value)); \
			return 1; \
		} \
		static void from_stack(lua_State* L, int pos, TYPE** value) \
		{ \
			if (lua_isnoneornil(L, pos)) \
			{ \
				*value = 0; \
				return; \
			} \
			LUA_CHECK_PARAM(lua_isuserdata(L, pos) != 0, type::userdata, pos); \
			*value = check_type<TYPE>(L, pos); \
		} \
		static bool try_get(lua_State* L, int pos, TYPE** value) \
		{ \
			if (lua_isnoneornil(L, pos) || lua_isuserdata(L, pos)) \
			{ \
				if (lua_isnoneornil(L, pos)) \
				{ \
					*value = 0; \
					return true; \
				} \
				*value = get_type<TYPE>(L, pos); \
				return true; \
			} \
			else \
				return false; \
		} \
	}; \
}
#endif//LUA_OP_DEFINE_HPP