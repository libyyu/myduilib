
_G.DEFAULT_POS = nil
_G.FRAME_MAPS = {}
require "core.preload"
require "utility.i64lib"
_G.FLua = require "FLua"
_G.Utils = require "utility.Utils"
_G.Enum = require "utility.Enum"
importluadll "LuaMyControls"
importluadll "luaext"
_G.Event = require "events"

function _G.LoadAsset(filename, type_)
	local buffer, size = DuiLib.CResourceManager.LoadAsset(filename, type_)
	return buffer, size
end

function _G.FindAsset(filename, type_)
	return DuiLib.CResourceManager.FindAsset(filename, type_)
end

local CControlUI_meta = _G.DuiLib.CControlUI.class()
local CWindowWnd_meta = _G.DuiLib.CWindowWnd.class()

local __lua_userdata = {}
setmetatable(__lua_userdata, {__mode = "k"})
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
function CWindowWnd_meta:SetUserData2(key, value)
	local t = rawget(__lua_userdata, self)
	if not t then
		t = {}
		rawset(__lua_userdata, self, t)
	end
	t[key] = value
end
function CWindowWnd_meta:GetUserData2(key)
	local t = rawget(__lua_userdata, self)
	if not t then
		return nil
	end
	return t[key]
end

function _T(x)
	return x
end 
