#include <iostream>
#include <tchar.h>
#include "base/console.hpp"
#ifdef _WIN32
#include<Windows.h>
#endif
#define LUA_LIB
#include "proto_interface_lua.hpp"

__cfunc_begin
extern int luaopen_protos(lua_State *L);
__cfunc_end


char* get_dir(const char* path)
{
	const int len = 256;
	char s_dir[len];
	s_dir[0] = 0x0;
	char s_drive[10];
	s_drive[0] = 0x0;
	static char s_path[len];
	s_path[0] = 0x0;
	_tsplitpath(path, s_drive, s_dir, NULL, NULL);
	int n = strlen(s_dir) - 1;
	if (s_dir[n] == '\\' || s_dir[n] == '/')
		s_dir[n] = '\0';
	sprintf(s_path, "%s%s", s_drive, s_dir);
	return s_path;
}

int panic(lua_State* l)
{
	std::string reason;
	reason += "unprotected error in call to Lua API (";
	const char* s = lua_tostring(l, -1);
	reason += s;
	reason += ")";

#ifdef _WIN32
	MessageBox(NULL, reason.c_str(), "Exception", MB_ICONERROR);
	OutputDebugStringA(reason.c_str());
#endif

	throw reason;
	return 0;
}
int print(lua_State* l)
{
	std::string s;
	int n = lua_gettop(l);
	lua_getglobal(l, "tostring");

	for (int i = 1; i <= n; ++i)
	{
		lua_pushvalue(l, -1); // function to be called
		lua_pushvalue(l, i);  // value to print
		lua_call(l, 1, 1);

		const char* ret = lua_tostring(l, -1);
		if (!ret)
		{
			//log error
		}
		else
		{
			s.append(ret);
		}
		if (i < n)
		{
			s.append("\t");
		}
		else
		{
			s.append("\n");
		}
		lua_pop(l, 1); //pop result
	}

	Console::Log("[LUA]%s", s.c_str());

	return 0;
}

void test()
{
	char* this_path = get_dir(__FILE__);
	std::cout << "this path is " << this_path << std::endl;

	/*char s_env[256];
	s_env[0] = 0x0;
	sprintf(s_env, ("cd %s"), this_path);
	system(s_env);*/

	char s_protopath[256];
	s_protopath[0] = 0x0;
	sprintf(s_protopath, ("%s"), this_path);

	char s_luafile[256];
	s_luafile[0] = 0x0;
	sprintf(s_luafile, ("%s\\main.lua"), this_path);

	lua_State* l = luaL_newstate();
	luaL_openlibs(l);
	lua_atpanic(l, panic);

	lua_pushcfunction(l, print);
	lua_setfield(l, LUA_GLOBALSINDEX, "print");

	luaopen_protos(l);

	bool success = lua::run(l, s_luafile, s_protopath);

	//const Message* msg = proto_lua::Instance()->GetMessage(s_protofile, "gp_create_role_days");
	//std::cout << msg->GetTypeName() << std::endl;
}
#ifdef _DEMO
int main(int argc, char** argv)
{
	test();
	system("pause");
	return 0;
}
#else
//BOOL APIENTRY DllMain(HANDLE hModule, DWORD  dwReason, LPVOID lpReserved)
//{	
//	return TRUE;
//}
#endif