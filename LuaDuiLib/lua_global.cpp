#define LUA_LIB
#include "UIlib.h"
#include "base/lua_wrapper.hpp"
#include <algorithm>
#include <functional>
#include <time.h>

namespace 
{
	static int Int64ToString(lua_State* l)
	{
		uint64 n; lua::get(l, 1, &n);
		std::stringstream ss;
		ss << n;
		lua::push(l, ss.str());

		return 1;
	}

	static int Int64ToInt_(lua_State* l)
	{
		uint64 n; lua::get(l, 1, &n);
		lua_Number v = n;
		lua::push(l, v);
		return 1;
	}

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
		void* p = lua_touserdata(l, 1);
		uint64 n = (uint64)(intptr_t)p;
		lua::push(l, n);
		return 1;
	}
	static int UInt64ToUserData(lua_State* l)
	{
		uint64 n; lua::get(l, 1, &n);
		void* p = (void*)(intptr_t)n;
		lua_pushlightuserdata(l, p);
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
	static T pow64(T a, T b) {
		if (b == 1) {
			return a;
		}
		T a2 = a * a;
		if (b % 2 == 1) {
			return pow64(a2, b / 2) * a;
		}
		else {
			return pow64(a2, b / 2);
		}
	}

	template<typename T>
	static int MInt64Pow(lua_State* l)
	{
		T p;
		T a;
		lua::get(l, 1, &a);
		if (lua_isnumber(l, 2))
		{
			int b = (int)luaL_checkinteger(l, 2);
			if (b > 0) {
				p = pow64(a, (T)b);
			}
			else if (b == 0) {
				p = 1;
			}
			else {
				return luaL_error(l, "pow by nagtive number %d", b);
			}
		}
		else
		{
			T b;
			lua::get(l, 2, &b);
			if (b > 0) {
				p = pow64(a, b);
			}
			else if (b == 0) {
				p = 1;
			}
			else {
				return luaL_error(l, "pow by nagtive number %d", (int)b);
			}
		}

		lua::push(l, p);
		return 1;
	}

	template<typename T>
	static int not64(lua_State* l)
	{
		T a;
		lua::get(l, 1, &a);

		return lua::push(l, ~a);
	}
	template<typename T>
	static int band64(lua_State* l)
	{
		T p;
		lua::get(l, 1, &p);
		int n = lua_gettop(l);
		for (int i = 2; i <= n; i++) {
			T b;
			lua::get(l, i, &b);
			p &= b;
		}
		
		return lua::push(l, p);
	}
	template<typename T>
	static int bor64(lua_State* l)
	{
		T p;
		lua::get(l, 1, &p);
		int n = lua_gettop(l);
		for (int i = 2; i <= n; i++) {
			T b;
			lua::get(l, i, &b);
			p |= b;
		}

		return lua::push(l, p);
	}
	template<typename T>
	static int bxor64(lua_State* l)
	{
		T p;
		lua::get(l, 1, &p);
		int n = lua_gettop(l);
		for (int i = 2; i <= n; i++) {
			T b;
			lua::get(l, i, &b);
			p ^= b;
		}

		return lua::push(l, p);
	}

	template<typename T>
	static int lshift64(lua_State* l)
	{
		T p;
		int n;
		lua::get(l, 1, &p, &n);
		p << n;
		return lua::push(l, p);
	}
	template<typename T>
	static int rshift64(lua_State* l)
	{
		T p;
		int n;
		lua::get(l, 1, &p, &n);
		p >> n;
		return lua::push(l, p);
	}

	static int lua_isint64(lua_State* l)
	{
		if (!lua_isstring(l, 1))
		{
			return lua::push(l, false);
		}
		size_t len = 0;
		const char* str = luaL_checklstring(l, 1, &len);
		if (len != 8)
		{
			return lua::push(l, false);
		}
		std::istringstream iss(str);
		::uint64 num;
		if (!(iss >> num))
		{
			return lua::push(l, false);
		}

		return lua::push(l, true);
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

	static int lua_GetTimeOfDay(lua_State* l)
	{
		struct timeval tpend;
		//gettimeofday(&tpend, NULL);

		//int secofday = (tpend.tv_sec + 3600 * 8) % 86400;
		//int hours = secofday / 3600;
		//int minutes = (secofday - hours * 3600) / 60;
		//int seconds = secofday % 60;
		//int milliseconds = tpend.tv_usec / 1000;

#define MSET(x) t.set(#x, tpend.x)
		lua::lua_table_ref_t t(l, (char*)NULL);
		MSET(tv_sec);
		MSET(tv_usec);
		lua::push(l, t);
		t.unref();
#undef MSET

		return 1;
	}

	static int lua_HIWORD(lua_State* l)
	{
		DWORD dw;
		lua::get(l, 1, &dw);
		UINT r = HIWORD(dw);
		lua::push(l, r);
		return 1;
	}
	static int lua_LOWORD(lua_State* l)
	{
		DWORD dw;
		lua::get(l, 1, &dw);
		UINT r = LOWORD(dw);
		lua::push(l, r);
		return 1;
	}

	static int lua_MBToUTF8(lua_State* L)
	{
		const char* mb = lua_tostring(L, 1);
		std::string u8 = DuiLib::Convert::MBToUTF8(mb);
		lua::push(L, u8);
		return 1;
	}
	static int lua_UTF8ToMB(lua_State* L)
	{
		const char* u8 = lua_tostring(L, 1);
		std::string mb = DuiLib::Convert::UTF8ToMB(u8);
		lua::push(L, mb);
		return 1;
	}
}

namespace DuiLib {
	int make_lib64(lua_State* l)
	{
		lua::stack_gurad guard(l);
		lua_createtable(l, 0, 0);
		lua_pushstring(l, "Int64ToInt");
		lua_pushcfunction(l, Int64ToInt_);
		lua_rawset(l, -3);
		lua_pushstring(l, "Int64ToString");
		lua_pushcfunction(l, Int64ToString);
		lua_rawset(l, -3);

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

		lua_pushstring(l, "bnot");
		lua_pushcfunction(l, not64<int64>);
		lua_rawset(l, -3);
		lua_pushstring(l, "band");
		lua_pushcfunction(l, band64<int64>);
		lua_rawset(l, -3);
		lua_pushstring(l, "bor");
		lua_pushcfunction(l, bor64<int64>);
		lua_rawset(l, -3);
		lua_pushstring(l, "bxor");
		lua_pushcfunction(l, bxor64<int64>);
		lua_rawset(l, -3);
		lua_pushstring(l, "isint64");
		lua_pushcfunction(l, lua_isint64);
		lua_rawset(l, -3);

		lua_setglobal(l, "lib64");
		return 1;
	}
	int make_lua_global(lua_State* l)
	{
		make_lib64(l);

		lua::stack_gurad guard(l);
		lua_createtable(l, 0, 0);

		lua_pushstring(l, "gmtime");
		lua_pushcfunction(l, Mgmtime_s);
		lua_rawset(l, -3);
		lua_pushstring(l, "strftime");
		lua_pushcfunction(l, Mtcsftime);
		lua_rawset(l, -3);
		lua_pushstring(l, "gettimeofday");
		lua_pushcfunction(l, lua_GetTimeOfDay);
		lua_rawset(l, -3);

		lua_pushstring(l, "HIWORD");
		lua_pushcfunction(l, lua_HIWORD);
		lua_rawset(l, -3);
		lua_pushstring(l, "LOWORD");
		lua_pushcfunction(l, lua_LOWORD);
		lua_rawset(l, -3);

		lua_pushstring(l, "MBToUTF8");
		lua_pushcfunction(l, lua_MBToUTF8);
		lua_rawset(l, -3);

		lua_pushstring(l, "UTF8ToMB");
		lua_pushcfunction(l, lua_UTF8ToMB);
		lua_rawset(l, -3);

		lua_setglobal(l, "helper");
		return 1;
	}
}
