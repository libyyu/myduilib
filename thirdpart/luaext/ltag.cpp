#define LUA_LIB
#include <lua.hpp>
#include <assert.h>
#include "luaext.h"
#include "itag.h"
#include <string>
#include <fstream>
#ifdef _MSC_VER
#include <Windows.h>
#endif

#define Meta_AudioTagInfo "Meta_AudioTagInfo"

extern lua_State* gL;


class AudioTagInfo
{
	std::string wide_to_multi(const wchar_t* c, int code = 65001 /*CP_UTF8*/) const
	{
		assert(c);
		std::wstring str(c);
#ifdef _MSC_VER
		int size = WideCharToMultiByte(code, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
#else
		size_t size = wcstombs(NULL, str.c_str(), 0);
#endif

		std::string mbstr(size, char(0));
#ifdef _MSC_VER
		WideCharToMultiByte(code, 0, str.c_str(), -1, const_cast<char*>(mbstr.c_str()), size, NULL, NULL);
#else
		wcstombs(const_cast<char*>(mbstr.c_str()), const_cast<wchar_t*>(str.c_str()), (size + 1) * 4);
#endif
		return mbstr;
	}
	std::wstring multi_to_wide(const char* c, int code = 65001 /*CP_UTF8*/) const
	{
		assert(c);
		std::string str(c);
#ifdef _MSC_VER
		int size = MultiByteToWideChar(code, 0, str.c_str(), -1, NULL, 0) - 1;
#else
		size_t size = mbstowcs(NULL, str.c_str(), 0);
#endif

		std::wstring wstr(size, wchar_t(0));
#ifdef _MSC_VER
		MultiByteToWideChar(code, 0, str.c_str(), str.length(), const_cast<wchar_t*>(wstr.c_str()), size);
#else
		mbstowcs(const_cast<wchar_t*>(wstr.c_str()), str.c_str(), size);
#endif
		return wstr;
	}
	std::string toMStr(const wchar_t* sInStr) const
	{
		std::string sOutStr;
		if (!sInStr || sInStr[0] == L'\0')
		{
			return sOutStr;
		}
		return wide_to_multi(sInStr);
	}
	std::wstring toWStr(const char* sInStr)
	{
		std::wstring sOutStr;
		if (!sInStr || sInStr[0] == '\0')
		{		
			return sOutStr;
		}

		return multi_to_wide(sInStr);
	}
public:
	AudioTagInfo()
	{
		_pTag = NULL;
	}
	virtual ~AudioTagInfo()
	{
		if (_pTag)
		{
			plugin_close_tag(_pTag);
		}
		_pTag = NULL;
	}
	virtual bool save_pic2file(const std::string& savename)
	{
		if (_pTag)
		{
			std::wstring stmp = toWStr(savename.c_str());
			return _pTag->exportImage(stmp.c_str());
		}
		return false;
	}
public:
	virtual bool init(const std::string& path, int& err)
	{
		std::string strU8 = path;// TOGBK(path.c_str());

		if (_pTag)
		{
			plugin_close_tag(_pTag);
			_pTag = NULL;
		}

		err = 0;
		_pTag = plugin_load_tag(strU8.c_str(), true, ITag::ReadAudioStyle::Average, &err);

		return _pTag && err == 0;
	}
	virtual std::string title() const
	{
		if (_pTag)
		{
			return toMStr(_pTag->title());
		}
		return "";
	}
	virtual std::string artist() const
	{
		if (_pTag)
		{
			return toMStr(_pTag->artist());
		}
		return "";
	}
	virtual std::string album() const
	{
		if (_pTag)
		{
			return toMStr(_pTag->album());
		}
		return "";
	}
	virtual std::string comment() const
	{
		if (_pTag)
		{
			return toMStr(_pTag->comment());
		}
		return "";
	}
	virtual std::string genre() const
	{
		if (_pTag)
		{
			return toMStr(_pTag->genre());
		}
		return "";
	}
	virtual unsigned long year() const
	{
		if (_pTag)
		{
			return _pTag->year();
		}
		return 0;
	}
	virtual unsigned long track() const
	{
		if (_pTag)
		{
			return _pTag->track();
		}
		return 0;
	}
	virtual unsigned long length() const
	{
		if (_pTag)
		{
			return _pTag->filesize();
		}
		return 0;
	}
	virtual unsigned long mineType() const
	{
		if (_pTag)
		{
			return _pTag->mineType();
		}
		return ITag::MineType::UNKONW;
	}

	virtual void title(const std::string& s_title)
	{
		if (_pTag)
		{
			std::wstring stmp = toWStr(s_title.c_str());
			_pTag->setTitle(stmp.c_str());
		}
	}
	virtual void artist(const std::string& s_artist)
	{
		if (_pTag)
		{
			std::wstring stmp = toWStr(s_artist.c_str());
			_pTag->setArtist(stmp.c_str());
		}
	}
	virtual void album(const std::string& s_album)
	{
		if (_pTag)
		{
			std::wstring stmp = toWStr(s_album.c_str());
			_pTag->setAlbum(stmp.c_str());
		}
	}
	virtual void comment(const std::string& s_comment)
	{
		if (_pTag)
		{
			std::wstring stmp = toWStr(s_comment.c_str());
			_pTag->setComment(stmp.c_str());
		}
	}
	virtual void genre(const std::string& s_genre)
	{
		if (_pTag)
		{
			std::wstring stmp = toWStr(s_genre.c_str());
			_pTag->setGenre(stmp.c_str());
		}
	}
	virtual void year(unsigned long u_year)
	{
		if (_pTag)
		{
			_pTag->setYear(u_year);
		}
	}
	virtual void track(unsigned long u_track)
	{
		if (_pTag)
		{
			_pTag->setTrack(u_track);
		}
	}
	virtual bool save()
	{
		if (_pTag)
		{
			return _pTag->save();
		}
		return false;
	}
private:
	ITag* _pTag;
};

static int __gc_AudioTagInfo(lua_State* L)
{
	lua::LuaUserData* userdata = (lua::LuaUserData*)luaL_checkudata(L, 1, Meta_AudioTagInfo);
	if (userdata && userdata->flag == 1)
	{
		AudioTagInfo* pTagInfo = static_cast<AudioTagInfo*>(userdata->object);
		if (pTagInfo) delete pTagInfo;
		userdata->flag = 0;
	}
	if (userdata)
	{
		lua::removeUserdataFromWeakTable(L, userdata->object);
		userdata->object = NULL;
	}
	return 0;
}
static int Release(lua_State* L)
{
	lua::LuaUserData* userdata = (lua::LuaUserData*)luaL_checkudata(L, 1, Meta_AudioTagInfo);
	if (userdata && userdata->flag == 1)
	{
		AudioTagInfo* pTagInfo = static_cast<AudioTagInfo*>(userdata->object);
		if (pTagInfo) delete pTagInfo;
		userdata->flag = 0;
	}
	if (userdata)
	{
		lua::removeUserdataFromWeakTable(L, userdata->object);
		userdata->object = NULL;
	}
	return 0;
}
static int NewAudioTagInfo(lua_State* L)
{
	AudioTagInfo* pTagInfo = new AudioTagInfo;
	assert(pTagInfo);
	if (!pTagInfo)
	{
		luaL_error(L, "Alloc AudioTagInfo Failed");
		return 0;
	}

	lua::push_to_lua(L, pTagInfo, Meta_AudioTagInfo);
	return 1;
}
static int save_pic2file(lua_State* L)
{
	AudioTagInfo* pTagInfo = lua::get_from_lua<AudioTagInfo>(L, 1, Meta_AudioTagInfo);
	assert(pTagInfo);
	std::string strPath = lua_tostring(L, 2);
	bool ret = pTagInfo->save_pic2file(strPath);
	lua_pushboolean(L, ret);
	return 1;
}
static int init(lua_State* L)
{
	AudioTagInfo* pTagInfo = lua::get_from_lua<AudioTagInfo>(L, 1, Meta_AudioTagInfo);
	assert(pTagInfo);
	std::string strPath = lua_tostring(L, 2);
	int err = 0;
	bool ret = pTagInfo->init(strPath, err);
	lua_pushboolean(L, ret);
	lua_pushinteger(L, err);
	return 2;
}
static int get_title(lua_State* L)
{
	AudioTagInfo* pTagInfo = lua::get_from_lua<AudioTagInfo>(L, 1, Meta_AudioTagInfo);
	assert(pTagInfo);
	std::string str = pTagInfo->title();
	lua_pushstring(L, str.c_str());
	return 1;
}
static int get_artist(lua_State* L)
{
	AudioTagInfo* pTagInfo = lua::get_from_lua<AudioTagInfo>(L, 1, Meta_AudioTagInfo);
	assert(pTagInfo);
	std::string str = pTagInfo->artist();
	lua_pushstring(L, str.c_str());
	return 1;
}
static int get_album(lua_State* L)
{
	AudioTagInfo* pTagInfo = lua::get_from_lua<AudioTagInfo>(L, 1, Meta_AudioTagInfo);
	assert(pTagInfo);
	std::string str = pTagInfo->album();
	lua_pushstring(L, str.c_str());
	return 1;
}
static int get_comment(lua_State* L)
{
	AudioTagInfo* pTagInfo = lua::get_from_lua<AudioTagInfo>(L, 1, Meta_AudioTagInfo);
	assert(pTagInfo);
	std::string str = pTagInfo->comment();
	lua_pushstring(L, str.c_str());
	return 1;
}
static int get_genre(lua_State* L)
{
	AudioTagInfo* pTagInfo = lua::get_from_lua<AudioTagInfo>(L, 1, Meta_AudioTagInfo);
	assert(pTagInfo);
	std::string str = pTagInfo->genre();
	lua_pushstring(L, str.c_str());
	return 1;
}
static int get_year(lua_State* L)
{
	AudioTagInfo* pTagInfo = lua::get_from_lua<AudioTagInfo>(L, 1, Meta_AudioTagInfo);
	assert(pTagInfo);
	unsigned long value = pTagInfo->year();
	lua_pushnumber(L, value);
	return 1;
}
static int get_track(lua_State* L)
{
	AudioTagInfo* pTagInfo = lua::get_from_lua<AudioTagInfo>(L, 1, Meta_AudioTagInfo);
	assert(pTagInfo);
	unsigned long value = pTagInfo->track();
	lua_pushnumber(L, value);
	return 1;
}
static int get_length(lua_State* L)
{
	AudioTagInfo* pTagInfo = lua::get_from_lua<AudioTagInfo>(L, 1, Meta_AudioTagInfo);
	assert(pTagInfo);
	unsigned long value = pTagInfo->length();
	lua_pushnumber(L, value);
	return 1;
}
static int get_mineType(lua_State* L)
{
	AudioTagInfo* pTagInfo = lua::get_from_lua<AudioTagInfo>(L, 1, Meta_AudioTagInfo);
	assert(pTagInfo);
	unsigned long value = pTagInfo->mineType();
	lua_pushnumber(L, value);
	return 1;
}
static int set_title(lua_State* L)
{
	AudioTagInfo* pTagInfo = lua::get_from_lua<AudioTagInfo>(L, 1, Meta_AudioTagInfo);
	assert(pTagInfo);
	const char* str = lua_tostring(L, 2);
	pTagInfo->title(str);
	return 0;
}
static int set_artist(lua_State* L)
{
	AudioTagInfo* pTagInfo = lua::get_from_lua<AudioTagInfo>(L, 1, Meta_AudioTagInfo);
	assert(pTagInfo);
	const char* str = lua_tostring(L, 2);
	pTagInfo->artist(str);
	return 0;
}
static int set_album(lua_State* L)
{
	AudioTagInfo* pTagInfo = lua::get_from_lua<AudioTagInfo>(L, 1, Meta_AudioTagInfo);
	assert(pTagInfo);
	const char* str = lua_tostring(L, 2);
	pTagInfo->album(str);
	return 0;
}
static int set_comment(lua_State* L)
{
	AudioTagInfo* pTagInfo = lua::get_from_lua<AudioTagInfo>(L, 1, Meta_AudioTagInfo);
	assert(pTagInfo);
	const char* str = lua_tostring(L, 2);
	pTagInfo->comment(str);
	return 0;
}
static int set_genre(lua_State* L)
{
	AudioTagInfo* pTagInfo = lua::get_from_lua<AudioTagInfo>(L, 1, Meta_AudioTagInfo);
	assert(pTagInfo);
	const char* str = lua_tostring(L, 2);
	pTagInfo->genre(str);
	return 0;
}
static int set_year(lua_State* L)
{
	AudioTagInfo* pTagInfo = lua::get_from_lua<AudioTagInfo>(L, 1, Meta_AudioTagInfo);
	assert(pTagInfo);
	unsigned long value = lua_tointeger(L, 2);
	pTagInfo->year(value);
	return 0;
}
static int set_track(lua_State* L)
{
	AudioTagInfo* pTagInfo = lua::get_from_lua<AudioTagInfo>(L, 1, Meta_AudioTagInfo);
	assert(pTagInfo);
	unsigned long value = lua_tointeger(L, 2);
	pTagInfo->track(value);
	return 0;
}
static int save(lua_State* L)
{
	AudioTagInfo* pTagInfo = lua::get_from_lua<AudioTagInfo>(L, 1, Meta_AudioTagInfo);
	assert(pTagInfo);
	bool ret = pTagInfo->save();
	lua_pushboolean(L, ret);
	return 1;
}

int lua_register_AudioTagInfo(lua_State* L)
{
	luaL_Reg reg[] = {
		{"New", NewAudioTagInfo },
		{NULL,NULL},
	};

	luaL_Reg mt_reg[] = {
		{"release", Release },
		{"save_pic2file", save_pic2file },
		{"init", init },
		{"get_title", get_title },
		{"get_artist", get_artist },
		{"get_album", get_album },
		{"get_comment", get_comment },
		{"get_genre", get_genre },
		{"get_year", get_year },
		{"get_track", get_track },
		{"get_length", get_length },
		{"get_mineType", get_mineType },
		{"set_title", set_title },
		{"set_artist", set_artist },
		{"set_album", set_album },
		{"set_comment", set_comment },
		{"set_genre", set_genre },
		{"set_year", set_year },
		{"set_track", set_track },
		{"save", save },
	};

	int nTop = lua_gettop(L);

	/// 注册reg中的方法到Lua中（“Database”相当于类名）,reg中的方法相当于Database成员函数
	luaL_register(L, "AudioTagInfo", reg);

	/// 创建名字为 Meta_Database 元表	
	luaL_newmetatable(L, Meta_AudioTagInfo);//mt
	/// 修改元表 Meta_Database 查找索引，把它指向 Meta_Database 自身	
	lua_pushvalue(L, -1);//mt,mt
	lua_setfield(L, -2, "__index");//mt
	/// 自动删除，如果表里有__gc,Lua的垃圾回收机制会调用它。	    
	lua_pushcfunction(L, __gc_AudioTagInfo);	// mt,func
	lua_setfield(L, -2, "__gc"); 	//mt

	for (int i = 0; i < sizeof(mt_reg) / sizeof(mt_reg[0]); ++i)
	{
		lua_pushcfunction(L, mt_reg[i].func);
		lua_setfield(L, -2, mt_reg[i].name);
	}

	lua_settop(L, nTop);

	return 1;
}