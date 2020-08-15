#define LUA_LIB
#include <lua.hpp>
#include <assert.h>
#include "luaext.h"
#include "MainThreadTask/MainThreadTask.h"

lua_State* gL = nullptr;

namespace lua
{
	bool LuaWrapper::pcall(int nArgs, int nResults)
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
			return false;
		}
	}

	LuaWrapper::LuaWrapper(lua_State* L): m_L(L), m_errRef(-1)
	{
		assert(m_L);
		lua_pushcfunction(m_L, error_traceback);
		m_errRef = luaL_ref(m_L, LUA_REGISTRYINDEX);
	}
	LuaWrapper::~LuaWrapper()
	{
		if (m_L && m_errRef != -1)
		{
			luaL_unref(m_L, LUA_REGISTRYINDEX, m_errRef);
			m_errRef = -1;
		}
		m_L = nullptr;
	}
	int LuaWrapper::error_traceback(lua_State* l)
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

	void tryGetUserdataFromWeakTable(lua_State* L, void* Obj)
	{
		lua_getglobal(L, "g_udref"); //ref
		if (lua_isnil(L, -1))
		{
			lua_pop(L, 1);	//
			lua_newtable(L); //ref
			lua_newtable(L); //ref,mt
			lua_pushstring(L, "v"); //ref,mt,"v"
			lua_setfield(L, -2, "__mode"); //ref,mt
			lua_setmetatable(L, -2); //ref
			lua_pushvalue(L, -1); //ref,ref
			lua_setglobal(L, "g_udref"); //ref
		}
		lua_pushlightuserdata(L, Obj); //ref,obj
		lua_gettable(L, -2); //ref,ud
	}

	void removeUserdataFromWeakTable(lua_State* L, void* Obj)
	{
		if (!Obj) return;
		lua_getglobal(L, "g_udref"); //ref
		if (!lua_isnil(L, -1))
		{
			lua_pushlightuserdata(L, Obj);//ref, obj
			lua_pushnil(L);//ref, obj, nil
			lua_settable(L, -3);
		}
		lua_pop(L, 1);
	}

	int push_to_lua(lua_State* L, void* obj, const char* mtname)
	{
		if (!obj)
		{
			lua_pushnil(L);
			return 1;
		}
		tryGetUserdataFromWeakTable(L, obj);
		if (lua_isnil(L, -1))
		{
			lua_pop(L, 1); //t
			if (mtname)
			{
				LuaUserData* userdata = (LuaUserData*)lua_newuserdata(L, sizeof(LuaUserData)); //t,ud
				userdata->object = obj;
				userdata->flag = 1;
				userdata->stamp = userdata;
				luaL_getmetatable(L, mtname);//t,ud,mt
				lua_setmetatable(L, -2);//t,ud
				lua_pushlightuserdata(L, obj); //t,ud,obj
				lua_pushvalue(L, -2); //t,ud,obj,ud
				lua_settable(L, -4); //t,ud
			}
			else
			{
				lua_pushlightuserdata(L, obj);
			}
		}
		else
		{
			LuaUserData* userdata = (LuaUserData*)lua_touserdata(L, -1);
			userdata->object = obj;
			//assert(userdata->flag == 1);
			userdata->flag = 1;
		}

		lua_remove(L, -2); //ud
		return 1;
	}
}


extern int lua_register_Database(lua_State* L);
extern int lua_register_PlayCenter(lua_State* L);
extern int lua_register_AudioTagInfo(lua_State* L);
extern int luaopen_lfs(lua_State* L);
extern int lua_register_encoding(lua_State* L);

extern "C"
{
	static int lua_MainThreadTask_tick(lua_State* L) 
	{
		MainThreadTaskManager::instance().tick();
		return 0;
	}

	LUALIB_API int luaopen_luaext(lua_State* L)
	{
		gL = L;

		lua_register_Database(L);
		lua_register_PlayCenter(L);
		lua_register_AudioTagInfo(L);
		luaopen_lfs(L);
		lua_register_encoding(L);

		{
			int nTop = lua_gettop(L);
			lua_createtable(L, 0, 0);
			lua_pushstring(L, "tick");
			lua_pushcfunction(L, lua_MainThreadTask_tick);
			lua_rawset(L, -3);
			lua_setglobal(L, "MainThreadTask");
			lua_settop(L, nTop);
		}

		return 1;
	}
	LUALIB_API void unload_luaext(lua_State* L)
	{
		gL = NULL;
	}
};