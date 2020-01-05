#define LUA_LIB
#include "UIlib.h"
#include "LuaApplication.h"
#include "base/lua_wrapper.hpp"
#include "lua_dui_wrapper.hpp"
#include "base/LuaEnv.hpp"
#include <algorithm>
#ifdef USE_CUSTOM_MEMORY
#define new FLIB_NEW
#endif
LuaEnv* globalLuaEnv = NULL;

#ifdef LUA_OBJECT_EXTERN
namespace lua
{
	static LuaObjectContainer lua_hash_objs_container;

	LuaObjectContainer& get_luaobj_container()
	{
		return lua_hash_objs_container;
	}
}
#endif//LUA_OBJECT_EXTERN

namespace DuiLib
{
#define TO_DUISTRING(s) toDuiString
#define TO_STDSTRING(s) toStdString

	CDuiString toDuiString(const char* arg)
	{
		CDuiString s_out;
#ifdef _UNICODE
		std::wstring puni = Convert::UTF8ToUnicode(arg, strlen(arg));
		s_out.Assign((LPCTSTR)puni.c_str(), puni.size());
#else
		std::string pmb = Convert::UTF8ToMB(arg, strlen(arg));
		s_out.Assign((LPCTSTR)pmb.c_str(), pmb.size());
#endif
		return s_out;
	}

	std::string toStdString(const LPCTSTR str)
	{
		std::string s_out;
#ifdef _UNICODE
		s_out = DuiLib::Convert::UnicodeToUTF8(str);
#else
		s_out = str;
#endif
		return s_out;
	}

	class DuiObjectHandler : public IObjectHandle
	{
	public:
		virtual void OnConstructor(void*)
		{}
		virtual void OnDeconstructor(void* p)
		{
			if (globalLuaEnv)
				lua::removeobject(p, *globalLuaEnv);
			else
				lua::removeobject(p, NULL);
		}
	};

	struct lua_load_tool
	{
		inline static void release_buffer(BYTE* pBuffer)
		{
			CResourceManager::Instance()->FreeAsset(pBuffer);
		}

		inline static bool load_buffer(const char* filename, BYTE** ppBuffer, unsigned int* size)
		{
			CDuiString szFile = toDuiString(filename);
			CDuiString szFileName(_T("lua/"));
			szFileName += szFile;
			DWORD sz = 0;
			if (!CResourceManager::Instance()->LoadAsset(szFileName, _T("LUA"), ppBuffer, &sz))
			{
				//DuiLogError(_T("(lua_load_tool.load_buffer)Failed Load Lua Script %s"), szFile.GetData());
				return false;
			}
			*size = (unsigned int)sz;
			return true;
		}

		inline static bool load_file(const char* filename, BYTE** ppBuffer, unsigned int* size)
		{
			HANDLE hFile = ::CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE) return false;
			DWORD dwSize = ::GetFileSize(hFile, NULL);
			if (dwSize == 0) return false;
			if (dwSize > 4096 * 1024) return false;

			DWORD dwRead = 0;
			BYTE* pByte = new BYTE[dwSize];
			::ReadFile(hFile, pByte, dwSize, &dwRead, NULL);
			::CloseHandle(hFile);

			*ppBuffer = pByte;
			*size = dwRead;
			return true;
		}

