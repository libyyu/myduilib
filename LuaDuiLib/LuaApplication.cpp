#define LUA_LIB
#include "UIlib.h"
#include "LuaApplication.h"
#include "lua_duilib_wrapper.hpp"
#include "lua_duilib_extend_wrapper.hpp"
#include "base/LuaEnv.hpp"
#include <algorithm>
#include <Shlobj.h>
#pragma  comment(lib, "Comctl32.lib")

#ifdef USE_CUSTOM_MEMORY
#define new FLIB_NEW
#endif
LuaEnv* globalLuaEnv = NULL;

class LuaObjectGC : public IObjectGC
{
public:
	virtual void OnGC(void* obj)
	{
		if (globalLuaEnv) {
			lua::remove_object_from_lua_global_table(*globalLuaEnv, obj);
			lua::remove_from_weaktable(*globalLuaEnv, obj);
		}
	}
};
static LuaObjectGC GCCollector;

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
		{
			DuiLogError(s.GetData());
			MessageBox(::GetActiveWindow(), s.GetData(), _T("Error"), 0);
			DuiException(s.GetData());
		}
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
		CDuiString szEntryFunc(_T("luaopen_")); szEntryFunc += szDllName;

		bool b = LuaApplication::Instance()->ImportLuaDLL(szDllName, szEntryFunc);
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
	int on_import_plugin(lua_State* L)
	{
		int n = lua_gettop(L);
		std::string fileName = lua_tostring(L, 1);
		CDuiString szDllName = toDuiString(fileName.c_str());
		bool b = CPaintManagerUI::LoadPlugin(szDllName);
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
}

namespace DuiLib
{
	static int lua_RegisterWindowMessage(lua_State* l)
	{
		CDuiString message;
		lua::get(l, 1, &message);
		return lua::push(l, ::RegisterWindowMessage(message));
	}
	static int lua_LoadIconFromFile(lua_State* l)
	{
		HINSTANCE hInstance = nullptr;
		CDuiString pstrFileName;
		lua::get(l, 1, &pstrFileName, &hInstance);
		HICON hIcon = ::LoadIcon(hInstance, pstrFileName);
		if (!hIcon)
		{
			hIcon = (HICON)::LoadImage(NULL, pstrFileName, IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTCOLOR);
			//hIcon = (HICON)::LoadImage(hInstance, pstrFileName, IMAGE_ICON, (::GetSystemMetrics(SM_CXICON) + 15) & ~15, (::GetSystemMetrics(SM_CYICON) + 15) & ~15, LR_DEFAULTCOLOR);
			return lua::push(l, hIcon);
		}
		else
		{
			return lua::push(l);
		}
	}
	static int lua_DestroyIcon(lua_State* l)
	{
		HICON hIcon = nullptr;
		lua::get(l, 1, &hIcon);
		DuiAssert(hIcon);
		::DestroyIcon(hIcon);
		return 0;
	}
	static int lua_GetCursorPos(lua_State* l)
	{
		POINT pt;
		::GetCursorPos(&pt);
		return lua::push(l, pt);
	}
	static int lua_GetModulePath(lua_State* l)
	{
		TCHAR s_buf[MAX_PATH];
		s_buf[0] = 0x0;
		DWORD n_result = ::GetModuleFileName(NULL, s_buf, sizeof(TCHAR) * MAX_PATH);
		TCHAR	s_drive[MAX_PATH];
		s_drive[0] = 0x0;
		TCHAR s_dir[MAX_PATH];
		s_dir[0] = 0x0;
		TCHAR s_filename[MAX_PATH];
		s_filename[0] = 0x0;
		TCHAR s_ext[MAX_PATH];
		s_ext[0] = 0x0;
		/*errno_t n_err_no =*/
		_tsplitpath_s(s_buf, s_drive, MAX_PATH, s_dir, MAX_PATH, s_filename, MAX_PATH, s_ext, MAX_PATH);
		//assert(n_err_no == 0);
		_tcscpy_s(s_buf, s_drive);
		_tcscat_s(s_buf, s_dir);

		CDuiString strpath = s_buf;
		strpath.Replace(_T("\\"), _T("/"));
		if (strpath.GetAt(strpath.GetLength() - 1) != _T('/'))
		{
			strpath += _T("/");
		}

		lua::push(l, strpath);
		return 1;
	}
	static int lua_GetAppName(lua_State* l)
	{
		TCHAR strAppName[MAX_PATH + 1];
		GetModuleFileName(NULL, strAppName, MAX_PATH);
		CDuiString sAppName(strAppName);
		lua::push(l, sAppName);
		return 1;
	}
	static int lua_GetAPPDATAPath(lua_State* l)
	{
		TCHAR Path[MAX_PATH] = { 0 };
		SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, Path);
		CDuiString sDataPath = Path;
		sDataPath.Replace(_T("\\"), _T("/"));
		if (sDataPath.GetAt(sDataPath.GetLength() - 1) != _T('/'))
		{
			sDataPath += _T("/");
		}

