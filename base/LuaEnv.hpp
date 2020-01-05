#ifndef _LUA_ENV_HPP
#define _LUA_ENV_HPP

#include "lua_script.hpp"
#include <string>
#include <assert.h>
#include <iostream>
#ifdef _WIN32
#include <direct.h>
#include <Windows.h>
#else
#include <unistd.h>
#include <dirent.h>
#endif

typedef void (*PLuaOutFunc) (const char* message);

class LuaOutHandler
{
    PLuaOutFunc m_print_func = nullptr;
    PLuaOutFunc m_warn_func = nullptr;
    PLuaOutFunc m_error_func = nullptr;
	PLuaOutFunc m_exception_func = nullptr;
public:
    void SetPrintHandler(PLuaOutFunc pfunc, int flag)
    {
        if(flag == 0)
            m_print_func = (PLuaOutFunc)pfunc;
        else if(flag == 1)
            m_warn_func = (PLuaOutFunc)pfunc;
        else if(flag == 2)
            m_error_func = (PLuaOutFunc)pfunc;
		else
			m_exception_func = (PLuaOutFunc)pfunc;
    }
    
    void Stdout(const char* message, bool warning)
    {
        if(warning){
            if(m_warn_func) m_warn_func(message);
            else std::cout << message << std::endl;
        }
        else {
            if(m_print_func) m_print_func(message);
            else std::cout << message << std::endl;
        }
    }
    void Stderr(const char* message)
    {
        if(m_error_func) m_error_func(message);
        else std::cerr << message << std::endl;
    }

	void Exception(const char* message)
	{
		if(m_exception_func) m_exception_func(message);
		else throw message;
	}
    
    static LuaOutHandler& Get()
    {
        static LuaOutHandler handler;
        return handler;
    }
};

