#ifndef lua_duilib_h
#define lua_duilib_h
#pragma once
#include "base/LuaEnv.hpp"
namespace DuiLib
{
	class LuaDuiLib
	{
	public:
		static bool Run(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow);
		static bool RegisterDuiLib(lua_State* l);
	private:
		static bool m_registered;
	};
}


#endif//lua_duilib_h