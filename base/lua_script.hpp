/*
 *  File:    lua_script.hpp
 *
 *  Author:  lidengfeng
 *  Date:    2017/06/12
 *  Purpose: lua和C++交互基础操作
             由于使用了模板变参，因此需要C++11或更高版本
 */
#ifndef _LUA_SCRIPT_HPP__
#define _LUA_SCRIPT_HPP__

#include <exception>
//#if !SUPPORT_PARAMS
//#warning "This library needs at least a C++11 compliant compiler, so some functions can not used."
//#endif

#include "lua.hpp"
#include <assert.h>
#include <list>
#include <set>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>

#define LUA_FUNC_DELC(name) int lua_##name(lua_State* l);
#define LUA_FUNC_IMPL(name) int lua_##name(lua_State* l)
#define LUA_FUNC_NAME(name) lua_##name
#define LUA_FUNC_REG(name) { #name, lua_##name }

#define SUPPORT_PARAMS 1
#define SUPPORT_CXX11  1

#ifdef _MSC_VER
typedef __int64           				int64;
typedef unsigned __int64  				uint64;
#else
typedef __int64_t         				int64;
typedef __uint64_t        				uint64;
#endif

namespace lua
{
	inline std::string gettraceback(lua_State *l, const char* err = "Lua traceback:")
	{
		int oldTop = lua_gettop(l);
		lua_checkstack(l, 3);
		lua_getglobal(l, "debug");
		lua_getfield(l, -1, "traceback");
		lua_pushstring(l, err);
		lua_pushnumber(l, 1);
		lua_pcall(l, 2, 1, 0);
		std::string trace = lua_tostring(l, -1);
		lua_settop(l, oldTop);
		return trace;
	}

	#define LUA_CHECK_ERROR(expr, type, idx) error_report(l, (expr), type, idx, #expr, __FILE__, __LINE__);

	inline void error_report(lua_State *l, bool suc, int type, int idx, const char *msg, const char* file, int line)
	{
		if( suc )
			return;

		std::ostringstream os;
		os << "lua parameter error: " << std::endl
			<< "index [" << idx << "]" << std::endl
			<< "real type [" << ::lua_typename(l, ::lua_type(l, idx)) << "]" << std::endl
			<< "expected type [" << ::lua_typename(l, type) << "]" << std::endl
			<< "information [" << msg << "]" << std::endl
			<< file << ":" << line << std::endl;

		luaL_error(l, "%s\n", os.str().c_str());
	}

	class lua_exception_t : public std::exception
	{
	public:
		explicit lua_exception_t(const char* err_) :
			m_err(err_)
		{}
		explicit lua_exception_t(const std::string& err_) :
			m_err(err_)
		{}
		~lua_exception_t() noexcept
		{
		}

		virtual char const* what() const
		{
			return m_err.c_str();
		}
	private:
		std::string m_err;
	};

	////////////////////////////////////////////////////////////////////////////////

	struct stack_gurad
	{
		lua_State* L;
		int nTop;
		stack_gurad(lua_State* L_) : L(L_){ nTop = lua_gettop(L); }
		~stack_gurad() { lua_settop(L, nTop); }
	};

	template<typename T>
	struct lua_op_t;

	struct lua_nil_t {
		lua_nil_t(){}
	};
	static const lua_nil_t Nil;

	template<>
	struct lua_op_t<lua_nil_t>
	{
		static int push_stack(lua_State* l, const lua_nil_t&)
		{
			lua_pushnil(l);
			return 1;
		}
		static void from_stack(lua_State* l, int pos, lua_nil_t* )
		{
			LUA_CHECK_ERROR(0 != lua_isnoneornil(l, pos), LUA_TNIL, pos);
		}
		static bool try_get(lua_State * l, int pos, lua_nil_t*)
		{
			return lua_isnoneornil(l, pos);
		}
	};

	struct lua_bytes_t {
		std::basic_string<unsigned char, std::char_traits<unsigned char>, std::allocator<unsigned char> > buff;
		bool is_nil;
		lua_bytes_t(unsigned char* buff_ = nullptr) {
			init(buff_);
		}
		void init(unsigned char* buff_){
			if (buff_){
				is_nil = false;
				buff.assign(buff_);
			}
			else {
				is_nil = true;
			}
		}
		size_t length() const {
			return buff.size();
		}
		operator const unsigned char*() { return buff.c_str(); }
	};

