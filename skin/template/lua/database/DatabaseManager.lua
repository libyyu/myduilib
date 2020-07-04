local CDatabaseManager = FLua.Class("CDatabaseManager")

USER_DB = "user_%s.sl"
DEFAULT_DB = "default.sl"
GLOBEL_DB = "globel.sl"
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

function CDatabaseManager:__ctor()
	self.m_spUserInfo = nil
	self.m_sDatabasePath = ""

	self.m_spGlobalDBase = nil 
	self.m_spUserDBase = nil 
	self.m_spDefaultUserDBase = nil
end

function CDatabaseManager:SetDatabasePath(sDatabasePath)
	if not sDatabasePath or string.len(sDatabasePath) == 0 then
		return
	end
	
	self.m_sDatabasePath = sDatabasePath:gsub("\\", "/")
	if self.m_sDatabasePath:sub(-1,-1) ~= "/" then
		self.m_sDatabasePath = self.m_sDatabasePath .. "/"
	end

	assert(Application.CreateDirectory(sDatabasePath))
end
function CDatabaseManager:GetDatabasePath()
	return self.m_sDatabasePath
end

function CDatabaseManager:SetCurrentUser(spUserInfo)
	self.m_spUserInfo = spUserInfo
	self.m_spUserDBase = nil
end

function CDatabaseManager:GetGlobalDBase()
	if self.m_spGlobalDBase then
		return self.m_spGlobalDBase
	end
	printf("开始创建全局数据库----数据库")
	if not self.m_spGlobalDBase and string.len(self.m_sDatabasePath) > 0 then
		local sDBFileName = self.m_sDatabasePath .. GLOBEL_DB
		local GlobalDatabaseT = require "database.GlobalDatabase"
		local spGlobalDBase = GlobalDatabaseT()
		if spGlobalDBase:OpenDB(sDBFileName) then
			self.m_spGlobalDBase = spGlobalDBase
		else
			spGlobalDBase:Release()
			spGlobalDBase = nil
		end
	end
	printf("结束创建全局数据库----数据库");
	return self.m_spGlobalDBase
end
function CDatabaseManager:GetUserDBase()
	if self.m_spUserDBase then
		return self.m_spUserDBase
	end
	printf("开始创建用户列表----数据库")
	if not self.m_spUserDBase and self.m_spUserInfo and string.len(self.m_sDatabasePath) > 0 then
		local sDBFileName = self.m_sDatabasePath;
		local sName = string.format(USER_DB, self.m_spUserInfo.sUserID)
		sDBFileName = sDBFileName .. sName

		local UserDatabaseT = require "database.UserDatabase"
		local spUserDBase = UserDatabaseT()
		if spUserDBase:OpenDB(sDBFileName) then
			self.m_spUserDBase = spUserDBase
		else
			spUserDBase:Release()
			spUserDBase = nil
		end
	end
	printf("结束创建用户列表----数据库")
	return self.m_spUserDBase
end
function CDatabaseManager:GetDefaultUserDBase()
	if self.m_spDefaultUserDBase then
		return self.m_spDefaultUserDBase
	end
	printf("开始创建默认列表----数据库")
	if not self.m_spDefaultUserDBase and string.len(self.m_sDatabasePath) > 0 then
		local sDBFileName = self.m_sDatabasePath .. DEFAULT_DB
		
		local UserDatabaseT = require "database.UserDatabase"
		local spUserDBase = UserDatabaseT()
		if spUserDBase:OpenDB(sDBFileName) then
			self.m_spDefaultUserDBase = spUserDBase
		else
			spUserDBase:Release()
			spUserDBase = nil
		end
	end
	printf("结束创建默认列表----数据库")
	return self.m_spDefaultUserDBase
end

function CDatabaseManager:GetUserDB()
	if self.m_spUserInfo then
		return self:GetUserDBase()
	end
	return self:GetDefaultUserDBase()
end

return CDatabaseManager