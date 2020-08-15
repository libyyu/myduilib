#define LUA_LIB
#include <lua.hpp>
#include <assert.h>
#include "luaext.h"
#include "AVPlayer.h"
#include <sstream>
#include "MainThreadTask/MainThreadTask.h"

#define Meta_AVPlayer "Meta_CAVPlayer"

extern lua_State* gL;

class MainThreadTask_OnAction : public MainThreadTask
{
	std::string name;
	int luaRef;
public:
	MainThreadTask_OnAction(const char* s, int r) : name(s), luaRef(r)
	{
	}
	virtual void doTask()
	{
		if (gL) {
			int nTop = lua_gettop(gL);
			lua_rawgeti(gL, LUA_REGISTRYINDEX, luaRef);
			if (!lua_istable(gL, -1))
			{
				lua_settop(gL, nTop);
				luaL_error(gL, "luaref is not a table!");
//				goto RETURN;
			}
			lua_getfield(gL, -1, name.c_str());	//--> t, func
			if (!lua_isfunction(gL, -1))
			{
				lua_settop(gL, nTop);
				lua_pushfstring(gL, "Fail to find function: %s", name.c_str());
				lua_error(gL);
//				goto RETURN;
			}
			//lua_pushlightuserdata(gL, event);
			int ret = lua_pcall(gL, 0, 0, 0);
			if (0 != ret)
			{
				lua_error(gL);
				lua_settop(gL, nTop);
//				goto RETURN;
			}
			lua_settop(gL, nTop);
		}
//	RETURN:
//		delete this;
	}
};

class LuaAVPlayer : public CAVPlayer, public IAVPlayerCallBack
{
	MainThreadTask_OnAction* pOnPlaying;
	MainThreadTask_OnAction* pOnPosChanged;
	MainThreadTask_OnAction* pOnEndReached;
public:
	LuaAVPlayer() : CAVPlayer(), _luaRef(LUA_NOREF), pOnPlaying(NULL), pOnEndReached(NULL), pOnPosChanged(NULL)
	{
		SetCallback(this);
	}
	virtual ~LuaAVPlayer()
	{
		if (pOnPlaying) { delete pOnPlaying; pOnPlaying = NULL; }
		if (pOnEndReached) { delete pOnEndReached; pOnEndReached = NULL; }
		if (pOnPosChanged) { delete pOnPosChanged; pOnPosChanged = NULL; }
		if (_luaRef != LUA_NOREF && gL)
		{
			luaL_unref(gL, LUA_REGISTRYINDEX, _luaRef);
		}
		_luaRef = LUA_NOREF;
	}
	void BindLua(int luaref)
	{
		if (_luaRef != LUA_NOREF && gL)
		{
			luaL_unref(gL, LUA_REGISTRYINDEX, _luaRef);
		}
		_luaRef = luaref;

		pOnPlaying = new MainThreadTask_OnAction("OnPlaying", _luaRef);
		pOnEndReached = new MainThreadTask_OnAction("OnEndReached", _luaRef);
		pOnPosChanged = new MainThreadTask_OnAction("OnPosChanged", _luaRef);
	}
protected:
	//多线程调用
	virtual void OnPlaying(CAVPlayer* player, void* event)         // 设置文件头读取完毕时的回调函数
	{
		if (pOnPlaying) MainThreadTaskManager::instance().addTask(pOnPlaying);
	}
	virtual void OnPosChanged(CAVPlayer* player, void* event)      // 设置文件位置改变时的回调函数
	{
		if (pOnPosChanged) MainThreadTaskManager::instance().addTask(pOnPosChanged);
	}
	virtual void OnEndReached(CAVPlayer* player, void* event)      // 设置文件头读取完毕时的回调函数
	{
		if(pOnEndReached) MainThreadTaskManager::instance().addTask(pOnEndReached);
	}
private:
private:
	int _luaRef;
};
///////////////////////////////////////////////////////////////////////////////////////
static int push_stack(lua_State* l, int64_t value)
{
	lua_pushlstring(l, (const char*)&value, 8);

	return 1;
}
static int push_stack(lua_State* l, uint64_t value)
{
	lua_pushlstring(l, (const char*)&value, 8);

	return 1;
}
///////////////////////////////////////////////////////////////////////////////////////
static int __gc_AVPlayer(lua_State* L)
{
	lua::LuaUserData* userdata = (lua::LuaUserData*)luaL_checkudata(L, 1, Meta_AVPlayer);
	if (userdata && userdata->object && userdata->flag == 1)
	{
		LuaAVPlayer* pPlayCenter = static_cast<LuaAVPlayer*>(userdata->object);
		if (pPlayCenter) delete pPlayCenter;
		userdata->flag = 0;
		userdata->object = NULL;
	}
	return 0;
}
static int NewPlayCenter(lua_State* L)
{
	LuaAVPlayer* pPlayCenter = new LuaAVPlayer;
	assert(pPlayCenter);
	if (!pPlayCenter)
	{
		luaL_error(L, "Alloc AVPlayer Failed");
		return 0;
	}
	if (!lua_isnoneornil(L, 1))
	{
		int luaRef = luaL_ref(L, LUA_REGISTRYINDEX);
		pPlayCenter->BindLua(luaRef);
	}
	lua::push_to_lua(L, pPlayCenter, Meta_AVPlayer);
	return 1;
}
static int BindLua(lua_State* L)
{
	LuaAVPlayer* pPlayCenter = lua::get_from_lua<LuaAVPlayer>(L, 1, Meta_AVPlayer);
	assert(pPlayCenter);
	lua_pushvalue(L, 2);
	int luaRef = luaL_ref(L, LUA_REGISTRYINDEX);
	pPlayCenter->BindLua(luaRef);
	return 0;
}

