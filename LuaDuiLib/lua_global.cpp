#define LUA_LIB
#include "UIlib.h"
#include "base/lua_wrapper.hpp"
#include <algorithm>
#include <functional>
#include <time.h>
namespace 
{
	static int IntToInt64(lua_State* l)
	{
		lua_Number v = lua_tonumber(l, 1);
		lua::push(l, (int64)v);
		return 1;
	}
	static int UIntToInt64(lua_State* l)
	{
		lua_Number v = lua_tonumber(l, 1);
		lua::push(l, (int64)(uint64)v);
		return 1;
	}
	static int LongToInt64(lua_State* l)
	{
		lua_Number v = lua_tonumber(l, 1);
		lua::push(l, (int64)(long)v);
		return 1;
	}
	static int ULongToInt64(lua_State* l)
	{
		lua_Number v = lua_tonumber(l, 1);
		lua::push(l, (int64)(uint64)(unsigned long)v);
		return 1;
	}
	static int MIntToUInt64(lua_State* l)
	{
		lua_Number v = lua_tonumber(l, 1);
		lua::push(l, (uint64)v);
		return 1;
	}
	static int MLongToUInt64(lua_State* l)
	{
		lua_Number v = lua_tonumber(l, 1);
		lua::push(l, (uint64)(int64)(long)v);
		return 1;
	}
	static int UIntToUInt64(lua_State* l)
	{
		lua_Number v = lua_tonumber(l, 1);
		lua::push(l, (uint64)(unsigned int)v);
		return 1;
	}
	static int ULongToUInt64(lua_State* l)
	{
		lua_Number v = lua_tonumber(l, 1);
		lua::push(l, (uint64)(unsigned long)v);
		return 1;
	}

	static int UserDataToUInt64(lua_State* l)
	{
		void* ptr = lua::getobject(l, 1);
		uint64 address = (uint64)ptr;
		lua::push(l, address);
		return 1;
	}
	static int UInt64ToUserData(lua_State* l)
	{
		uint64 address;
		lua::get(l, 1, &address);
		void* ptr = reinterpret_cast<void*>(address);
		lua::pushobject(l, ptr, NULL);
		return 1;
	}

	template<typename T>
	static int MInt64Compare(lua_State* l)
	{
		T a, b;
		lua::get(l, 1, &a, &b);
		int v = 0;
		if (a < b) v = -1;
		else if (a > b) v = 1;
		lua::push(l, v);
		return 1;
	}
	
	template<typename T>
	static int MInt64Add(lua_State* l)
	{
		T a;
		lua::get(l, 1, &a);
		if (lua_isnumber(l, 2))
		{
			int b = (int)lua_tonumber(l, 2);
			a += b;
		}
		else
		{
			T b;
			lua::get(l, 2, &b);
			a += b;
		}

		lua::push(l, a);
		return 1;
	}

	template<typename T>
	static int MInt64Sub(lua_State* l)
	{
		T a;
		lua::get(l, 1, &a);
		if (lua_isnumber(l, 2))
		{
			int b = (int)lua_tonumber(l, 2);
			a -= b;
		}
		else
		{
			T b;
			lua::get(l, 2, &b);
			a -= b;
		}

		lua::push(l, a);
		return 1;
	}

	template<typename T>
	static int MInt64Mul(lua_State* l)
	{
		T a;
		lua::get(l, 1, &a);
		if (lua_isnumber(l, 2))
		{
			int b = (int)lua_tonumber(l, 2);
			a *= b;
		}
		else
		{
			T b;
			lua::get(l, 2, &b);
			a *= b;
		}

		lua::push(l, a);
		return 1;
	}

	template<typename T>
	static int MInt64Div(lua_State* l)
	{
		T a;
		lua::get(l, 1, &a);
		if (lua_isnumber(l, 2))
		{
			int b = (int)lua_tonumber(l, 2);
			if (b == 0)
			{
				return luaL_error(l, "div by zero");
			}
			a /= b;
		}
		else
		{
			T b;
			lua::get(l, 2, &b);
			if (b == 0)
			{
				return luaL_error(l, "div by zero");
			}
			a /= b;
		}

		lua::push(l, a);
		return 1;
	}

	template<typename T>
	static int MInt64Mod(lua_State* l)
	{
		T a;
		lua::get(l, 1, &a);
		if (lua_isnumber(l, 2))
		{
			int b = (int)lua_tonumber(l, 2);
			if (b == 0)
			{
				return luaL_error(l, "mod by zero");
			}
			a %= b;
		}
		else
		{
			T b;
			lua::get(l, 2, &b);
			if (b == 0)
			{
				return luaL_error(l, "mod by zero");
			}
			a %= b;
		}

		lua::push(l, a);
		return 1;
	}

	template<typename T>
	static int MInt64Unm(lua_State* l)
	{
		T a;
		lua::get(l, 1, &a);
		a = -a;
		lua::push(l, a);
		return 1;
	}

	template<typename T>
	static int MInt64Pow(lua_State* l)
	{
		T a;
		lua::get(l, 1, &a);
		if (lua_isnumber(l, 2))
		{
			int b = (int)lua_tonumber(l, 2);
			a = (T)pow(a, b);
		}
		else
		{
			T b;
			lua::get(l, 2, &b);
			a = (T)pow(a, b);
		}

		lua::push(l, a);
		return 1;
	}