	template<>
	struct lua_op_t<lua_bytes_t>
	{
		static int push_stack(lua_State* l, const lua_bytes_t& value)
		{
			if (!value.is_nil)
				lua_pushlstring(l, (const char*)(value.buff.c_str()), value.length());
			else
				lua_pushnil(l);
			return 1;
		}
		static void from_stack(lua_State* l, int pos, lua_bytes_t* value)
		{
			if (lua_isnoneornil(l, pos))
			{
				(*value).init(nullptr);
				return;
			}
			LUA_CHECK_ERROR(0 != lua_isstring(l, pos), LUA_TSTRING, pos);
			size_t sz;
			(*value).init((unsigned char*)lua_tolstring(l, pos, &sz));
		}
		static bool try_get(lua_State * l, int pos, lua_bytes_t* value)
		{
			if (lua_isnoneornil(l, pos) || lua_isstring(l, pos))
			{
				from_stack(l, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t<char>
	{
		static int push_stack(lua_State* l, char value)
		{
			lua_pushnumber(l, (lua_Number)value);
			return 1;
		}
		static void from_stack(lua_State* l, int pos, char* value)
		{
			if (lua_isnoneornil(l, pos))
			{
				return;
			}
			LUA_CHECK_ERROR(0 != lua_isnumber(l, pos), LUA_TNUMBER, pos);
			*value = (char)luaL_checknumber(l, pos);
		}
		static bool try_get(lua_State * l, int pos, char* value)
		{
			if (lua_isnoneornil(l, pos) || lua_isnumber(l, pos))
			{
				from_stack(l, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t<unsigned char>
	{
		static int push_stack(lua_State* l, unsigned char value)
		{
			lua_pushnumber(l, (lua_Number)value);
			return 1;
		}
		static void from_stack(lua_State* l, int pos, unsigned char* value)
		{
			if (lua_isnoneornil(l, pos))
			{
				return;
			}
			LUA_CHECK_ERROR(0 != lua_isnumber(l, pos), LUA_TNUMBER, pos);
			*value = (unsigned char)luaL_checknumber(l, pos);
		}
		static bool try_get(lua_State * l, int pos, unsigned char* value)
		{
			if (lua_isnoneornil(l, pos) || lua_isnumber(l, pos))
			{
				from_stack(l, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t<signed char>
	{
		static int push_stack(lua_State* l, signed char value)
		{
			lua_pushnumber(l, (lua_Number)value);
			return 1;
		}
		static void from_stack(lua_State* l, int pos, signed char* value)
		{
			if (lua_isnoneornil(l, pos))
			{
				return;
			}
			LUA_CHECK_ERROR(0 != lua_isnumber(l, pos), LUA_TNUMBER, pos);
			*value = (signed char)luaL_checknumber(l, pos);
		}
		static bool try_get(lua_State * l, int pos, signed char* value)
		{
			if (lua_isnoneornil(l, pos) || lua_isnumber(l, pos))
			{
				from_stack(l, pos, value);
				return true;
			}
			else
				return false;
		}
	};
	
	template<>
	struct lua_op_t<char*>
	{
		static int push_stack(lua_State* l, char* value)
		{
			if (value)
				lua_pushstring(l, value);
			else
				lua_pushnil(l);
			return 1;
		}
		static void from_stack(lua_State* l, int pos, char** value)
		{
			if (lua_isnoneornil(l, pos))
			{
				*value = 0;
				return;
			}
			LUA_CHECK_ERROR(0 != lua_isstring(l, pos), LUA_TSTRING, pos);
			*value = (char*)luaL_checkstring(l, pos);
		}
		static bool try_get(lua_State * l, int pos, char** value)
		{
			if (lua_isnoneornil(l, pos) || lua_isstring(l, pos))
			{
				from_stack(l, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t<const char*>
	{
		static int push_stack(lua_State* l, const char* value)
		{
			if (value)
				lua_pushstring(l, value);
			else
				lua_pushnil(l);
			return 1;
		}
		static void from_stack(lua_State* l, int pos, const char** value)
		{
			if (lua_isnoneornil(l, pos))
			{
				*value = 0;
				return;
			}
			LUA_CHECK_ERROR(0 != lua_isstring(l, pos), LUA_TSTRING, pos);
			*value = (const char*)luaL_checkstring(l, pos);
		}
		static bool try_get(lua_State * l, int pos, const char** value)
		{
			if (lua_isnoneornil(l, pos) || lua_isstring(l, pos))
			{
				from_stack(l, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t<unsigned char*>
	{
		static int push_stack(lua_State* l, unsigned char* value)
		{
			lua_pushlstring(l, (const char*)value, strlen((char*)value));
			return 1;
		}
		static void from_stack(lua_State* l, int pos, unsigned char** value)
		{
			if (lua_isnoneornil(l, pos))
			{
				return;
			}
			LUA_CHECK_ERROR(0 != lua_isstring(l, pos), LUA_TSTRING, pos);
			size_t sz;
			(*value) = (unsigned char*)lua_tolstring(l, pos, &sz);
		}
		static bool try_get(lua_State * l, int pos, unsigned char** value)
		{
			if (lua_isnoneornil(l, pos) || lua_isnumber(l, pos))
			{
				from_stack(l, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<int N>
	struct lua_op_t<char[N]>
	{
		static int push_stack(lua_State* l, const char* value)
		{
			if (value)
				lua_pushstring(l, value);
			else
				lua_pushnil(l);
			return 1;
		}
		static void from_stack(lua_State* l, int pos, const char** value)
		{
			if (lua_isnoneornil(l, pos))
			{
				*value = 0;
				return;
			}
			LUA_CHECK_ERROR(lua_isstring(l, pos) != 0, LUA_TSTRING, pos);
			*value = (const char*)luaL_checkstring(l, pos);
		}
		static bool try_get(lua_State * l, int pos, const char** value)
		{
			if (lua_isnoneornil(l, pos) || lua_isstring(l, pos))
			{
				from_stack(l, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<int N>
	struct lua_op_t<const char[N]>
	{
		static int push_stack(lua_State* l, const char* value)
		{
			if (value)
				lua_pushstring(l, value);
			else
				lua_pushnil(l);
			return 1;
		}
		static void from_stack(lua_State* l, int pos, const char** value)
		{
			if (lua_isnoneornil(l, pos))
			{
				*value = 0;
				return;
			}
			LUA_CHECK_ERROR(lua_isstring(l, pos) != 0, LUA_TSTRING, pos);
			*value = (const char*)luaL_checkstring(l, pos);
		}
		static bool try_get(lua_State * l, int pos, const char** value)
		{
			if (lua_isnoneornil(l, pos) || lua_isstring(l, pos))
			{
				from_stack(l, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t<bool>
	{
		static int push_stack(lua_State* l, bool value)
		{
			lua_pushboolean(l, value);
			return 1;
		}
		static void from_stack(lua_State* l, int pos, bool* value)
		{
			LUA_CHECK_ERROR(lua_isboolean(l, pos) || lua_isnoneornil(l, pos), LUA_TBOOLEAN, pos);
			if (lua_isboolean(l, pos))
			{
				*value = (0 != lua_toboolean(l, pos));
			}
			else
			{
				*value = false;
			}
		}
		static bool try_get(lua_State * l, int pos, bool* value)
		{
			if (lua_isnoneornil(l, pos) || lua_isboolean(l, pos))
			{
				from_stack(l, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t<int>
	{
		static int push_stack(lua_State* l, int value)
		{
			lua_pushinteger(l, value);
			return 1;
		}
		static void from_stack(lua_State* l, int pos, int* value)
		{
			LUA_CHECK_ERROR(0 != lua_isnumber(l, pos), LUA_TNUMBER, pos);
			*value = (int)luaL_checkinteger(l, pos);
		}
		static bool try_get(lua_State * l, int pos, int* value)
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
	struct lua_op_t<unsigned int>
	{
		static int push_stack(lua_State* l, unsigned int value)
		{
			lua_pushinteger(l, value);
			return 1;
		}
		static void from_stack(lua_State* l, int pos, unsigned int* value)
		{
			LUA_CHECK_ERROR(0 != lua_isnumber(l, pos), LUA_TNUMBER, pos);
			*value = (unsigned int)luaL_checkinteger(l, pos);
		}
		static bool try_get(lua_State * l, int pos, unsigned int* value)
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
	struct lua_op_t<short>
	{
		static int push_stack(lua_State* l, short value)
		{
			lua_pushnumber(l, (lua_Number)value);
			return 1;
		}
		static void from_stack(lua_State* l, int pos, short* value)
		{
			LUA_CHECK_ERROR(0 != lua_isnumber(l, pos), LUA_TNUMBER, pos);
			*value = (short)luaL_checknumber(l, pos);
		}
		static bool try_get(lua_State * l, int pos, short* value)
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
	struct lua_op_t<unsigned short>
	{
		static int push_stack(lua_State* l, unsigned short value)
		{
			lua_pushnumber(l, (lua_Number)value);
			return 1;
		}
		static void from_stack(lua_State* l, int pos, unsigned short* value)
		{
			LUA_CHECK_ERROR(0 != lua_isnumber(l, pos), LUA_TNUMBER, pos);
			*value = (unsigned short)luaL_checknumber(l, pos);
		}
		static bool try_get(lua_State * l, int pos, unsigned short* value)
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
	struct lua_op_t<float>
	{
		static int push_stack(lua_State* l, float value)
		{
			lua_pushnumber(l, (lua_Number)value);
			return 1;
		}
		static void from_stack(lua_State* l, int pos, float* value)
		{
			LUA_CHECK_ERROR(0 != lua_isnumber(l, pos), LUA_TNUMBER, pos);
			*value = (float)luaL_checknumber(l, pos);
		}
		static bool try_get(lua_State * l, int pos, float* value)
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
	struct lua_op_t<double>
	{
		static int push_stack(lua_State* l, double value)
		{
			lua_pushnumber(l, (lua_Number)value);
			return 1;
		}
		static void from_stack(lua_State* l, int pos, double* value)
		{
			LUA_CHECK_ERROR(0 != lua_isnumber(l, pos), LUA_TNUMBER, pos);
			*value = (double)luaL_checknumber(l, pos);
		}
		static bool try_get(lua_State * l, int pos, double* value)
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
#if 1//BUILD_64BIT
	template<>
	struct lua_op_t<long>
	{
		static int push_stack(lua_State* l, long value)
		{
			lua_pushnumber(l, (lua_Number)value);
			return 1;
		}
		static void from_stack(lua_State* l, int pos, long* value)
		{
			LUA_CHECK_ERROR(0 != lua_isnumber(l, pos), LUA_TNUMBER, pos);
			*value = (long)luaL_checknumber(l, pos);
		}
		static bool try_get(lua_State * l, int pos, long* value)
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
	struct lua_op_t<unsigned long>
	{
		static int push_stack(lua_State* l, unsigned long value)
		{
			lua_pushnumber(l, (lua_Number)value);
			return 1;
		}
		static void from_stack(lua_State* l, int pos, unsigned long* value)
		{
			LUA_CHECK_ERROR(0 != lua_isnumber(l, pos), LUA_TNUMBER, pos);
			*value = (unsigned long)luaL_checknumber(l, pos);
		}
		static bool try_get(lua_State * l, int pos, unsigned long* value)
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
#endif//BUILD_64BIT
    
	template<>
	struct lua_op_t<::int64>
	{
		static int push_stack(lua_State* l, ::int64 value)
		{
			std::stringstream ss;
			ss << value;
			std::string str = ss.str();

			lua_getglobal(l, "make_i64");
			if (lua_isfunction(l, -1))
			{
				lua_pushlstring(l, str.c_str(), str.length());
				lua_pushboolean(l, false);
				lua_pcall(l, 2, 1, 0);
			}
			else
			{
				lua_pop(l, 1);
				lua_pushlstring(l, str.c_str(), str.length());
			}
			
			return 1;
		}
		static void from_stack(lua_State* l, int pos, ::int64* value)
		{
			if (lua_isnoneornil(l, pos))
			{
				return;
			}
			LUA_CHECK_ERROR(lua_isstring(l, pos) || lua_istable(l, pos), LUA_TSTRING, pos);
			lua_getglobal(l, "get_i64");
			const char* str;
			if (lua_isfunction(l, -1))
			{
				lua_pushvalue(l, pos);
				lua_pcall(l, 1, 2, 0);
				bool unsignedflag;
				str = lua_tostring(l, -2);
				unsignedflag = lua_toboolean(l, -1);
				assert(!unsignedflag);
			}
			else
			{
				lua_pop(l, 1);
				size_t len = 0;
				str = luaL_checklstring(l, pos, &len);
			}
			std::istringstream iss(str);
			int64 num;
			iss >> num;
			*value = (::int64)num;
		}
		static bool try_get(lua_State * l, int pos, ::int64* value)
		{
			if (lua_isnoneornil(l, pos) || lua_isstring(l, pos) || lua_istable(l, pos))
			{
				from_stack(l, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t<::uint64>
	{
		static int push_stack(lua_State* l, ::uint64 value)
		{
			std::stringstream ss;
			ss << value;
			std::string str = ss.str();

			lua_getglobal(l, "make_i64");
			if (lua_isfunction(l, -1))
			{
				lua_pushlstring(l, str.c_str(), str.length());
				lua_pushboolean(l, true);
				lua_pcall(l, 2, 1, 0);
			}
			else
			{
				lua_pop(l, 1);
				lua_pushlstring(l, str.c_str(), str.length());
			}

			return 1;
		}
		static void from_stack(lua_State* l, int pos, ::uint64* value)
		{
			if (lua_isnoneornil(l, pos))
			{
				return;
			}
			LUA_CHECK_ERROR(lua_isstring(l, pos) || lua_istable(l, pos), LUA_TSTRING, pos);
			lua_getglobal(l, "get_i64");
			std::string str;
			if (lua_isfunction(l, -1))
			{
				lua_pushvalue(l, pos);
				lua_pcall(l, 1, 2, 0);
				bool unsignedflag;
				str = lua_tostring(l, -2);
				unsignedflag = lua_toboolean(l, -1);
				assert(unsignedflag);
			}
			else
			{
				lua_pop(l, 1);
				size_t len = 0;
				str = luaL_checklstring(l, pos, &len);
			}

			std::istringstream iss(str.c_str());
			uint64 num;
			iss >> num;
			*value = (::uint64)num;
		}
		static bool try_get(lua_State * l, int pos, ::uint64* value)
		{
			if (lua_isnoneornil(l, pos) || lua_isstring(l, pos) || lua_istable(l, pos))
			{
				from_stack(l, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t<std::string>
	{
		static int push_stack(lua_State* l, const std::string& value)
		{
			lua_pushstring(l, value.c_str());
			return 1;
		}
		static void from_stack(lua_State* l, int pos, std::string* value)
		{
			if (lua_isnoneornil(l, pos))
			{
				return;
			}

			LUA_CHECK_ERROR(0 != lua_isstring(l, pos), LUA_TSTRING, pos);
			const char* str = luaL_checkstring(l, pos);
			*value = str;
		}
		static bool try_get(lua_State * l, int pos, std::string* value)
		{
			if (lua_isnoneornil(l, pos) || lua_isstring(l, pos))
			{
				from_stack(l, pos, value);
				return true;
			}
			else
				return false;
		}
	};
	
	template<>
	struct lua_op_t<const std::string>
	{
		static int push_stack(lua_State* l, const std::string& value)
		{
			return lua_op_t<std::string>::push_stack(l, value);
		}
		static void from_stack(lua_State* l, int pos, std::string* value)
		{
			lua_op_t<std::string>::from_stack(l, pos, value);
		}
		static bool try_get(lua_State * l, int pos, std::string* value)
		{
			return lua_op_t<std::string>::try_get(l, pos, value);
		}
	};

	template<>
	struct lua_op_t < void* >
	{
		static int push_stack(lua_State* l, void* value)
		{
			if (value)
				lua_pushlightuserdata(l, value);
			else
				lua_pushnil(l);
			return 1;
		}
		static void from_stack(lua_State* l, int pos, void** value)
		{
			if (lua_isnoneornil(l, pos))
			{
				*value = 0;
				return;
			}
			LUA_CHECK_ERROR(0 != lua_isuserdata(l, pos), LUA_TUSERDATA, pos);
			*value = lua_touserdata(l, pos);
		}
		static bool try_get(lua_State * l, int pos, void** value)
		{
			if (lua_isnoneornil(l, pos) || lua_isuserdata(l, pos))
			{
				from_stack(l, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t < lua_CFunction >
	{
		static int push_stack(lua_State* l, lua_CFunction value)
		{
			if (value)
				lua_pushcfunction(l, value);
			else
				lua_pushnil(l);
			return 1;
		}
		static void from_stack(lua_State* l, int pos, lua_CFunction* value)
		{
			if (lua_isnoneornil(l, pos))
			{
				*value = 0;
				return;
			}
			LUA_CHECK_ERROR(0 != lua_isfunction(l, pos), LUA_TFUNCTION, pos);
			*value = (lua_CFunction)lua_tocfunction(l, pos);
		}
		static bool try_get(lua_State * l, int pos, lua_CFunction* value)
		{
			if (lua_isnoneornil(l, pos) || lua_isfunction(l, pos))
			{
				from_stack(l, pos, value);
				return true;
			}
			else
				return false;
		}
	};
	
	template<typename T>
	struct lua_op_t < std::vector<T> >
	{
		static int push_stack(lua_State* l, const std::vector<T>& value)
		{
			lua_newtable(l);
			typename std::vector<T>::const_iterator it = value.begin();
			for (int i = 1; it != value.end(); ++it, ++i)
			{
				lua_op_t<int>::push_stack(l , i);
				lua_op_t<T>::push_stack(l, *it);
				lua_settable(l, -3);
			}
			return 1;
		}
		static void from_stack(lua_State* l, int pos, std::vector<T>*& value)
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
				T v = T();
				lua_op_t<T>::from_stack(l, -1, &v);
				value->push_back(v);
				lua_pop(l, 1);
			}
		}
		static bool try_get(lua_State * l, int pos, std::vector<T>*& value)
		{
			if (lua_isnoneornil(l, pos) || lua_istable(l, pos))
			{
				from_stack(l, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<typename T>
	struct lua_op_t < std::list<T> >
	{
		static int push_stack(lua_State* l, const std::list<T>& value)
		{
			lua_newtable(l);
			typename std::list<T>::const_iterator it = value.begin();
			for (int i = 1; it != value.end(); ++it, ++i)
			{
				lua_op_t<int>::push_stack(l, i);
				lua_op_t<T>::push_stack(l, *it);
				lua_settable(l, -3);
			}
			return 1;
		}
		static void from_stack(lua_State* l, int pos, std::list<T>*& value)
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
				T v = T();
				lua_op_t<T>::from_stack(l, -1, &v);
				value->push_back(v);
				lua_pop(l, 1);
			}
		}
		static bool try_get(lua_State * l, int pos, std::list<T>*& value)
		{
			if (lua_isnoneornil(l, pos) || lua_istable(l, pos))
			{
				from_stack(l, pos, value);
				return true;
			}
			else
				return false;
		}
	};
	
	template<typename T>
	struct lua_op_t < std::set<T> >
	{
		static int push_stack(lua_State* l, const std::set<T>& value)
		{
			lua_newtable(l);
			typename std::set<T>::const_iterator it = value.begin();
			for (int i = 1; it != value.end(); ++it, ++i)
			{
				lua_op_t<int>::push_stack(l, i);
				lua_op_t<T>::push_stack(l, *it);
				lua_settable(l, -3);
			}
			return 1;
		}

		static void from_stack(lua_State* l, int pos, std::set<T>*& value)
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
				T val = T();
				lua_op_t<T>::from_stack(l, -1, &val);
				value->insert(val);
				lua_pop(l, 1);
			}
		}
		static bool try_get(lua_State * l, int pos, std::set<T>*& value)
		{
			if (lua_isnoneornil(l, pos) || lua_istable(l, pos))
			{
				from_stack(l, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<typename K, typename V>
	struct lua_op_t < std::map<K, V> >
	{
		static int push_stack(lua_State* l, const std::map<K, V>& value)
		{
			lua_newtable(l);
			typename std::map<K, V>::const_iterator it = value.begin();
			for (; it != value.end(); ++it)
			{
				lua_op_t<K>::push_stack(l, it->first);
				lua_op_t<V>::push_stack(l, it->second);
				lua_settable(l, -3);
			}
			return 1;
		}

		static void from_stack(lua_State* l, int pos, std::map<K, V>*& value)
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
				K key = K();
				V val = V();
				lua_op_t<K>::from_stack(l, -2, &key);
				lua_op_t<V>::from_stack(l, -1, &val);
				value->insert(make_pair(key, val));
				lua_pop(l, 1);
			}
		}
		static bool try_get(lua_State * l, int pos, std::map<K, V>*& value)
		{
			if (lua_isnoneornil(l, pos) || lua_istable(l, pos))
			{
				from_stack(l, pos, value);
				return true;
			}
			else
				return false;
		}
	};
	/////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	inline int push(lua_State* l)
	{
		return 0;
	}

	template<typename T>
	inline int push(lua_State* l, const T& value)
	{
		return lua_op_t<T>::push_stack(l, value);
	}
#if SUPPORT_PARAMS
	template <typename T, typename ... Args>
	inline int push(lua_State* l, const T& value, const Args &... args)
	{
		return push(l, value) + push(l, args...);
	}
#endif
	template<typename T>
	inline void get(lua_State* l, int pos, T* value)
	{
		lua_op_t<T>::from_stack(l, pos, value);
	}
	template<typename T>
	inline bool tryget(lua_State* l, int pos, T* value)
	{
		return lua_op_t<T>::try_get(l, pos, value);
	}
#if SUPPORT_PARAMS
	template <typename T, typename ... Args>
	inline void get(lua_State* l, int pos, T* value, Args ... args)
	{
		get(l, pos, value);
		get(l, pos + 1, args...);
	}
#endif
	template<typename T>
	inline void pop(lua_State* l, T* value)
	{
		get(l, -1, value);
		lua_pop(l, -1);
	}
#if SUPPORT_PARAMS
	template <typename T, typename ... Args>
	inline void pop(lua_State* l, T* value, Args ... args)
	{
		pop(l, value);
		pop(l, args...);
	}
#endif

	inline bool pcall(lua_State* l, int nArgs, int nResults, int nErrorsRef = -1)
	{
		int oldTop = lua_gettop(l) - nArgs - 1;
		int errPos = 0;
		if (nErrorsRef != -1)
		{
			lua_rawgeti(l, LUA_REGISTRYINDEX, nErrorsRef);
			lua_insert(l, oldTop + 1);
			errPos = oldTop + 1;
		}
		if (lua_pcall(l, nArgs, nResults, errPos) == 0)
		{
			errPos ? lua_remove(l, errPos) : lua_settop(l, oldTop);
			return true;
		}
		else
		{
			errPos ? lua_remove(l, errPos) : lua_settop(l, oldTop);
			return false;
		}
	}
	//////////////////////////////////////////////////////////////
	struct lua_ref_t
	{
		lua_State* l;
		int ref_v;
		lua_ref_t() : l(NULL), ref_v(LUA_NOREF) {}
		lua_ref_t(lua_State* l_) : l(l_), ref_v(LUA_NOREF) {}
		lua_ref_t(lua_State* l_, int r) : l(l_), ref_v(r) {}
		//~lua_ref_t() { unref(); }
		operator int() { return ref_v; }
		void unref(){
			if (ref_v != LUA_NOREF && l)
				luaL_unref(l, LUA_REGISTRYINDEX, ref_v);
			ref_v = LUA_NOREF;
		}
	};

	struct lua_func_ref_t : lua_ref_t
	{
		lua_func_ref_t() : lua_ref_t() {}
		lua_func_ref_t(lua_State* l_) : lua_ref_t(l_) {}
		lua_func_ref_t(lua_State* l_, int r) : lua_ref_t(l_, r) {}
		lua_func_ref_t(lua_State* l_, const char* method) : lua_ref_t(l_, LUA_NOREF)
		{
			assert(method != NULL && strlen(method) >0);
			lua_getglobal(l, method);
			int top = lua_gettop(l);
			LUA_CHECK_ERROR(lua_isfunction(l, top) != 0, LUA_TFUNCTION, top);
			ref_v = luaL_ref(l, LUA_REGISTRYINDEX);
		}
#if SUPPORT_PARAMS
		template <typename ... Args>
		inline bool call(const Args &... args)
		{
			assert(l);

			lua_getglobal(l, "debug");
			lua_getfield(l, -1, "traceback");
			int tracebackFuncRef = luaL_ref(l, LUA_REGISTRYINDEX);
			lua_pop(l, 1);

			lua_rawgeti(l, (int)LUA_REGISTRYINDEX, this->ref_v);  //function
			if (!lua_isfunction(l, -1))
			{
				lua_pop(l, 1);
				luaL_error(l, "Error: %d is not a function\n", this->ref_v);
				luaL_unref(l, LUA_REGISTRYINDEX, tracebackFuncRef);
				return false;
			}

			bool success = pcall(l, push(l, args...), -1, tracebackFuncRef);
			luaL_unref(l, LUA_REGISTRYINDEX, tracebackFuncRef);
			return success;
		}
#endif
	};

	struct lua_table_ref_t : lua_ref_t
	{
		lua_table_ref_t() : lua_ref_t() {}
		lua_table_ref_t(lua_State* l_) : lua_ref_t(l_) {}
		lua_table_ref_t(lua_State* l_, int r) : lua_ref_t(l_, r) {}
		lua_table_ref_t(lua_State* l_, const char* name) : lua_ref_t(l_, LUA_NOREF)
		{
			create(l_, name);
		}
		inline void create(lua_State* l_, const char* name)
		{
			l = l_;
			if (!name)
			{
				lua_newtable(l);
				ref_v = luaL_ref(l, LUA_REGISTRYINDEX);
			}
			else
			{
				bool exist = true;
				lua_getglobal(l, name);
				if (lua_isnoneornil(l, -1))
				{
					exist = false;
					lua_newtable(l);
				}
				if (!exist)
					lua_setglobal(l, name);
				else
					lua_pop(l, 1);

				lua_getglobal(l, name);
				ref_v = luaL_ref(l, LUA_REGISTRYINDEX);
			}
		}
#if SUPPORT_PARAMS
		template <typename ... Args>
		inline bool call(const char* method, const Args &... args)
		{
			assert(l);

			lua_getglobal(l, "debug");
			lua_getfield(l, -1, "traceback");
			int tracebackFuncRef = luaL_ref(l, LUA_REGISTRYINDEX);
			lua_pop(l, 1);

			lua_rawgeti(l, (int)LUA_REGISTRYINDEX, this->ref_v); //t
			lua_getfield(l, -1, method);//t,func
			if (!lua_isfunction(l, -1))
			{
				luaL_unref(l, LUA_REGISTRYINDEX, tracebackFuncRef);
				lua_pop(l, 1);
				return false;
			}
			bool success = pcall(l, push(l, args...), -1, tracebackFuncRef);
			luaL_unref(l, LUA_REGISTRYINDEX, tracebackFuncRef);
			return success;
		}
#endif
		template<typename T>
		inline void get(const char* field, T* value, bool try_get = false) const
		{
			int oldTop = lua_gettop(l);
			lua_rawgeti(l, (int)LUA_REGISTRYINDEX, this->ref_v); //t
			lua::push(l, field);
			lua_gettable(l,-2);
			if (!try_get)
				lua::get(l, -1, value);
			else
				lua_op_t<T>::try_get(l, -1, value);
			lua_settop(l, oldTop);
		}
		template<typename T>
		inline void get(int index, T* value, bool try_get = false) const
		{
			int oldTop = lua_gettop(l);
			lua_rawgeti(l, (int)LUA_REGISTRYINDEX, this->ref_v); //t
			lua_rawgeti(l, -1, index);
			if (!try_get)
				lua::get(l, -1, value);
			else
				lua_op_t<T>::try_get(l, -1, value);
			lua_settop(l, oldTop);
		}
		template<typename T>
		inline T get(const char* field, bool try_get = false) const
		{
			T v = T();
			get(field, &v, try_get);
			return v;
		}
		template<typename T>
		inline T get(int index, bool try_get = false) const
		{
			T v = T();
			get(index, &v, try_get);
			return v;
		}

		inline bool has(const char* field)
		{
			int oldTop = lua_gettop(l);
			lua_rawgeti(l, (int)LUA_REGISTRYINDEX, this->ref_v); //t
			lua::push(l, field);
			lua_gettable(l, -2);
			bool b = lua_isnil(l, -1);
			lua_settop(l, oldTop);
			return !b;
		}

		template<typename T>
		inline void set(const char* field, const T& value)
		{
			int oldTop = lua_gettop(l);
			lua_rawgeti(l, (int)LUA_REGISTRYINDEX, this->ref_v); //t
			lua::push(l, field);
			lua::push(l, value);
			lua_settable(l,-3);
			lua_settop(l, oldTop);
		}
		template<typename T>
		inline void set(int index, const T& value)
		{
			int oldTop = lua_gettop(l);
			lua_rawgeti(l, (int)LUA_REGISTRYINDEX, this->ref_v); //t
			lua::push(l, value);
			lua_rawseti(l, -2, index);
			lua_settop(l, oldTop);
		}
		inline int length() const
		{
			int n = lua_gettop(l);
			lua_rawgeti(l, (int)LUA_REGISTRYINDEX, this->ref_v); //t
#ifdef LUA5_2
			int len = (int)lua_rawlen(l, -1);
#else
			int len = (int)lua_objlen(l, -1);
#endif
			lua_settop(l, n);
			return len;
		}
	};

	template<>
	struct lua_op_t<lua_ref_t>
	{
		static int push_stack(lua_State* l, const lua_ref_t& value)
		{
			lua_rawgeti(l, LUA_REGISTRYINDEX, value.ref_v);
			return 1;
		}
		static void from_stack(lua_State* l, int pos, lua_ref_t* value)
		{
			lua_pushvalue(l, pos);
			int ref_v = luaL_ref(l, LUA_REGISTRYINDEX);
			(*value).ref_v = ref_v;
			(*value).l = l;
		}
		static bool try_get(lua_State * l, int pos, lua_ref_t* value)
		{
			from_stack(l, pos, value);
			return true;
		}
	};
	
	template<>
	struct lua_op_t<lua_func_ref_t>
	{
		static int push_stack(lua_State* l, const lua_func_ref_t& value)
		{
			lua_rawgeti(l, LUA_REGISTRYINDEX, value.ref_v);
			return 1;
		}
		static void from_stack(lua_State* l, int pos, lua_func_ref_t* value)
		{
			LUA_CHECK_ERROR(0 != lua_isfunction(l, pos), LUA_TFUNCTION, pos);
			lua_pushvalue(l, pos);
			int ref_v = luaL_ref(l, LUA_REGISTRYINDEX);
			(*value).ref_v = ref_v;
			(*value).l = l;
		}
		static bool try_get(lua_State * l, int pos, lua_func_ref_t* value)
		{
			if (lua_isfunction(l, pos))
			{
				from_stack(l, pos, value);
				return true;
			}
			else 
				return false;
		}
	};

	template<>
	struct lua_op_t<lua_table_ref_t>
	{
		static int push_stack(lua_State* l, const lua_table_ref_t& value)
		{
			lua_rawgeti(l, LUA_REGISTRYINDEX, value.ref_v);
			return 1;
		}
		static void from_stack(lua_State* l, int pos, lua_table_ref_t* value)
		{
			LUA_CHECK_ERROR(0 != lua_istable(l, pos), LUA_TTABLE, pos);
			lua_pushvalue(l, pos);
			int ref_v = luaL_ref(l, LUA_REGISTRYINDEX);
			(*value).ref_v = ref_v;
			(*value).l = l;
		}
		static bool try_get(lua_State * l, int pos, lua_table_ref_t* value)
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
	//////////////////////////////////////////////////////////////
#if SUPPORT_PARAMS
	template <typename ... Args>
	inline bool callFunction(lua_State* l, const char* method, const Args &... args)
	{
		assert(l);

		lua_getglobal(l, "debug");
		lua_getfield(l, -1, "traceback");
		int tracebackFuncRef = luaL_ref(l, LUA_REGISTRYINDEX);
		lua_pop(l, 1);

		lua_getglobal(l, method);
		if (!lua_isfunction(l, -1))
		{
			lua_pop(l, 1);
			luaL_unref(l, LUA_REGISTRYINDEX, tracebackFuncRef);
			return false;
		}

		bool success = pcall(l, push(l, args...), -1, tracebackFuncRef);
		luaL_unref(l, LUA_REGISTRYINDEX, tracebackFuncRef);
		return success;
	}

	template <typename ... Args>
	inline bool callFunction(lua_State* l, int refFunc, const Args &... args)
	{
		assert(l);

		lua_getglobal(l, "debug");
		lua_getfield(l, -1, "traceback");
		int tracebackFuncRef = luaL_ref(l, LUA_REGISTRYINDEX);
		lua_pop(l, 1);

		lua_rawgeti(l, (int)LUA_REGISTRYINDEX, refFunc);  //function
		if (!lua_isfunction(l, -1))
		{
			lua_pop(l, 1);
			luaL_error(l, "Error: %d is not a function\n", refFunc);
			luaL_unref(l, LUA_REGISTRYINDEX, tracebackFuncRef);
			return false;
		}

		bool success = pcall(l, push(l, args...), -1, tracebackFuncRef);
		luaL_unref(l, LUA_REGISTRYINDEX, tracebackFuncRef);
		return success;
	}

	template <typename ... Args>
	inline bool callTableFunction(lua_State* l, const char* module, const char* method, const Args &... args)
	{
		assert(l);

		lua_getglobal(l, "debug");
		lua_getfield(l, -1, "traceback");
		int tracebackFuncRef = luaL_ref(l, LUA_REGISTRYINDEX);
		lua_pop(l, 1);

		lua_getglobal(l, module);	//--> t
		if (!lua_istable(l, -1))
		{
			luaL_unref(l, LUA_REGISTRYINDEX, tracebackFuncRef);
			lua_pop(l, 1);
			return false;
		}

		lua_getfield(l, -1, method);//t,func
		if (lua_isnoneornil(l, -1))
		{
			luaL_unref(l, LUA_REGISTRYINDEX, tracebackFuncRef);
			lua_pop(l, 1);
			return false;
		}

		bool success = pcall(l, push(l, args...), -1, tracebackFuncRef);
		luaL_unref(l, LUA_REGISTRYINDEX, tracebackFuncRef);
		return success;
	}

	template <typename ... Args>
	inline bool callTableFunction(lua_State* l, int refTable, const char* method, const Args &... args)
	{
		assert(l);

		lua_getglobal(l, "debug");
		lua_getfield(l, -1, "traceback");
		int tracebackFuncRef = luaL_ref(l, LUA_REGISTRYINDEX);
		lua_pop(l, 1);

		lua_rawgeti(l, (int)LUA_REGISTRYINDEX, refTable); //t
		lua_getfield(l, -1, method);//t,func
		if (lua_isnoneornil(l, -1))
		{
			luaL_unref(l, LUA_REGISTRYINDEX, tracebackFuncRef);
			lua_pop(l, 1);
			return false;
		}
	
		bool success = pcall(l, push(l, args...), -1, tracebackFuncRef);
		luaL_unref(l, LUA_REGISTRYINDEX, tracebackFuncRef);
		return success;
	}
#endif

#if SUPPORT_PARAMS
	template <typename ... Args>
	inline bool runFile(lua_State* l, const char* filename, const Args &... args)
#else
	inline bool runFile(lua_State* l, const char* filename)
#endif
	{
		assert(l);
		lua_getglobal(l, "debug");
		lua_getfield(l, -1, "traceback");
		int tracebackFuncRef = luaL_ref(l, LUA_REGISTRYINDEX);
		lua_pop(l, 1);

		if (0 != luaL_loadfile(l, filename))
		{
			luaL_unref(l, LUA_REGISTRYINDEX, tracebackFuncRef);
			lua_pop(l, 1);
			return false;
		}

#if SUPPORT_PARAMS
		int n = push(l, args...);
#else
		int n = 0;
#endif
		bool success = pcall(l, n, -1, tracebackFuncRef);
		luaL_unref(l, LUA_REGISTRYINDEX, tracebackFuncRef);
		return success;
	}

#if SUPPORT_PARAMS
	template <typename ... Args>
	inline bool runString(lua_State* l, const char* buff, const Args &... args)
#else
	inline bool runString(lua_State* l, const char* buff)
#endif
	{
		assert(l);
		lua_getglobal(l, "debug");
		lua_getfield(l, -1, "traceback");
		int tracebackFuncRef = luaL_ref(l, LUA_REGISTRYINDEX);
		lua_pop(l, 1);

		if (0 != luaL_loadstring(l, buff))
		{
			luaL_unref(l, LUA_REGISTRYINDEX, tracebackFuncRef);
			lua_pop(l, 1);
			return false;
		}

#if SUPPORT_PARAMS
		int n = push(l, args...);
#else
		int n = 0;
#endif
		bool success = pcall(l, n, -1, tracebackFuncRef);
		luaL_unref(l, LUA_REGISTRYINDEX, tracebackFuncRef);
		return success;
	}

	//create table and put it at top
	inline void createtable(lua_State* l, const char* name)
	{
		assert(name);

		lua_getglobal(l, "_G");
		std::string tn;
		for (size_t i = 0; i < strlen(name); ++i)
		{
			if (name[i] == '.')
			{
				if (tn.empty()) continue;
				lua_pushstring(l, tn.c_str());
				lua_rawget(l, -2);
				if (lua_isnil(l, -1))
				{
					lua_pop(l, 1);
					lua_createtable(l, 0, 0);
					lua_pushstring(l, tn.c_str());
					lua_pushvalue(l, -2);
					lua_rawset(l, -4);
				}
				lua_remove(l, -2);
				tn.clear();
			}
			else
			{
				tn += name[i];
			}
		}
		if (!tn.empty())
		{
			lua_pushstring(l, tn.c_str());//g,name
			lua_rawget(l, -2);//g,t
			if (lua_isnil(l, -1))
			{
				lua_pop(l, 1);//g
				lua_createtable(l, 0, 0);//g,t
				lua_pushstring(l, tn.c_str());//g,t,name
				lua_pushvalue(l, -2);//g,t,name,t
				lua_rawset(l, -4);//g,t
			}
			lua_remove(l, -2);//t
		}
	}
	
	//get table and put it at top
	inline void gettable(lua_State* l, const char* name)
	{
		assert(name);
		lua_getglobal(l, "_G");//g
		std::string tn;
		for (size_t i = 0; i < strlen(name); ++i)
		{
			if (name[i] == '.')
			{
				if (tn.empty()) continue;
				lua_pushstring(l, tn.c_str());
				lua_rawget(l, -2);
				//if (lua_isnil(l, -1))
				//{
				//	lua_pop(l, 1);
				//}
				lua_remove(l, -2);
				tn.clear();
			}
			else
			{
				tn += name[i];
			}
		}
		if (!tn.empty())
		{
			lua_pushstring(l, tn.c_str());//g,name
			lua_rawget(l, -2);//g,t
			//if (lua_isnil(l, -1))
			//{
			//	lua_pop(l, 1);//g
			//}
			lua_remove(l, -2);//
		}
	}
}



#endif//_LUA_SCRIPT_HPP__
