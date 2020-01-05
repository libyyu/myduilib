
_G.DEFAULT_POS = nil
_G.FRAME_MAPS = {}
require "core.preload"
require "utility.i64lib"
_G.FLua = require "FLua"
_G.Utils = require "utility.Utils"
_G.Enum = require "utility.Enum"
importdll "LuaMyControls"
importdll "luaext"
_G.Event = require "events"

function _G.LoadAsset(filename, type_)
	local buffer, size = DuiLib.CResourceManager.LoadAsset(filename, type_)
	return buffer, size
end

function _G.FindAsset(filename, type_)
	return DuiLib.CResourceManager.FindAsset(filename, type_)
end

_G.nMaxSongNumber = 250
_G.MAX_USER_LIST = 30

_G.IDI_APP = 1001
local WM_USER = DuiLib.MsgArgs.WM_USER
local WM_USER_BEGIN = WM_USER + 1024
_G.USER_MSG = Enum.make
{
	"WM_WEB_SERVICE", "=", WM_USER_BEGIN,
	"WM_HTTP_FILE_DOWNLOAD",
	"WM_INSERT_EXPRESSION",
	"WM_MENU_NOTIFY",
	"WM_TRAY_NOTIFY",
	"WM_ENDING_SCROLL",
}

--定义菜单类型
_G.emMenuType = Enum.make
{
	"EMT_MAIN",
	"EMT_PLAYLIST",
	"EMT_SONGLIST",
	"EMT_SONGITEM",
	"EMT_LRCVIEW",
	"EMT_PLAYMODE",
	"EMT_TONEWLIST",
	"EMT_TOOTHERLIST",
}
--计时器
_G.emTimerType = Enum.make
{
	"EMT_PLAYING_TIMERID", "=", 0x100,
	"EMT_SHOWINFO_TIMERID"
}
--播放列表类型
_G.PlayListType = 
{
	plt_sys = 0,		--系统列表,如默认列表;
	plt_auto = 1, 		--智能列表，推荐，自动生成;
	plt_Roaming = 2,	--漫游;
	plt_user = 3,  		--用户自建列表;
};

local CControlUI_meta = _G.DuiLib.CControlUI.__methods

local __lua_userdata = {}
--setmetatable(__lua_userdata, {__mode = "k"})
function CControlUI_meta:SetUserData2(key, value)
	local t = rawget(__lua_userdata, self)
	if not t then
		t = {}
		rawset(__lua_userdata, self, t)
	end
	t[key] = value
end
function CControlUI_meta:GetUserData2(key)
	local t = rawget(__lua_userdata, self)
	if not t then
		return nil
	end
	return t[key]
end