		lua::push(l, sDataPath);
		return 1;
	}
	static int lua_CreateDirectory(lua_State* l)
	{
		CDuiString strPath;
		lua::get(l, 1, &strPath);
		bool ret = true;
		if (!Path::IsDirectoryExist(strPath))
		{
			ret = Path::CreateDirectory(strPath, NULL);
		}
		return lua::push(l, ret);
	}
	static int lua_IsFileExist(lua_State* l)
	{
		CDuiString strPath;
		lua::get(l, 1, &strPath);
		bool ret = Path::IsFileExist(strPath);
		return lua::push(l, ret);
	}
	static int lua_IsDirectoryExist(lua_State* l)
	{
		CDuiString strPath;
		lua::get(l, 1, &strPath);
		bool ret = Path::IsDirectoryExist(strPath);
		return lua::push(l, ret);
	}
	static void FindImportFilesRaw(CDuiString strPath, std::vector<CDuiString>& importfiles)
	{
		WIN32_FIND_DATA find_data;
		HANDLE h_find = INVALID_HANDLE_VALUE;
		CDuiString dir = strPath;
		if (strPath[strPath.GetLength() - 1] == _T('\\'))
		{
			dir = strPath.Left(strPath.GetLength() - 1);
		}
		CDuiString s_search_dir = dir;
		s_search_dir += _T("\\*.*");
		h_find = ::FindFirstFile(s_search_dir, &find_data);
		s_search_dir = dir + _T("\\");
		s_search_dir += find_data.cFileName;
		if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) //目录
		{
			if (!(_tcscmp(find_data.cFileName, _T(".")) == 0 || _tcscmp(find_data.cFileName, _T("..")) == 0))	//非上层和自身目录
			{
				FindImportFilesRaw(s_search_dir, importfiles);
			}
		}
		else
		{
			importfiles.push_back(s_search_dir);
		}
		while (::FindNextFile(h_find, &find_data))
		{
			s_search_dir = dir + _T("\\");
			s_search_dir += find_data.cFileName;
			if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) //目录
			{
				if (!(_tcscmp(find_data.cFileName, _T(".")) == 0 || _tcscmp(find_data.cFileName, _T("..")) == 0))	//非上层和自身目录
				{
					FindImportFilesRaw(s_search_dir, importfiles);
				}
			}
			else
			{
				importfiles.push_back(s_search_dir);
			}
		}
		::FindClose(h_find);
	}
	static int lua_SHBrowseForFolder(lua_State* l)
	{
		HWND hWnd = NULL;
		CDuiString strTitle;
		lua::get(l, 1, &hWnd, &strTitle);
		std::vector<CDuiString> filelist;

		TCHAR Buffer[MAX_PATH] = { 0 };
		BROWSEINFO bi;
		ZeroMemory(&bi, sizeof(BROWSEINFO));
		bi.hwndOwner = hWnd;
		bi.pidlRoot = NULL;
		bi.pszDisplayName = Buffer;
		bi.lpszTitle = strTitle.GetData();
		bi.ulFlags = 0;
		bi.lpfn = NULL;
		LPITEMIDLIST pIDList = SHBrowseForFolder(&bi);
		if (pIDList)
		{
			if (SHGetPathFromIDList(pIDList, Buffer))
			{
				FindImportFilesRaw(Buffer, filelist);
			}
		}

		lua::push(l, filelist);
		return 1;
	}
	static int lua_RegSet(lua_State* l)
	{
		HKEY hKey = NULL;
		CDuiString cs_subkey;
		CDuiString cs_value_name;
		CDuiString cs_value;
		HKEY hKEY;
		lua::get(l, 1, &hKey, &cs_subkey, &cs_value_name, &cs_value);
		long ret0 = ::RegOpenKeyEx(hKey, cs_subkey, 0, KEY_ALL_ACCESS, &hKEY);
		if (ret0 != ERROR_SUCCESS)
		{
			ret0 = ::RegCreateKey(hKey, cs_subkey, &hKEY);
			if (ret0 != ERROR_SUCCESS)
			{
				lua::push(l, false);
				return 1;
			}
		}

		long ret1 = ::RegSetValueEx(hKEY, cs_value_name, 0, REG_SZ, (CONST BYTE*)(LPCTSTR)(cs_value.GetData()), cs_value.GetLength() * 2);

		long ret2 = ::RegCloseKey(hKEY);

		if (ret1 != ERROR_SUCCESS)
		{
			lua::push(l, false);
			return 1;
		}
		lua::push(l, true);
		return 1;
	}
	static int lua_RegSetEx(lua_State* l)
	{
		HKEY hKey = NULL;
		CDuiString cs_subkey;
		CDuiString cs_value_name;
		CDuiString cs_value;
		HKEY hKEY;
		lua::get(l, 1, &hKey, &cs_subkey, &cs_value_name, &cs_value);
		bool b_OK = false;
		long n_result = ::RegOpenKeyEx(hKey, cs_subkey, 0, KEY_ALL_ACCESS, &hKEY);
		if (n_result != ERROR_SUCCESS)
		{
			n_result = RegCreateKey(hKey, cs_subkey, &hKEY);
			if (n_result != ERROR_SUCCESS)
			{
				lua::push(l, false);
				return 1;
			}
		}

		TCHAR lpa[MAX_PATH];

		DWORD dwAidSize = MAX_PATH;
		DWORD type = REG_SZ;

		n_result = ::RegQueryValueEx(hKEY, cs_value_name, NULL, &type, (LPBYTE)lpa, &dwAidSize);

		if (n_result == ERROR_SUCCESS)
		{
			if (cs_value == lpa)
			{
				b_OK = true;
			}
		}

		if (!b_OK)
		{
			n_result = ::RegSetValueEx(hKEY, cs_value_name, 0, REG_SZ, (CONST BYTE*)(LPCTSTR)(cs_value.GetData()), cs_value.GetLength() * 2);
			if (n_result == ERROR_SUCCESS)
			{
				b_OK = true;
			}

		}
		::RegCloseKey(hKEY);

		lua::push(l, b_OK);
		return 1;
	}
	static int lua_RegDelete(lua_State* l)
	{
		CDuiString cs_subkey, cs_value_name;
		lua::get(l, 1, &cs_subkey, &cs_value_name);
		HKEY hKEY;
		long ret0 = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, cs_subkey, 0, KEY_ALL_ACCESS, &hKEY);
		long ret1 = ::RegDeleteValue(hKEY, cs_value_name);
		long ret2 = ::RegCloseKey(hKEY);

		if (ret1 == ERROR_SUCCESS)
			lua::push(l, true);
		else
			lua::push(l, false);
		return 1;
	}
	static int lua_RegQuery(lua_State* l)
	{
		CDuiString cs_subkey, cs_value_name;
		lua::get(l, 1, &cs_subkey, &cs_value_name);
		HKEY hKEY;
		long ret0 = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, cs_subkey, 0, KEY_ALL_ACCESS, &hKEY);

		TCHAR lpa[_MAX_DIR];
		CDuiString cs_value;

		DWORD dwAidSize = MAX_PATH;
		DWORD type = REG_SZ;
		long ret1 = ::RegQueryValueEx(hKEY, cs_value_name, NULL, &type, (LPBYTE)lpa, &dwAidSize);
		if (ret1 == ERROR_SUCCESS)
		{
			cs_value = lpa;
		}
		long ret2 = ::RegCloseKey(hKEY);

		lua::push(l, cs_value);
		return 1;
	}
	
	static WORD _HKF2SDK(WORD mod)
	{
		return ((mod & HOTKEYF_ALT) ? MOD_ALT : 0) \
			| ((mod & HOTKEYF_CONTROL) ? MOD_CONTROL : 0) \
			| ((mod & HOTKEYF_SHIFT) ? MOD_SHIFT : 0);
	}
	static WORD _SDK2HKF(WORD mod)
	{
		return ((mod & MOD_ALT) ? HOTKEYF_ALT : 0) \
			| ((mod & MOD_CONTROL) ? HOTKEYF_CONTROL : 0) \
			| ((mod & MOD_SHIFT) ? HOTKEYF_SHIFT : 0);
	}
	static int lua_RegisterHotKey(lua_State* l)
	{
		DWORD dwkey;
		HWND hWnd = nullptr;
		lua::get(l, 1, &hWnd, &dwkey);
		WORD wVirtualKeyCode = LOBYTE(LOWORD(dwkey));
		WORD wModifiers = HIBYTE(LOWORD(dwkey));
		UINT fsModifiers = _HKF2SDK(wModifiers);
		bool ret = ::RegisterHotKey(hWnd, dwkey, fsModifiers, wVirtualKeyCode);
		return lua::push(l, ret);
	}
	static int lua_UnregisterHotKey(lua_State* l)
	{
		DWORD dwkey;
		HWND hWnd = nullptr;
		lua::get(l, 1, &dwkey, &hWnd);
		bool ret = ::UnregisterHotKey(hWnd, dwkey);
		return lua::push(l, ret);
	}
	static int lua_FindFirstFile(lua_State* l)
	{
		CDuiString strPath;
		lua::get(l, 1, &strPath);
		WIN32_FIND_DATA* FIND_DATA = new WIN32_FIND_DATA();
		HANDLE h_find = INVALID_HANDLE_VALUE;
		h_find = ::FindFirstFile(strPath, FIND_DATA);
		if (h_find == INVALID_HANDLE_VALUE)
		{
			delete FIND_DATA;
			lua::push(l, lua::Nil);
			return 1;
		}

		lua_newtable(l);
		lua::push(l, "FIND_DATA");
		lua_pushlightuserdata(l, FIND_DATA);
		lua_settable(l, -3);

		lua::push(l, "HFIND");
		lua_pushlightuserdata(l, h_find);
		lua_settable(l, -3);

		return 1;
	}
	static int lua_FindNextFile(lua_State* l)
	{
		WIN32_FIND_DATA* FIND_DATA = NULL;
		HANDLE h_find = INVALID_HANDLE_VALUE;

		lua_pushnil(l);
		int real_pos = 1;
		while (lua_next(l, real_pos) != 0)
		{
			std::string key;
			lua::get(l, -2, &key);
			if (key == "FIND_DATA")
				FIND_DATA = (WIN32_FIND_DATA*)lua_touserdata(l, -1);
			else if (key == "HFIND")
				h_find = (HANDLE)lua_touserdata(l, -1);
			lua_pop(l, 1);
		}

		bool ret = false;
		if (FIND_DATA && h_find)
			ret = ::FindNextFile(h_find, FIND_DATA);

		lua::push(l, ret);
		return 1;
	}
	static int lua_GetFindData(lua_State* l)
	{
		WIN32_FIND_DATA* FIND_DATA = NULL;
		lua_pushnil(l);
		int real_pos = 1;
		while (lua_next(l, real_pos) != 0)
		{
			std::string key;
			lua::get(l, -2, &key);
			if (key == "FIND_DATA")
				FIND_DATA = (WIN32_FIND_DATA*)lua_touserdata(l, -1);
			lua_pop(l, 1);
		}
		
		CDuiString strPath;
		lua::get(l, 2, &strPath);

		if (FIND_DATA)
		{
			lua_newtable(l);
			lua::push(l, "cFileName");
			lua::push(l, Convert::ToUTF8(FIND_DATA->cFileName));
			lua_settable(l, -3);

			lua::push(l, "dwFileAttributes");
			lua::push(l, FIND_DATA->dwFileAttributes);
			lua_settable(l, -3);

			lua::push(l, "nFileSizeHigh");
			lua::push(l, FIND_DATA->nFileSizeHigh);
			lua_settable(l, -3);

			lua::push(l, "nFileSizeLow");
			lua::push(l, FIND_DATA->nFileSizeLow);
			lua_settable(l, -3);


			if ((FIND_DATA->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				lua::push(l, "mode");
				lua::push(l, "directory");
				lua_settable(l, -3);
			}
			else
			{
				lua::push(l, "mode");
				lua::push(l, "file");
				lua_settable(l, -3);
			}

			CDuiString strFullPath = Path::CombinePath(strPath, FIND_DATA->cFileName);

			struct _tstat stat_info;
			_tstat(strFullPath, &stat_info);
			lua::push(l, "st_size");
			lua::push(l, (unsigned int)(stat_info.st_size));
			lua_settable(l, -3);
			
			return 1;
		}
		else
		{
			lua::push(l, lua::Nil);
			return 1;
		}
	}
	static int lua_FindClose(lua_State* l)
	{
		WIN32_FIND_DATA* FIND_DATA = NULL;
		HANDLE h_find = INVALID_HANDLE_VALUE;

		lua_pushnil(l);
		int real_pos = 1;
		while (lua_next(l, real_pos) != 0)
		{
			std::string key;
			lua::get(l, -2, &key);
			if (key == "FIND_DATA")
				FIND_DATA = (WIN32_FIND_DATA*)lua_touserdata(l, -1);
			else if (key == "HFIND")
				h_find = (HANDLE)lua_touserdata(l, -1);
			lua_pop(l, 1);
		}

		if (h_find)
		{
			::FindClose(h_find);
		}
		if (FIND_DATA)
			delete FIND_DATA;
		return 0;
	}

	static int lua_SetMainWindow(lua_State* l)
	{
		CLuaWindow* pWindow = nullptr;
		lua::get(l, 1, &pWindow);
		LuaApplication::Instance()->SetMainWindow(pWindow);
		return 0;
	}

	static void lua_QuitApp()
	{
		::PostQuitMessage(0L);
	}
	
	static int lua_CreateTaskbarList(lua_State* l)
	{
		HRESULT hr = LuaApplication::Instance()->TaskbarList.CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_ALL);
		if (SUCCEEDED(hr))
			lua::push(l, true);
		else
			lua::push(l, false);

		return 1;
	}
	static int lua_TaskbarListSetProgressState(lua_State* l)
	{
		if (!LuaApplication::Instance()->TaskbarList)
		{
			lua::push(l, false);
			return 1;
		}
		/*
		TBPF_NOPROGRESS	= 0,
		TBPF_INDETERMINATE	= 0x1,
		TBPF_NORMAL	= 0x2,
		TBPF_ERROR	= 0x4,
		TBPF_PAUSED	= 0x8
		*/
		HWND hWnd;
		int state;
		lua::get(l, 1, &hWnd, &state);
		HRESULT hr = LuaApplication::Instance()->TaskbarList->SetProgressState(hWnd, (TBPFLAG)state);
		if (SUCCEEDED(hr))
			lua::push(l, true);
		else
			lua::push(l, false);

		return 1;
	}
	static int lua_TaskbarListSetProgressValue(lua_State* l)
	{
		if (!LuaApplication::Instance()->TaskbarList)
		{
			lua::push(l, false);
			return 1;
		}
		HWND hWnd;
		double a, b;
		lua::get(l, 1, &hWnd, &a, &b);
		HRESULT hr = LuaApplication::Instance()->TaskbarList->SetProgressValue(hWnd, a, b);
		if (SUCCEEDED(hr))
			lua::push(l, true);
		else
			lua::push(l, false);

		return 1;
	}
	static int lua_TaskbarListSetOverlayIcon(lua_State* l)
	{
		if (!LuaApplication::Instance()->TaskbarList)
		{
			lua::push(l, false);
			return 1;
		}
		HWND hWnd;
		HICON hIcon;
		std::string luaTip;
		lua::get(l, 1, &hWnd, &hIcon, &luaTip);
		std::wstring suni = DuiLib::Convert::UTF8ToUnicode(luaTip.c_str());
		HRESULT hr = LuaApplication::Instance()->TaskbarList->SetOverlayIcon(hWnd, hIcon, suni.c_str());
		if (SUCCEEDED(hr))
			lua::push(l, true);
		else
			lua::push(l, false);

		return 1;
	}
	static int lua_ImageList_LoadImage(lua_State* l)
	{
		if (!LuaApplication::Instance()->TaskbarList)
		{
			lua::push(l, false);
			return 1;
		}
		DuiLib::CDuiString sImgPath;
		int cx;
		int cGrow;
		DWORD r, g, b;
		unsigned int type;
		unsigned int nflags;
		HINSTANCE hInstance;
		lua::get(l, 1, &hInstance, &sImgPath, &cx, &cGrow, &r, &g, &b, &type, &nflags);
		HIMAGELIST hImglist = ImageList_LoadImage(hInstance, sImgPath, cx, cGrow, RGB(r, g, b), type, nflags);
		if (hImglist)
			lua_pushlightuserdata(l, hImglist);
		else
			lua_pushnil(l);
		return 1;
	}
	static int lua_ImageList_Destroy(lua_State* l)
	{
		HIMAGELIST hImglist;
		lua::get(l, 1, &hImglist);
		ImageList_Destroy(hImglist);
		return 0;
	}
	static int lua_TaskbarListThumbBarSetImageList(lua_State* l)
	{
		if (!LuaApplication::Instance()->TaskbarList)
		{
			lua::push(l, false);
			return 1;
		}
		HWND hWnd;
		HIMAGELIST hImglist;
		lua::get(l, 1, &hWnd, &hImglist);
		HRESULT hr = LuaApplication::Instance()->TaskbarList->ThumbBarSetImageList(hWnd, hImglist);
		if (SUCCEEDED(hr))
			lua::push(l, true);
		else
			lua::push(l, false);

		return 1;
	}
	static int lua_TaskbarListThumbBarAddButtons(lua_State* l)
	{
		if (!LuaApplication::Instance()->TaskbarList)
		{
			lua::push(l, false);
			return 1;
		}
		HWND hWnd;
		lua::get(l, 1, &hWnd);
		std::vector< THUMBBUTTON > buttons;
		
		lua_pushnil(l);
		int real_pos = 2;
		while (lua_next(l, real_pos) != 0)
		{
			THUMBBUTTON b;
			lua::table t;
			lua::get(l, -1, &t);
			t.get("id", &(b.iId));
			if (t.has("mask"))
			{
				unsigned int mask;
				t.get("mask", &mask);
				b.dwMask = (THUMBBUTTONMASK)mask;
			}
			if (t.has("flag"))
			{
				unsigned int flag;
				t.get("flag", &flag);
				b.dwFlags = (THUMBBUTTONFLAGS)flag;
			}
			if (t.has("tip"))
			{
				std::string luaTip;
				t.get("tip", &luaTip);
				std::wstring suni = DuiLib::Convert::UTF8ToUnicode(luaTip.c_str());
				lstrcpyW(b.szTip, suni.c_str());
			}
			if (t.has("icon"))
				t.get("icon", &b.hIcon);
			if (t.has("bitmap"))
				t.get("bitmap", &b.iBitmap);
			lua_pop(l, 1);
			buttons.push_back(b);
		}

		HRESULT hr = LuaApplication::Instance()->TaskbarList->ThumbBarAddButtons(hWnd, buttons.size(), &buttons[0]);
		if (SUCCEEDED(hr))
			lua::push(l, true);
		else
			lua::push(l, false);

		return 1;
	}
	static int lua_TaskbarListThumbBarUpdateButtons(lua_State* l)
	{
		if (!LuaApplication::Instance()->TaskbarList)
		{
			lua::push(l, false);
			return 1;
		}
		HWND hWnd;
		int iButton;
		lua::get(l, 1, &hWnd, &iButton);
		THUMBBUTTON b;
		lua::table t;
		lua::get(l, 3, &t);
		t.get("id", &(b.iId));
		if (t.has("mask"))
		{
			unsigned int mask;
			t.get("mask", &mask);
			b.dwMask = (THUMBBUTTONMASK)mask;
		}
		if (t.has("flag"))
		{
			unsigned int flag;
			t.get("flag", &flag);
			b.dwFlags = (THUMBBUTTONFLAGS)flag;
		}
		if (t.has("tip"))
		{
			std::string luaTip;
			t.get("tip", &luaTip);
			std::wstring suni = DuiLib::Convert::UTF8ToUnicode(luaTip.c_str());
			lstrcpyW(b.szTip, suni.c_str());
		}
		if (t.has("icon"))
			t.get("icon", &b.hIcon);
		if (t.has("bitmap"))
			t.get("bitmap", &b.iBitmap);

		HRESULT hr = LuaApplication::Instance()->TaskbarList->ThumbBarUpdateButtons(hWnd, iButton, &b);
		if (SUCCEEDED(hr))
			lua::push(l, true);
		else
			lua::push(l, false);

		return 1;
	}
	static int lua_GetTempPath(lua_State* l)
	{
		CDuiString szPath = Path::GetTempPath();
		lua::push(l, szPath);
		return 1;
	}
	static int lua_CreateLogger(lua_State* l)
	{
		if (lua_isstring(l, 1))
		{
			CDuiString szPath;
			lua::get(l, 1, &szPath);
			DuiLib::CreateLogger(szPath.GetData());
		}
		else
		{
			DuiLib::CreateLogger(NULL);
		}

		return 0;
	}
	static int lua_GetTickCount(lua_State* l)
	{
		DWORD dwV = ::GetTickCount();
		lua::push(l, dwV);
		return 1;
	}
	static int lua_GetLastError(lua_State* l)
	{
		UINT code = ::GetLastError();
		lua::push(l, code);
		return 1;
	}

	static int lua_OnInitEnv(lua_State* l)
	{
		return 0;
	}

	static int lua_OpenFolder(lua_State* L)
	{
		DuiLib::CDuiString cs_path;
		lua::get(L, 1, &cs_path);
		cs_path.Replace(_T('/'), _T('\\'));
		ShellExecute(NULL, NULL, _T("explorer"), (DuiLib::CDuiString(_T("/select,")) + cs_path).GetData(), NULL, SW_SHOW);
		return 0;
	}

	static int lua_OpenFile(lua_State* L)
	{
		DuiLib::CDuiString cs_path;
		lua::get(L, 1, &cs_path);
		cs_path.Replace(_T('/'), _T('\\'));
		ShellExecute(NULL, NULL, _T("open"), cs_path.GetData(), NULL, SW_SHOW);
		return 0;
	}

	static bool RegisterApplicationAPIToLua(lua_State* l)
	{
		//Application
		lua::lua_register_t<void>(l, "Application")
			.def("RegisterWindowMessage", lua_RegisterWindowMessage)
			.def("CreateTaskbarList", lua_CreateTaskbarList)
			.def("TaskbarListSetProgressState", lua_TaskbarListSetProgressState)
			.def("TaskbarListSetProgressValue", lua_TaskbarListSetProgressValue)
			.def("TaskbarListSetOverlayIcon", lua_TaskbarListSetOverlayIcon)
			.def("ImageList_LoadImage", lua_ImageList_LoadImage)
			.def("ImageList_Destroy", lua_ImageList_Destroy)
			.def("TaskbarListThumbBarSetImageList", lua_TaskbarListThumbBarSetImageList)
			.def("TaskbarListThumbBarAddButtons", lua_TaskbarListThumbBarAddButtons)
			.def("TaskbarListThumbBarUpdateButtons", lua_TaskbarListThumbBarUpdateButtons)
			.def("LoadIconFromFile", lua_LoadIconFromFile)
			.def("DestroyIcon", lua_DestroyIcon)
			.def("GetCursorPos", lua_GetCursorPos)
			.def("GetModulePath", lua_GetModulePath)
			.def("GetAPPDATAPath", lua_GetAPPDATAPath)
			.def("CreateLogger", lua_CreateLogger)
			.def("GetTempPath", lua_GetTempPath)
			.def("CreateDirectory", lua_CreateDirectory)
			.def("SHBrowseForFolder", lua_SHBrowseForFolder)
			.def("IsDirectoryExist", lua_IsDirectoryExist)
			.def("IsFileExist", lua_IsFileExist)
			.def("RegSet", lua_RegSet)
			.def("RegSetEx", lua_RegSetEx)
			.def("RegDelete", lua_RegDelete)
			.def("RegQuery", lua_RegQuery)
			.def("GetAppName", lua_GetAppName)
			.def("RegisterHotKey", lua_RegisterHotKey)
			.def("UnregisterHotKey", lua_UnregisterHotKey)
			.def("FindFirstFile", lua_FindFirstFile)
			.def("FindNextFile", lua_FindNextFile)
			.def("GetFindData", lua_GetFindData)
			.def("FindClose", lua_FindClose)
			.def("SetMainWindow", lua_SetMainWindow)
			.def("QuitApp", lua_QuitApp)
			.def("GetLastError", lua_GetLastError)
			.def("OnInitEnv", lua_OnInitEnv)
			.def("OpenFolder", lua_OpenFolder)
			.def("OpenFile", lua_OpenFile)
#ifdef _DEBUG
			.readonly("IsDebug", true);
#else
			.readonly("IsDebug", false);
#endif

		return true;
	}

	static void _KillDuiTimer(IDuiTimer* pDuiTimer)
	{
		DuiAssert(pDuiTimer);
		if (pDuiTimer)
		{
			if (pDuiTimer && pDuiTimer->GetUserData().iInt) {
				if (globalLuaEnv) {
					luaL_unref(*globalLuaEnv, LUA_REGISTRYINDEX, pDuiTimer->GetUserData().iInt);
				}
			}
			CDuiTimerBase* pObject = static_cast<CDuiTimerBase*>(pDuiTimer);
			LuaApplication::Instance()->TimerSource -= *pObject;
			delete pObject;
		}
	}

	void LuaApplication::OnDuiTimerCallback(IDuiTimer* pDuiTimer, HWND, CLuaWindow*, WPARAM)
	{
		int iref = pDuiTimer->GetUserData().iInt;
		if (globalLuaEnv && iref)
		{
			lua::lua_stack_gurad guard(*globalLuaEnv);
			globalLuaEnv->doFunc(iref);
			CDuiTimerBase* pTimerUI = (CDuiTimerBase*)pDuiTimer;
			if (!pTimerUI->GetTimerID())
			{
				_KillDuiTimer(pDuiTimer);
			}
		}
	}

	void LuaApplication::OnGlobalTimerCallback(IDuiTimer* pDuiTimer, void*)
	{
		DWORD id = ::GetCurrentThreadId();
		OutputDebugString(_T("current Thread:"));
	}

	IDuiTimer* LuaApplication::AddTimer(CLuaWindow* pWindow, int iInterval, int iTotalTimer/* = NULL*/, bool bAutoRun/* = true*/, bool bLoop/* = false*/, bool bRevers/* = false*/, unUserData* userdata/* = NULL*/)
	{
		DuiAssert(pWindow && ::IsWindow(*pWindow));
		auto DuiTimer = MakeDuiTimer(this, &LuaApplication::OnDuiTimerCallback,
			pWindow->GetHWND(), pWindow, (WPARAM)0, iInterval, iTotalTimer, bAutoRun, bLoop, bRevers);
		if (userdata) {
			DuiTimer.SetUserData(*userdata);
		}
		TimerSource += DuiTimer;

		return DuiTimer.Copy();
	}

	static int lua_AddGlobalTimer(lua_State* l)
	{
		CLuaWindow* pMainWindow = LuaApplication::Instance()->GetMainWindow();
		DuiAssert(pMainWindow && ::IsWindow(*pMainWindow));
		int nTop = lua_gettop(l);
		int iInterval;
		int iTotalTimer = 0;
		bool bAutoRun = true;
		bool bLoop = false;
		bool bRevers = false;
		int iref = LUA_NOREF;
		if (nTop == 6) {
			lua::get(l, 1, &iInterval, &iTotalTimer, &bAutoRun, &bLoop, &bRevers);
		}
		else if (nTop == 5) {
			lua::get(l, 1, &iInterval, &iTotalTimer, &bAutoRun, &bLoop);
		}
		else if (nTop == 4) {
			lua::get(l, 1, &iInterval, &iTotalTimer, &bAutoRun);
		}
		else if (nTop == 3) {
			lua::get(l, 1, &iInterval, &iTotalTimer);
		}
		else if (nTop == 2) {
			lua::get(l, 1, &iInterval);
		}
		else {
			lua_pushfstring(l, "timer at least two params");
			lua_error(l);
			return 1;
		}
		if (!lua_isfunction(l, nTop)) {
			lua_pushfstring(l, "param #%d must be function", nTop);
			lua_error(l);
			return 1;
		}

		lua_pushvalue(l, nTop);
		iref = luaL_ref(l, LUA_REGISTRYINDEX);

		unUserData ud;
		ud.iInt = iref;
		IDuiTimer* pDuiTimer = LuaApplication::Instance()->AddTimer(pMainWindow, iInterval, iTotalTimer, bAutoRun, bLoop, bRevers, &ud);
		lua_pushlightuserdata(l, pDuiTimer);
		return 1;
	}
	static int lua_RemoveGlobalTimer(lua_State* l)
	{
		IDuiTimer* pDuiTimer = (IDuiTimer*)lua_touserdata(l, 1);
		_KillDuiTimer(pDuiTimer);
		return 0;
	}

	static bool RegisterTimerAPIToLua(lua_State* l)
	{
		//Timer
		lua::lua_register_t<void>(l, "Timer")
			.def("AddGlobalTimer", lua_AddGlobalTimer)
			.def("RemoveGlobalTimer", lua_RemoveGlobalTimer)
			.def("GetTickCount", lua_GetTickCount);

		return true;
	}
}

