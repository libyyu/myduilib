local CDatabase = require "database.Database"
local CGlobalDBase = FLua.Class(CDatabase, "CGlobalDBase")

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
function CGlobalDBase:__ctor()
	self.UserInfoTableName = "user_lists"
	self.LastLogonUserKey = "_lastLogonUser"
end

function CGlobalDBase:_CreateUserTable()
	printf("[Begin]创建用户列表<table:%s>----数据库", self.UserInfoTableName)
	local sSql = [[create table %s (
	id Integer primary key autoincrement,
	user_id text unique,
	email text unique,
	password text,
	nick_name text,
	keep_logged int);]]
	if not self:IsExistTable(self.UserInfoTableName) then
		self:ExecSql(sSql, self.UserInfoTableName)
	end
	if not self:IsExistTable(self.UserInfoTableName) then
		errorf("[End]创建用户列表<table:%s>----数据库",self.UserInfoTableName)
		return false
	end

	printf("[End]创建用户列表<table:%s>----数据库",self.UserInfoTableName)
	return true
end

function CGlobalDBase:GetAllUserInfo()
	printf("[Begin]获取用户列表<table:%s>---数据库",self.UserInfoTableName)
	local sSql = "select user_id,email,password,nick_name,keep_logged from %s;"
	local rs = self:QuerySql(sSql,self.UserInfoTableName)

	local LogonUserInfoT = require "data.LogonUserInfo"
	local UserInfoList = {}
	while true do
		if not rs or rs:eof() then
			break
		end 

		local info = LogonUserInfoT()
		info.sUserID      = rs:getStringField(0)
		info.sLogonEmail  = rs:getStringField(1)
		info.sPasswordMD5 = rs:getStringField(2)
		info.sNickName    = rs:getStringField(3)
		info.bKeepLogged  = rs:getIntField(4) ~= 0 and true or false

		rs:nextRow()

		UserInfoList[#UserInfoList+1] = info
	end
	printf("[End]获取用户列表<table:%s>----数据库",self.UserInfoTableName)
	return UserInfoList
end

function CGlobalDBase:GetUserInfo(sUserID)
	printf("[Begin]获取用户信息<table:%s><user_id:%s>----数据库",self.UserInfoTableName,sUserID)
	local sSql = "select user_id,email,password,nick_name,keep_logged from %s where user_id = %s;"
	local rs = self:QuerySql(sSql,self.UserInfoTableName,sUserID)

	local LogonUserInfoT = require "data.LogonUserInfo"
	local info = nil
	if not rs:eof() then
		info = LogonUserInfoT()

		info.sUserID      = rs:getStringField(0)
		info.sLogonEmail  = rs:getStringField(1)
		info.sPasswordMD5 = rs:getStringField(2)
		info.sNickName    = rs:getStringField(3)
		info.bKeepLogged  = rs:getIntField(4) ~= 0 and true or false
	end
	printf("[End]获取用户信息<table:%s><user_id:%s>----数据库",self.UserInfoTableName,sUserID)

	return info
end

function CGlobalDBase:AddUserInfo(userInfo)
	if not userInfo then return false end
	printf("[Begin]添加用户信息<table:%s><user_id:%s>----数据库",self.UserInfoTableName,userInfo.sUserID)
	local sSql = "insert into %s(user_id ,email,password,nick_name,keep_logged) values (%s,%s,%s,%s,%u);"
	local nErrCode = self:ExecSql(
		sSql,
		self.UserInfoTableName,
		userInfo.sUserID,
		userInfo.sLogonEmail,
		userInfo.sPasswordMD5,
		userInfo.sNickName,
		userInfo.bKeepLogged and 1 or 0)
	printf("[End]添加用户信息<table:%s><user_id:%s>----数据库",self.UserInfoTableName,userInfo.sUserID)

	return -1 ~= nErrCode
end

function CGlobalDBase:UpdateUserInfo(userInfo)
	if not userInfo then return false end
	printf("[Begin]更新用户信息<table:%s><user_id:%s>----数据库",self.UserInfoTableName,userInfo.sUserID)
	local sSql = "update %s set email = %s, password = %s,nick_name = %s,keep_logged = %u where user_id = %s;"

	local nErrCode = self:ExecSql(
		sSql,
		self.UserInfoTableName,
		userInfo.sLogonEmail,
		userInfo.sPasswordMD5,
		userInfo.sNickName,
		userInfo.bKeepLogged and 1 or 0,
		userInfo.sUserID)
	printf("[End]更新用户信息<table:%s><user_id:%s>----数据库",self.UserInfoTableName,userInfo.sUserID)

	return -1 ~= nErrCode
end

function CGlobalDBase:RemoveUserInfo(userID)
	printf("[Begin]移除用户信息<table:%s><user_id:%s>----数据库",self.UserInfoTableName,tostring(userID))
	local sSql = "delete from %s where user_id = %s;"

	local nErrCode = self:ExecSql(
		sSql,
		self.UserInfoTableName,
		tostring(userID))
	printf("[End]移除用户信息<table:%s><user_id:%s>----数据库",self.UserInfoTableName,tostring(userID))

	return -1 ~= nErrCode
end

function CGlobalDBase:GetLastUserInfo()
	printf("[Begin]获取最后一个登录用户信息<table:%s>----数据库",self.UserInfoTableName);
	local sValue = ""
	local nUserID = 0
	local sUserID = ""
	sValue = self:GetDictString(self.LastLogonUserKey)
	if sValue and string.len(sValue) >0 then
	
		nUserID = tonumber(sValue)
		sUserID = sValue
		sValue.ReleaseBuffer();
	end

	local LogonUserInfoT = require "data.LogonUserInfo"
	if("" == sUserID) then
		warnf("[End]获取最后一个登录用户信息<table:%s>----数据库",self.UserInfoTableName);
		return LogonUserInfoT()
	end

	printf("[End]获取最后一个登录用户信息<table:%s>----数据库",self.UserInfoTableName);
	return self:GetUserInfo(sUserID)
end

function CGlobalDBase:UpdateLastUserInfo(userInfo)
	assert(userInfo)
	if not userInfo then return false end
	printf("[Begin]更新最后一个登录用户信息<table:%s><user_id:%s>----数据库",self.UserInfoTableName,userInfo.sUserID)
	self:DeleteDict(self.LastLogonUserKey)
	printf("[End]更新最后一个登录用户信息<table:%s><user_id:%s>----数据库",self.UserInfoTableName,userInfo.sUserID)
	return self:InsertDict(self.LastLogonUserKey, userInfo.sUserID)
end

function CGlobalDBase:OnCreateTables()
	return self:_CreateUserTable()
end

function CGlobalDBase:OnUpdateTables(oldVer, newVer)
	return true
end

return CGlobalDBase

