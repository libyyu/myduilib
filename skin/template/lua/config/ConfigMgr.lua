local CConfigMgr = FLua.Class("CConfigMgr")

function CConfigMgr:__ctor()
	self.m_pNormal = nil
	self.m_pAlert = nil
	self.m_pHotKey = nil
	self.m_pCache = nil
	self.m_pSkin = nil
	self.m_pUpdate = nil
	self.m_pLrc = nil
end
function CConfigMgr:Load()
	self:GetNormalConfig():Load()
	self:GetAlertConfig():Load()
	self:GetHotKeyConfig():Load()
	self:GetCacheConfig():Load()
	self:GetSkinConfig():Load()
	self:GetLrcConfig():Load()
end

function CConfigMgr:GetNormalConfig()
	local NormalConfigT = require "config.NormalConfig"
	if not self.m_pNormal then
		self.m_pNormal = NormalConfigT()
	end
	return self.m_pNormal
end
function CConfigMgr:GetAlertConfig()
	local AlertConfigT = require "config.AlertConfig"
	if not self.m_pAlert then
		self.m_pAlert = AlertConfigT()
	end
	return self.m_pAlert
end
function CConfigMgr:GetHotKeyConfig()
	local HotKeyConfigT = require "config.HotKeyConfig"
	if not self.m_pHotKey then
		self.m_pHotKey = HotKeyConfigT()
	end
	return self.m_pHotKey
end
function CConfigMgr:GetCacheConfig()
	local CacheConfigT = require "config.CacheConfig"
	if not self.m_pCache then
		self.m_pCache = CacheConfigT()
	end
	return self.m_pCache
end
function CConfigMgr:GetSkinConfig()
	local SkinConfigT = require "config.SkinConfig"
	if not self.m_pSkin then
		self.m_pSkin = SkinConfigT()
	end
	return self.m_pSkin
end
function CConfigMgr:GetUpdateConfig()
	local UpdateConfigT = require "config.UpdateConfig"
	if not self.m_pUpdate then
		self.m_pUpdate = UpdateConfigT()
	end
	return self.m_pUpdate
end
function CConfigMgr:GetLrcConfig()
	local LrcConfigT = require "config.LrcConfig"
	if not self.m_pLrc then
		self.m_pLrc = LrcConfigT()
	end
	return self.m_pLrc
end

return CConfigMgr