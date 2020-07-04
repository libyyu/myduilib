local CDatabase = require "database.Database"
local CUserDBase = FLua.Class(CDatabase, "CUserDBase")

local function printf(fmt, ...)
	local v = ...
	if v then
		print(string.format(fmt, ...))
	else
		print(fmt)
	end
end
local function warnf(fmt, ...)
	local v = ...
	if v then
		warn(string.format(fmt, ...))
	else
		warn(fmt)
	end
end
local function errorf(fmt, ...)
	local v = ...
	if v then
		error(string.format(fmt, ...), 1)
	else
		error(fmt, 1)
	end
end
---------------------------------------------------------
function CUserDBase:__ctor()
end


function CUserDBase:OnCreateTables()
	return true
end

function CUserDBase:OnUpdateTables(oldVer, newVer)
	return true
end

return CUserDBase