	static int Mgmtime_s(lua_State* l)
	{
		int64 tt;
		lua::get(l, 1, &tt);
		time_t timeCurrent = tt;
		struct tm   tmCurrent;
		gmtime_s(&tmCurrent, &timeCurrent);

#define MSET(x) t.set(#x, tmCurrent.x)
		lua::lua_table_ref_t t(l, (char*)NULL);
		MSET(tm_sec);
		MSET(tm_min);
		MSET(tm_hour);
		MSET(tm_mday);
		MSET(tm_mon);
		MSET(tm_year);
		MSET(tm_wday);
		MSET(tm_yday);
		MSET(tm_isdst);
		lua::push(l, t);
		t.unref();
#undef MSET
		return 1;
	}

	static int Mtcsftime(lua_State* l)
	{
		struct tm   tmCurrent;
		lua::lua_table_ref_t t;
		lua::get(l, 1, &t);
#define MGET(x) tmCurrent.x = t.get<int>(#x)
		MGET(tm_sec);
		MGET(tm_min);
		MGET(tm_hour);
		MGET(tm_mday);
		MGET(tm_mon);
		MGET(tm_year);
		MGET(tm_wday);
		MGET(tm_yday);
		MGET(tm_isdst);
		t.unref();
#undef MGET
		TCHAR       szCurrent[MAX_PATH] = { 0 };
		_tcsftime(szCurrent, MAX_PATH, _T("%X"), &tmCurrent);
		lua::push(l, szCurrent);
		return 1;
	}
}

int make_lua_global(lua_State* l)
{
	lua::stack_gurad guard(l);
	lua_createtable(l, 0, 0);
	lua_pushstring(l, "IntToInt64");
	lua_pushcfunction(l, IntToInt64);
	lua_rawset(l, -3);
	lua_pushstring(l, "UIntToInt64");
	lua_pushcfunction(l, UIntToInt64);
	lua_rawset(l, -3);
	lua_pushstring(l, "LongToInt64");
	lua_pushcfunction(l, LongToInt64);
	lua_rawset(l, -3);
	lua_pushstring(l, "ULongToInt64");
	lua_pushcfunction(l, ULongToInt64);
	lua_rawset(l, -3);
	lua_pushstring(l, "IntToUInt64");
	lua_pushcfunction(l, MIntToUInt64);
	lua_rawset(l, -3);
	lua_pushstring(l, "UIntToUInt64");
	lua_pushcfunction(l, UIntToUInt64);
	lua_rawset(l, -3);
	lua_pushstring(l, "ULongToUInt64");
	lua_pushcfunction(l, ULongToUInt64);
	lua_rawset(l, -3);
	lua_pushstring(l, "ULongToUInt64");
	lua_pushcfunction(l, ULongToUInt64);
	lua_rawset(l, -3);
	lua_pushstring(l, "UserDataToUInt64");
	lua_pushcfunction(l, UserDataToUInt64);
	lua_rawset(l, -3);
	lua_pushstring(l, "UInt64ToUserData");
	lua_pushcfunction(l, UInt64ToUserData);
	lua_rawset(l, -3);
	lua_pushstring(l, "Int64Compare");
	lua_pushcfunction(l, MInt64Compare<int64>);
	lua_rawset(l, -3);
	lua_pushstring(l, "Int64Add");
	lua_pushcfunction(l, MInt64Add<int64>);
	lua_rawset(l, -3);
	lua_pushstring(l, "Int64Sub");
	lua_pushcfunction(l, MInt64Sub<int64>);
	lua_rawset(l, -3);
	lua_pushstring(l, "Int64Mul");
	lua_pushcfunction(l, MInt64Mul<int64>);
	lua_rawset(l, -3);
	lua_pushstring(l, "Int64Div");
	lua_pushcfunction(l, MInt64Div<int64>);
	lua_rawset(l, -3);
	lua_pushstring(l, "Int64Mod");
	lua_pushcfunction(l, MInt64Mod<int64>);
	lua_rawset(l, -3);
	lua_pushstring(l, "Int64Unm");
	lua_pushcfunction(l, MInt64Unm<int64>);
	lua_rawset(l, -3);
	lua_pushstring(l, "Int64Pow");
	lua_pushcfunction(l, MInt64Pow<int64>);
	lua_rawset(l, -3);
	lua_pushstring(l, "UInt64Compare");
	lua_pushcfunction(l, MInt64Compare<uint64>);
	lua_rawset(l, -3);
	lua_pushstring(l, "UInt64Add");
	lua_pushcfunction(l, MInt64Add<uint64>);
	lua_rawset(l, -3);
	lua_pushstring(l, "UInt64Sub");
	lua_pushcfunction(l, MInt64Sub<uint64>);
	lua_rawset(l, -3);
	lua_pushstring(l, "UInt64Mul");
	lua_pushcfunction(l, MInt64Mul<uint64>);
	lua_rawset(l, -3);
	lua_pushstring(l, "UInt64Div");
	lua_pushcfunction(l, MInt64Div<uint64>);
	lua_rawset(l, -3);
	lua_pushstring(l, "UInt64Mod");
	lua_pushcfunction(l, MInt64Mod<uint64>);
	lua_rawset(l, -3);
	lua_pushstring(l, "UInt64Unm");
	lua_pushcfunction(l, MInt64Unm<uint64>);
	lua_rawset(l, -3);
	lua_pushstring(l, "UInt64Pow");
	lua_pushcfunction(l, MInt64Pow<uint64>);
	lua_rawset(l, -3);

	lua_pushstring(l, "gmtime");
	lua_pushcfunction(l, Mgmtime_s);
	lua_rawset(l, -3);
	lua_pushstring(l, "strftime");
	lua_pushcfunction(l, Mtcsftime);
	lua_rawset(l, -3);
	
	lua_setglobal(l, "helper");
	return 1;
}
