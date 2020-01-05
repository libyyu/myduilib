#ifndef luaext_h
#define luaext_h
#pragma once

namespace lua
{
	struct LuaUserData
	{
		LuaUserData* stamp;
		void* object;
		int	flag;
	};

	void tryGetUserdataFromWeakTable(lua_State* L, void* Obj);

	void removeUserdataFromWeakTable(lua_State* L, void* Obj);

	int push_to_lua(lua_State* L, void* obj, const char* mtname = NULL);

	template<class T>
	T* get_from_lua(lua_State* L, int pos, const char* mtname)
	{
		LuaUserData* userdata = (LuaUserData*)luaL_checkudata(L, pos, mtname);
		if (userdata && userdata->object)
		{
			return static_cast<T*>(userdata->object);
		}
		else
		{
			return NULL;
		}
	}

	struct LuaWrapper
	{
		int m_errRef;
		lua_State* m_L;
	public:
		bool pcall(int nArgs, int nResults = -1);

		LuaWrapper(lua_State* L);
		~LuaWrapper();
	protected:
		static int error_traceback(lua_State* L);
	};
}

#endif//luaext_h