		inline static void release_file(BYTE* pBuffer)
		{
			if (pBuffer)
			{
				delete[] pBuffer;
			}
		}
	};

	void on_lua_print(int level, const char* message)
	{
		CDuiString s = toDuiString(message);
		switch (level)
		{
		case 3:
			DuiLogError(s.GetData());
			MessageBox(NULL, s.GetData(), _T("Error"), 0);
			DuiException(s.GetData());
			break;
		case 2: //error
			DuiLogError(s.GetData());
			//MessageBox(NULL, s.GetData(), _T("Error"), 0);
			break;
		case 1:
			DuiLogWarning(s.GetData());
			break;
		default:
			DuiLogInfo(s.GetData());
			break;
		}
	}
	
	int on_lua_loader(lua_State* L)
	{
		int top = lua_gettop(L);
		std::string fileName = lua_tostring(L, 1);
		std::replace(fileName.begin(), fileName.end(), '.', '/');
		fileName += ".lua";

		BYTE* buffer = NULL;
		unsigned int nRead = 0;
		bool success = lua_load_tool::load_buffer(fileName.c_str(), &buffer, &nRead);
		if (!success)
		{
			lua_pop(L, 1);
			std::string err = "loader failed to read file with path: " + fileName;
			lua_pushstring(L, err.c_str());
			return 1;
		}
		std::string chunk = "@";
		chunk += fileName;
		if (luaL_loadbuffer(L, (char*)buffer, nRead, chunk.c_str()) != 0)
		{
			lua_load_tool::release_buffer(buffer);
			std::string err = lua_tostring(L, -1);
			lua_pushstring(L, err.c_str());
			return 1;
		}
		lua_load_tool::release_buffer(buffer);
		return 1;
	}

	int on_import_luadll(lua_State* L)
	{
		int n = lua_gettop(L);
		std::string fileName = lua_tostring(L, 1);
		CDuiString szDllName = toDuiString(fileName.c_str());
		bool b = LuaApplication::Instance()->ImportLuaDLL(szDllName, NULL);
		if (!b)
		{
			lua_error(L);
			return 1;
		}
		else
		{
			return lua_gettop(L) - n;
		}
	}

	///////////////////////////////////////////////////////////////
	////
	LuaApplication::LuaApplication(): valid(false)
	{
		Initialize();
	}
	LuaApplication::~LuaApplication()
	{
		Unitialize();
	}

	bool LuaApplication::Initialize()
	{
		HRESULT hr = ::OleInitialize(NULL);
		if (FAILED(hr))
		{
			DuiLogError(_T("Failed initialize Ole."));
			return false;
		}
		
		if (!StartupLua())
		{
			DuiLogError(_T("Failed StartupLua."));
			return false;
		}

		if (!RegisterScript())
		{
			DuiLogError(_T("Failed RegisterScript."));
			return false;
		}
		valid = true;
		return true;
	}

	void LuaApplication::Unitialize()
	{
		if (!valid) return;
		valid = false;
		cmd_lines.Empty();
		ShutdownLua();
		CPaintManagerUI::Term();
		::OleUninitialize();
		CDuiObjectMgr::Get().SetObjectHandler(NULL);
	}

	bool LuaApplication::StartupLua()
	{
		ShutdownLua();
		{
			auto lua_print = [](const char* log)
			{
				on_lua_print(0, log);
			};
			auto lua_warn = [](const char* log)
			{
				on_lua_print(1, log);
			};
			auto lua_error = [](const char* log)
			{
				on_lua_print(2, log);
			};
			auto lua_exception = [](const char* log)
			{
				on_lua_print(3, log);
			};

			LuaOutHandler::Get().SetPrintHandler(lua_print, 0);
			LuaOutHandler::Get().SetPrintHandler(lua_warn, 1);
			LuaOutHandler::Get().SetPrintHandler(lua_error, 2);
			LuaOutHandler::Get().SetPrintHandler(lua_exception, 3);
		}

		globalLuaEnv = new LuaEnv(true);
		globalLuaEnv->SetSearcher(on_lua_loader);
		{
			lua::stack_gurad guard(*globalLuaEnv);
			lua_pushcfunction(*globalLuaEnv, on_import_luadll);
			lua_setfield(*globalLuaEnv, LUA_GLOBALSINDEX, "importdll");
			
			lua_pushboolean(*globalLuaEnv, false);
			lua_setfield(*globalLuaEnv, LUA_GLOBALSINDEX, "CLOSING");

			lua_pushboolean(*globalLuaEnv, true);
			lua_setfield(*globalLuaEnv, LUA_GLOBALSINDEX, "INIT");
		}
		return true;
	}
	bool LuaApplication::ShutdownLua()
	{
		if (globalLuaEnv != NULL)
		{
			lua_pushboolean(*globalLuaEnv, true);
			lua_setfield(*globalLuaEnv, LUA_GLOBALSINDEX, "CLOSING");
	
			delete globalLuaEnv;
			globalLuaEnv = NULL;

			for (int i = 0; i < lua_plugins.GetSize(); ++i)
			{
				const LPCTSTR key = lua_plugins.GetAt(i);
				UnloadLuaDLL(key);
			}

			lua_plugins.Empty();
		}
		return true;
	}

	extern bool register_lua_scripts(lua_State*);
	bool LuaApplication::RegisterScript()
	{
		return register_lua_scripts(*globalLuaEnv);
	}
	bool LuaApplication::ImportLuaDLL(LPCTSTR dllName, LPCTSTR dllEntry)
	{
		if (lua_plugins.Find(dllName) != NULL)
		{
			return true;
		}

		CDuiString szDllName = dllName;
		CDuiString szDllNameWithExt = szDllName + _T(".dll");
		CDuiString szEntryFunc(_T("luaopen_")); szEntryFunc += szDllName;
		if (dllEntry != NULL) szEntryFunc = dllEntry;

		HPLUGINS hDll = CSystem::Instance()->LoadPlugin(szDllNameWithExt);
		if (hDll)
		{
			typedef int(*LuaOpenEntryFunc)(lua_State *);
			LuaOpenEntryFunc lpfunc = CSystem::Instance()->GetFunction<LuaOpenEntryFunc>(hDll, szEntryFunc);
			if (!lpfunc)
			{
				CSystem::Instance()->FreePlugin(hDll);
				DuiLogError(_T("Failed to GetFunction : %s@%s"), szDllName, szEntryFunc);
				return false;
			}
			lpfunc(*globalLuaEnv);
			lua_plugins.Insert(dllName, hDll);
			return true;
		}
		else
		{
			DuiLogError(_T("Failed to Load Dll : %s"), szDllName);
			return false;
		}
	}
	bool LuaApplication::UnloadLuaDLL(LPCTSTR dllName)
	{
		HPLUGINS hDll = (HPLUGINS)lua_plugins.Find(dllName);
		if (hDll == NULL)
		{
			return true;
		}
		CDuiString szDllName = dllName;
		CDuiString szDllNameWithExt = szDllName + _T(".dll");

		CDuiString szEntryFunc(_T("unload_")); szEntryFunc += dllName;
		typedef void(*LuaUnloadEntryFunc)(lua_State *);
		LuaUnloadEntryFunc lpfunc = CSystem::Instance()->GetFunction<LuaUnloadEntryFunc>(hDll, szEntryFunc);
		if (lpfunc)
		{
			lpfunc(NULL);
		}
		CSystem::Instance()->FreePlugin(szDllNameWithExt);
		lua_plugins.Set(dllName, NULL);	
		return true;
	}

	static DuiObjectHandler _obj_handler;

	bool LuaApplication::RunInternal(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR lpCmdLine, int nCmdShow)
	{		
		if (!valid)
		{
			MessageBox(NULL, _T("Failed to Initialize"), _T("Error"), 0);
			return false;
		}

		CDuiObjectMgr::Get().SetObjectHandler(&_obj_handler);
		CPaintManagerUI::SetInstance(hInstance);

		CDuiString szEntryFile = Path::CombinePath(CPaintManagerUI::GetInstancePath(), "entry.lua");
		std::string entryfile = toStdString(szEntryFile.GetData());
		BYTE* buffer = NULL;
		unsigned int dwSize = 0;
		bool success = lua_load_tool::load_file(entryfile.c_str(), &buffer, &dwSize);
		if (!success)
		{
			CDuiString error = _T("Failed to Initialize");
			error += szEntryFile;
			MessageBox(NULL, error.GetData(), _T("Error"), 0);
			return false;
		}

		std::string chunk = "@entry.lua";
		if (0 != luaL_loadbuffer(*globalLuaEnv, (char*)buffer, dwSize, chunk.c_str()))
		{
			lua_load_tool::release_file(buffer);

			const char* err = lua_tostring(*globalLuaEnv, -1);
			DuiLogError("%s", err);
			lua_pop(*globalLuaEnv, 1);
			CDuiString s = toDuiString(err);
			MessageBox(NULL, s.GetData(), _T("Error"), 0);
			return false;
		}
		lua_load_tool::release_file(buffer);

		success = true;
		if (globalLuaEnv->doCall(0, 0))
		{
			if (!globalLuaEnv->doGlobal("main", lpCmdLine))
			{
				success = false;
			}
		}
		else
		{
			success = false;
		}

		if (!success)
		{
			const char* err = lua_tostring(*globalLuaEnv, -1);
			if (err)
			{
				CDuiString s = toDuiString(err);
				MessageBox(NULL, s.GetData(), _T("Error"), 0);
			}
		}
		else
		{
			DuiLib::CPaintManagerUI::MessageLoop();
		}

		globalLuaEnv->doGlobal("exit", success);

		return success;
	}

	bool LuaApplication::Run(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
	{
		bool b = RunInternal(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
		Unitialize();
		return b;
	}
}

extern "C"
{
	int LUALIB_API RunByLua(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
	{
		return DuiLib::LuaApplication::Instance()->Run(hInstance, hPrevInstance, lpCmdLine, nCmdShow) ? 0 : 1;
	}
}