static int Release(lua_State* L)
{
	lua::LuaUserData* userdata = (lua::LuaUserData*)luaL_checkudata(L, 1, Meta_AVPlayer);
	if (userdata && userdata->object && userdata->flag == 1)
	{
		LuaAVPlayer* pPlayCenter = static_cast<LuaAVPlayer*>(userdata->object);
		if (pPlayCenter) delete pPlayCenter;
		userdata->flag = 0;
		userdata->object = NULL;
	}
	return 0;
}
static int PlayFile(lua_State* L)
{
	LuaAVPlayer* pPlayCenter = lua::get_from_lua<LuaAVPlayer>(L, 1, Meta_AVPlayer);
	assert(pPlayCenter);
	std::string strPath = lua_tostring(L, 2);
	bool ret = pPlayCenter->PlayFile(strPath);
	lua_pushboolean(L, ret);
	return 1;
}
static int PlayURL(lua_State* L)
{
	LuaAVPlayer* pPlayCenter = lua::get_from_lua<LuaAVPlayer>(L, 1, Meta_AVPlayer);
	assert(pPlayCenter);
	std::string strPath = lua_tostring(L, 2);
	bool ret = pPlayCenter->PlayURL(strPath);
	lua_pushboolean(L, ret);
	return 1;
}
static int Play(lua_State* L)
{
	LuaAVPlayer* pPlayCenter = lua::get_from_lua<LuaAVPlayer>(L, 1, Meta_AVPlayer);
	assert(pPlayCenter);
	int ret = pPlayCenter->Play();
	lua_pushinteger(L, ret);
	return 1;
}
static int Pause(lua_State* L)
{
	LuaAVPlayer* pPlayCenter = lua::get_from_lua<LuaAVPlayer>(L, 1, Meta_AVPlayer);
	assert(pPlayCenter);
	pPlayCenter->Pause();
	return 0;
}
static int Stop(lua_State* L)
{
	LuaAVPlayer* pPlayCenter = lua::get_from_lua<LuaAVPlayer>(L, 1, Meta_AVPlayer);
	assert(pPlayCenter);
	pPlayCenter->Stop();
	return 0;
}
static int Volume(lua_State* L)
{
	LuaAVPlayer* pPlayCenter = lua::get_from_lua<LuaAVPlayer>(L, 1, Meta_AVPlayer);
	assert(pPlayCenter);
	int iVol = lua_tointeger(L, 2);
	pPlayCenter->Volume(iVol);
	return 0;
}
static int VolumeIncrease(lua_State* L)
{
	LuaAVPlayer* pPlayCenter = lua::get_from_lua<LuaAVPlayer>(L, 1, Meta_AVPlayer);
	assert(pPlayCenter);
	pPlayCenter->VolumeIncrease();
	return 0;
}
static int VolumeReduce(lua_State* L)
{
	LuaAVPlayer* pPlayCenter = lua::get_from_lua<LuaAVPlayer>(L, 1, Meta_AVPlayer);
	assert(pPlayCenter);
	pPlayCenter->VolumeReduce();
	return 0;
}
static int SeekTo(lua_State* L)
{
	LuaAVPlayer* pPlayCenter = lua::get_from_lua<LuaAVPlayer>(L, 1, Meta_AVPlayer);
	assert(pPlayCenter);
	int iValue = lua_tointeger(L, 2);
	pPlayCenter->SeekTo(iValue);
	return 0;
}
static int SeekForward(lua_State* L)
{
	LuaAVPlayer* pPlayCenter = lua::get_from_lua<LuaAVPlayer>(L, 1, Meta_AVPlayer);
	assert(pPlayCenter);
	int iValue = 5;
	if (!lua_isnoneornil(L, 2))
		iValue = luaL_checkinteger(L, 2);
	pPlayCenter->SeekForward(iValue);
	return 0;
}
static int SeekBackward(lua_State* L)
{
	LuaAVPlayer* pPlayCenter = lua::get_from_lua<LuaAVPlayer>(L, 1, Meta_AVPlayer);
	assert(pPlayCenter);
	int iValue = 5;
	if (!lua_isnoneornil(L, 2))
		iValue = luaL_checkinteger(L, 2);
	pPlayCenter->SeekBackward(iValue);
	return 0;
}
static int SetHWND(lua_State* L)
{
	LuaAVPlayer* pPlayCenter = lua::get_from_lua<LuaAVPlayer>(L, 1, Meta_AVPlayer);
	assert(pPlayCenter);
	H_HWND hWnd = NULL;
	if (lua_isuserdata(L, 2))
		hWnd = (H_HWND)lua_touserdata(L, 2);

	pPlayCenter->SetHWND(hWnd);
	return 0;
}
static int GetHWND(lua_State* L)
{
	LuaAVPlayer* pPlayCenter = lua::get_from_lua<LuaAVPlayer>(L, 1, Meta_AVPlayer);
	assert(pPlayCenter);
	H_HWND hWnd = pPlayCenter->GetHWND();
	lua_pushlightuserdata(L, hWnd);
	return 1;
}
static int GetStatus(lua_State* L)
{
	LuaAVPlayer* pPlayCenter = lua::get_from_lua<LuaAVPlayer>(L, 1, Meta_AVPlayer);
	assert(pPlayCenter);
	int iValue = pPlayCenter->GetStatus();
	lua_pushinteger(L, iValue);
	return 1;
}
static int IsValid(lua_State* L)
{
	LuaAVPlayer* pPlayCenter = lua::get_from_lua<LuaAVPlayer>(L, 1, Meta_AVPlayer);
	assert(pPlayCenter);
	if (pPlayCenter)
	{
		lua_pushboolean(L, pPlayCenter->IsValid());
		return 1;
	}
	return 0;
}
static int IsOpen(lua_State* L)
{
	LuaAVPlayer* pPlayCenter = lua::get_from_lua<LuaAVPlayer>(L, 1, Meta_AVPlayer);
	assert(pPlayCenter);
	bool ret = pPlayCenter->IsOpen();
	lua_pushboolean(L, ret);
	return 1;
}
static int IsPlaying(lua_State* L)
{
	LuaAVPlayer* pPlayCenter = lua::get_from_lua<LuaAVPlayer>(L, 1, Meta_AVPlayer);
	assert(pPlayCenter);
	bool ret = pPlayCenter->IsPlaying();
	lua_pushboolean(L, ret);
	return 1;
}
static int GetPos(lua_State* L)
{
	LuaAVPlayer* pPlayCenter = lua::get_from_lua<LuaAVPlayer>(L, 1, Meta_AVPlayer);
	assert(pPlayCenter);
	int ret = pPlayCenter->GetPos();
	lua_pushinteger(L, ret);
	return 1;
}
static int GetVolume(lua_State* L)
{
	LuaAVPlayer* pPlayCenter = lua::get_from_lua<LuaAVPlayer>(L, 1, Meta_AVPlayer);
	assert(pPlayCenter);
	int ret = pPlayCenter->GetVolume();
	lua_pushinteger(L, ret);
	return 1;
}
static int GetTotalTime(lua_State* L)
{
	LuaAVPlayer* pPlayCenter = lua::get_from_lua<LuaAVPlayer>(L, 1, Meta_AVPlayer);
	assert(pPlayCenter);
	__int64 ret = pPlayCenter->GetTotalTime();
	push_stack(L, ret);
	return 1;
}
static int GetTime(lua_State* L)
{
	LuaAVPlayer* pPlayCenter = lua::get_from_lua<LuaAVPlayer>(L, 1, Meta_AVPlayer);
	assert(pPlayCenter);
	__int64 ret = pPlayCenter->GetTime();
	push_stack(L, ret);
	return 1;
}

