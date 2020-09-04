
_G.DEFAULT_POS = nil
_G.FRAME_MAPS = {}
require "core.preload"
require "coro"
require "utility.i64lib"
_G.FLua = require "FLua"
_G.Utils = require "utility.Utils"
_G.Enum = require "utility.Enum"
importplugin "MyControls"
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

function IsInt64(v)
	if type(v) == "table" and getmetatable(v) and getmetatable(v).magic64 then
		return true
	elseif type(v) == "string" and lib64.isint64(v) then 
		return true
	else
		return false
	end
end

local mytostring = function(...)
	if Utils then
		return Utils.Value2String(...)
	else
		return tostring(...)
	end
end

--[[
	抓取当前栈里面的局部已经上层变量信息
]]
function _G.DumpStack(info, withTraceback, thread, level)
	local function make_value(value)
		local mt = getmetatable(value)
		if mt and mt.__pMessage and mt.__pDescriptor then--是一个pb协议
			local v = tostring(value)
			local arr = v:split("\n")
			local sb = {}
			for ii, v in ipairs(arr) do
				if ii == 1 then
					sb[#sb+1] = mytostring(v)
				else
					sb[#sb+1] = '        ' .. mytostring(v)
				end
			end
			return table.concat(sb, "\n")
		else
			return mytostring(value)
		end
	end

	local function make_kv(name, value, records)
		local tt = type(value)
		if (tt=="userdata") then
			if not records[value] then
				records[value] = true
				return name .. '=' .. mytostring(value) .. '\n'
			end
		elseif (tt=="table") then
			if not records[value] then
				records[value] = true
				return name .. '=' .. make_value(value) .. "\n"
			end
		else
			if tt == "function" and mytostring(value):find('function: builtin#') then
				return nil
			end

			return name .. '=' .. mytostring(value) .. '\n'
		end
	end

	local function vars(f, records)
		local dump = ''
		local func = (thread and debug.getinfo(thread, f, 'f').func or debug.getinfo(f, 'f').func)
		local i = 1
		-- get locals
		while true do
			local name, value = nil, nil
			if thread then
				name, value = debug.getlocal(thread, f, i)
			else
				name, value = debug.getlocal(f, i)
			end
			if not name or name == "" then break end
			if string.sub(name, 1, 1) ~= '(' then
				local result = make_kv(name, value, records)
				if result then
					dump = dump ..  '        ' .. result
				end
			end
			i = i + 1
		end
		-- get varargs (these use negative indices)
		i = 1
		local iarg = 0
		while true do
			local name, value = nil, nil
			if thread then
				name, value = debug.getlocal(thread, f, -i)
			else
				name, value = debug.getlocal(f, -i)
			end
			-- `not name` should be enough, but LuaJIT 2.0.0 incorrectly reports `(*temporary)` names here
			if not name or name == ""  or name ~= '(*vararg)' then break end
			iarg = iarg + 1
			name = "#"..iarg

			local result = make_kv(name, value, records)
			if result then
				dump = dump ..  '        ' .. result
			end

			i = i + 1
		end
		-- get upvalues
		i = 1
		while func do -- check for func as it may be nil for tail calls
			local name, value = nil, nil
			if thread then
				name, value = debug.getupvalue(thread, func, i)
			else
				name, value = debug.getupvalue(func, i)
			end
			if not name or name == "" then break end
			local result = make_kv(name, value, records)
			if result then
				dump = dump ..  '        ' .. result
			end

			i = i + 1
		end
		return dump
	end
	local stacktrack
	if withTraceback then
		stacktrack = thread and debug.traceback(thread, info, 2) or debug.traceback(info, 2) or ""
	else
		stacktrack = info or "stack info:"
	end
	local records = {}
	local dump = ''
	local count = 0
	level = level or 3
	for i = level, 10 do
		local source = (thread and debug.getinfo(thread, i, 'S') or debug.getinfo(i, 'S'))
		if not source then break end
		local params = vars(i+1, records)
		if params and #params >0 then
			count = count + 1
			dump = dump .. '    -stack' .. mytostring(count) .. '\n'
			dump = dump .. params
		end
		if source.what == 'main' then break end
	end
	return stacktrack .. '\n' .. dump .. "\n"
end