//extern int luaopen_int64(lua_State*);

namespace DuiLib 
{
	///////////////////////////////////////////////////////////////
	////
	LuaApplication::LuaApplication(): valid(false), exiting(false), m_pMainWindow(NULL)
	{
		mainThreadId = ::GetCurrentThreadId();
		Initialize();

		SetObjectGCInterface(&GCCollector);
	}
	LuaApplication::~LuaApplication()
	{
		Unitialize();
		SetObjectGCInterface(NULL);
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
		exiting = true;
		cmd_lines.Empty();
		ShutdownLua();
		CPaintManagerUI::Term();
		::OleUninitialize();
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
			lua::lua_stack_gurad guard(*globalLuaEnv);
			lua_pushcfunction(*globalLuaEnv, on_import_luadll);
			lua_setfield(*globalLuaEnv, LUA_GLOBALSINDEX, "importluadll");

			lua_pushcfunction(*globalLuaEnv, on_import_plugin);
			lua_setfield(*globalLuaEnv, LUA_GLOBALSINDEX, "importplugin");
			
			lua_pushboolean(*globalLuaEnv, exiting);
			lua_setfield(*globalLuaEnv, LUA_GLOBALSINDEX, "CLOSING");

			lua_pushboolean(*globalLuaEnv, true);
			lua_setfield(*globalLuaEnv, LUA_GLOBALSINDEX, "INIT");
			const char* s = ROOT_DIR;
			lua_pushstring(*globalLuaEnv, s);
			lua_setfield(*globalLuaEnv, LUA_GLOBALSINDEX, "ROOT_DIR");

			lua_pushlightuserdata(*globalLuaEnv, &GCCollector);
			lua_setfield(*globalLuaEnv, LUA_GLOBALSINDEX, "__UD_GCCollector");
		}
		return true;
	}
	bool LuaApplication::ShutdownLua()
	{
		if (globalLuaEnv != NULL)
		{
			CDuiPtrArray arr = TimerSource.GetAllDuiTimers();
			for (int i = 0; i < arr.GetSize(); ++i)
			{
				IDuiTimer* pDuiTimer = (IDuiTimer*)(arr.GetAt(i));
				if (pDuiTimer && pDuiTimer->GetUserData().iInt) {
					if (globalLuaEnv) {
						luaL_unref(*globalLuaEnv, LUA_REGISTRYINDEX, pDuiTimer->GetUserData().iInt);
					}
				}
			}

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
		TimerSource.RemoveTimerAll();
		return true;
	}

	extern bool register_lua_scripts(lua_State*);
	extern int make_lua_global(lua_State*);
	
	bool LuaApplication::RegisterScript()
	{
		make_lua_global(*globalLuaEnv);
		//luaopen_int64(*globalLuaEnv);
		return register_lua_scripts(*globalLuaEnv) && RegisterTimerAPIToLua(*globalLuaEnv) && RegisterApplicationAPIToLua(*globalLuaEnv);
	}
	bool LuaApplication::ImportLuaDLL(LPCTSTR dllName, LPCTSTR dllEntry)
	{
		if (lua_plugins.Find(dllName) != NULL)
		{
			return true;
		}

		CDuiString szDllName = dllName;
		CDuiString szDllNameWithExt = szDllName + _T(".dll");
		CDuiString szEntryFunc = dllEntry;

		HPLUGINS hDll = CSystem::Instance()->LoadPlugin(szDllNameWithExt);
		if (hDll)
		{
			if (dllEntry)
			{
				typedef int(*LuaOpenEntryFunc)(lua_State*);
				LuaOpenEntryFunc lpfunc = CSystem::Instance()->GetFunction<LuaOpenEntryFunc>(hDll, szEntryFunc);
				if (!lpfunc)
				{
					CSystem::Instance()->FreePlugin(hDll);
					DuiLogError(_T("Failed to GetFunction : %s@%s"), szDllName, szEntryFunc);
					return false;
				}
				lpfunc(*globalLuaEnv);
			}
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

	bool LuaApplication::RunInternal(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR lpCmdLine, int nCmdShow)
	{		
		if (!valid)
		{
			MessageBox(NULL, _T("Failed to Initialize"), _T("Error"), 0);
			return false;
		}

		CPaintManagerUI::SetInstance(hInstance);
		CDuiString szFileName = Path::GetCurAppName(TRUE);
		CDuiString szLuaFile = szFileName + _T(".lua");
		CDuiString szEntryFile = Path::CombinePath(CPaintManagerUI::GetInstancePath(), szLuaFile);
		std::string entryfile = toStdString(szEntryFile.GetData());
		BYTE* buffer = NULL;
		unsigned int dwSize = 0;
		bool success = lua_load_tool::load_file(entryfile.c_str(), &buffer, &dwSize);
		if (!success)
		{
			DuiAssertX(false, _T("Failed to Initialize: %s"), szEntryFile.GetData());
			return false;
		}

		std::string chunk = "@" + toStdString(szLuaFile.GetData());
		if (0 != luaL_loadbuffer(*globalLuaEnv, (char*)buffer, dwSize, chunk.c_str()))
		{
			lua_load_tool::release_file(buffer);

			const char* err = lua_tostring(*globalLuaEnv, -1);
			DuiLogError("%s", err);
			lua_pop(*globalLuaEnv, 1);
			CDuiString s = toDuiString(err);
			DuiAssertX(false, _T("Failed to LoadLuaBuffer: %s"), s.GetData());
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
		
		Unitialize();

		return success;
	}

	bool LuaApplication::Run(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
	{
		bool b = RunInternal(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
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