int lua_register_PlayCenter(lua_State* L)
{
	luaL_Reg reg[] = {
		{"New", NewPlayCenter },
		{NULL,NULL},
	};

	luaL_Reg mt_reg[] = {
		{"Release", Release },
		{"PlayFile", PlayFile },
		{"PlayURL", PlayURL },
		{"Play", Play },
		{"Pause", Pause },
		{"Stop", Stop },
		{"Volume", Volume },
		{"VolumeIncrease", VolumeIncrease },
		{"VolumeReduce", VolumeReduce },
		{"SeekTo", SeekTo },
		{"SeekForward", SeekForward },
		{"SeekBackward", SeekBackward },
		{"SetHWND", SetHWND },
		{"GetHWND", GetHWND },
		{"GetStatus", GetStatus },
		{"IsValid", IsValid },
		{"IsOpen", IsOpen },
		{"IsPlaying", IsPlaying },
		{"GetPos", GetPos },
		{"GetVolume", GetVolume },
		{"GetTotalTime", GetTotalTime },
		{"GetTime", GetTime },
	};

	int nTop = lua_gettop(L);

	/// 注册reg中的方法到Lua中（“Database”相当于类名）,reg中的方法相当于Database成员函数
	luaL_register(L, "AVPlayer", reg);

	/// 创建名字为 Meta_Database 元表	
	luaL_newmetatable(L, Meta_AVPlayer);//mt
	/// 修改元表 Meta_Database 查找索引，把它指向 Meta_Database 自身	
	lua_pushvalue(L, -1);//mt,mt
	lua_setfield(L, -2, "__index");//mt
	/// 自动删除，如果表里有__gc,Lua的垃圾回收机制会调用它。	
	lua_pushcfunction(L, __gc_AVPlayer);	//mt,func
	lua_setfield(L, -2, "__gc"); 	//mt

	for (int i = 0; i < sizeof(mt_reg) / sizeof(mt_reg[0]); ++i)
	{
		lua_pushcfunction(L, mt_reg[i].func);
		lua_setfield(L, -2, mt_reg[i].name);
	}

	lua_settop(L, nTop);

	return 1;
}