class LuaEnv
{
public:
	LuaEnv(bool open_ = true)
    : m_L(nullptr)
    , m_errRef(-1)
	{
		if (open_) open();
	}
	~LuaEnv()
	{
		close();
	}
	LuaEnv(lua_State* L)
	: m_L(nullptr)
    , m_errRef(-1)
	{
		close();
		m_L = L;
		luaL_openlibs(m_L);
		lua_atpanic(m_L, panic);

		/*lua_pushcfunction(m_L, print);
		lua_setfield(m_L, LUA_GLOBALSINDEX, "print");

		lua_pushcfunction(m_L, warn);
		lua_setfield(m_L, LUA_GLOBALSINDEX, "warn");*/
		lua_pushcfunction(m_L, error_traceback);
		m_errRef = luaL_ref(m_L, LUA_REGISTRYINDEX);
	}
	bool open()
	{
		close();
		m_L = luaL_newstate();
		luaL_openlibs(m_L);
		lua_atpanic(m_L, panic);

		lua_pushcfunction(m_L, print);
		lua_setfield(m_L, LUA_GLOBALSINDEX, "print");

		lua_pushcfunction(m_L, warn);
		lua_setfield(m_L, LUA_GLOBALSINDEX, "warn");

		lua_pushcfunction(m_L, error_traceback);
		m_errRef = luaL_ref(m_L, LUA_REGISTRYINDEX);

		return true;
	}
	void close()
	{
		if (m_L)
		{
			luaL_unref(m_L, LUA_REGISTRYINDEX, m_errRef);
			m_errRef = -1;
			lua_close(m_L);
		}
		m_L = nullptr;
	}
	operator lua_State* () { return m_L; }
	operator bool() { return m_L != nullptr; }
public:
	std::string getTraceback(const char* err = "Lua traceback:")
	{
		lua_State* l = m_L;
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

	void SetSearcher(lua_CFunction loader)
	{
		lua_State* L = m_L;
		int top = lua_gettop(L);

		lua_pushcfunction(L, loader);
		int loaderFunc = lua_gettop(L);

		lua_getfield(L, LUA_GLOBALSINDEX, "package");
		lua_getfield(L, -1, "loaders");
		int loaderTable = lua_gettop(L);

		for (size_t e = lua_objlen(L, loaderTable) + 1; e > 1; e--)
		{
			lua_rawgeti(L, loaderTable, int(e - 1));
			lua_rawseti(L, loaderTable, int(e));
		}
		lua_pushvalue(L, loaderFunc);
		lua_rawseti(L, loaderTable, 1);

		lua_settop(L, top);
	}
    
	bool doCall(int nArgs, int nResults = -1)
	{
		lua_State* L = m_L;
		assert(m_L != nullptr && "lua vm is closed.");

		int oldTop = lua_gettop(L) - nArgs - 1;
		lua_rawgeti(L, LUA_REGISTRYINDEX, m_errRef);
		lua_insert(L, oldTop + 1);
		if (lua_pcall(L, nArgs, nResults, oldTop + 1) == 0)
		{
			lua_remove(L, oldTop + 1);	//pop errorFunc
			return true;
		}
		else
		{
			lua_remove(L, oldTop + 1);	//pop errorFunc
			const char* errmsg = lua_tostring(L, -1);
			on_error_handler(errmsg);
			return false;
		}
	}

	template <typename ... Args>
	inline bool doString(const char* buff, const Args &... args)
	{
		lua_State* l = m_L;
		assert(m_L != nullptr && "lua vm is closed.");

		if (0 != luaL_loadstring(l, buff))
		{
			const char* errmsg = lua_tostring(l, -1);
			on_error_handler(errmsg);

			return false;
		}

		int n = lua::push(l, args...);
		return doCall(n, -1);
	}

	template <typename ... Args>
	inline bool doFile(const char* file, const Args &... args)
	{
		lua_State* l = m_L;
		assert(m_L != nullptr && "lua vm is closed.");

		if (0 != luaL_loadfile(l, file))
		{
			const char* errmsg = lua_tostring(l, -1);
			on_error_handler(errmsg);

			return false;
		}

		int n = lua::push(l, args...);
		return doCall(n, -1);
	}

	template <typename ... Args>
	inline bool doGlobal(const char* method, const Args &... args)
	{
		lua_State* l = m_L;
		assert(m_L != nullptr && "lua vm is closed.");

		lua_getglobal(l, method);
		if (!lua_isfunction(l, -1))
		{
			lua_pop(l, 1);
			return false;
		}
		int n = lua::push(l, args...);
		return doCall(n, -1);
	}
	
	template <typename ... Args>
	inline bool doTableFunc(int refTable, const char* method, const Args &... args)
	{
		lua_State* l = m_L;
		assert(m_L != nullptr && "lua vm is closed.");

		lua_rawgeti(l, (int)LUA_REGISTRYINDEX, refTable); //t
		lua_getfield(l, -1, method);//t,func
		if (lua_isnil(l, -1))
		{
			lua_pop(l, 1);
			return false;
		}
		int n = lua::push(l, args...);
		return doCall(n, -1);
	}
	
	template <typename ... Args>
	inline bool doTableFunc(const char* module, const char* method, const Args &... args)
	{
		lua_State* L = m_L;
		assert(m_L != nullptr && "lua vm is closed.");
		int oldTop = lua_gettop(L);

		lua_getglobal(L, module);	//--> t
		if (!lua_istable(L, -1))
		{
			std::stringstream msg;
			msg << "Fail to find module:" << module;
			on_error_handler(msg.str().c_str());

			lua_settop(L, oldTop);
			return false;
		}
		lua_getfield(L, -1, method);	//--> t, func
		if (!lua_isfunction(L, -1))
		{
			std::stringstream msg;
			msg << "Fail to find function:" << method;
			on_error_handler(msg.str().c_str());

			lua_settop(L, oldTop);
			return false;
		}

		int n = lua::push(L, args...);
		return doCall(n, -1);
	}
protected:
	static int panic(lua_State* l)
	{
		std::string reason = "";
		reason += "unprotected error in call to Lua API (";
		const char* s = lua_tostring(l, -1);
		reason += s;
		reason += ")\n";

		LuaOutHandler::Get().Exception(reason.c_str());
		return 0;
	}
	static int print(lua_State* l)
	{
		std::string s = "[LUA]" + on_print_handler(l);
        LuaOutHandler::Get().Stdout(s.c_str(), false);
		return 0;
	}
	static int warn(lua_State* l)
	{
		std::string s = "[LUA][Warn]" + on_print_handler(l);
		LuaOutHandler::Get().Stdout(s.c_str(), true);
		return 0;
	}
	static int error_traceback(lua_State* l)
	{
		if (!lua_isstring(l, 1))
			return 1;
		lua_checkstack(l, 3);
		lua_getglobal(l, "debug"); //msg, debug
		lua_getfield(l, -1, "traceback"); //msg, debug,traceback
		lua_pushvalue(l, 1); //msg, debug,traceback, msg
		lua_pushnumber(l, 1); //msg, debug,traceback, msg, level
		lua_pcall(l, 2, 1, 0);//msg, debug, tracebackstring
	
		return 1;
	}
	static void on_error_handler(const char* msg)
	{
        if(msg) LuaOutHandler::Get().Stderr(msg);
	}
private:
	static std::string on_print_handler(lua_State* l)
	{
		std::string s;
		int n = lua_gettop(l);
		lua_getglobal(l, "tostring");

		for (int i = 1; i <= n; ++i)
		{
			lua_pushvalue(l, -1); // function to be called
			lua_pushvalue(l, i);  // value to print
			lua_pcall(l, 1, 1, 0);

			const char* ret = lua_tostring(l, -1);
			if(ret)
				s.append(ret);
			
			if (i < n)
				s.append("\t");

			lua_pop(l, 1); //pop result
		}

		return s;
	}
private:
	lua_State* m_L;
	int        m_errRef;
};


#endif//_LUA_ENV_H
