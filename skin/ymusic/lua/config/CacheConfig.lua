--************************************************************************
--*                        提醒设置                                       *
--************************************************************************
local CConfigBase = require "config.ConfigBase"
local CCacheConfig = FLua.Class(CConfigBase, "CCacheConfig")

local CacheDirKey = "_CacheDirKey"
local MaxCacheKey = "_MaxCacheKey"
local MaxCacheSize = 1024
local MinCacheSize = 512

function CCacheConfig:__constructor()
	self.m_sCacheDir = theApp:GetRuntimeState():GetCacheMgr():GetCacheDir()
	self.m_sMaxCacheSpace = 512
end
function CCacheConfig:Load()
	self:UpdateDB()
	local theDB = self:GetDB()
	assert(theDB)
	if not theDB then
		return false
	end
	self.m_sCacheDir = theDB:GetDictString(CacheDirKey)
	self.m_sMaxCacheSpace = theDB:GetDictInt(MaxCacheKey)
	if not self.m_sMaxCacheSpace or self.m_sMaxCacheSpace < MinCacheSize then
		self.m_sMaxCacheSpace = MinCacheSize
	end
	return true
end
function CCacheConfig:Save()
	local theDB = self:GetDB()
	assert(theDB)
	if not theDB then
		return false
	end
	if not self:IsModify() then
		return true
	end

	theDB:BeginTransaction()
	theDB:DeleteDict(CacheDirKey)
	theDB:InsertDictString(CacheDirKey, self.m_sCacheDir)
	theDB:DeleteDict(MaxCacheKey)
	theDB:InsertDictInt(MaxCacheKey, self.m_sMaxCacheSpace)

	theDB:Commit()
	self:Modify(false)

	return true
end

function CCacheConfig:GetCacheDir()
	return self.m_sCacheDir
end

function CCacheConfig:SetCacheDir(spath)
	local sDir = spath:gsub("\\", "/")
	if sDir:sub(-1,-1) ~= "/" then
		sDir = sDir .. "/"
	end

	if self.m_sCacheDir ~= sDir then
		theApp:GetRuntimeState():GetCacheMgr():ClearAllCatch()
		self.m_sCacheDir = sDir
		theApp:GetRuntimeState():GetCacheMgr():Init(sDir)
		self:Modify(true)
	end
end

function CCacheConfig:GetMaxCacheSpace()
	return self.m_sMaxCacheSpace
end

function CCacheConfig:SetMaxCacheSpace(nMax)
	if nMax < MinCacheSize then
		nMax = MinCacheSize
	elseif nMax > MaxCacheSize then
		nMax = MaxCacheSize
	end
	if self.m_sMaxCacheSpace ~= nMax then
		self.m_sMaxCacheSpace = nMax
		self:Modify(true)
	end
end

return CCacheConfig