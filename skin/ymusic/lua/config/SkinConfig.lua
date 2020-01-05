--************************************************************************
--*                        提醒设置                                       *
--************************************************************************
local CConfigBase = require "config.ConfigBase"
local CSkinConfig = FLua.Class(CConfigBase, "CSkinConfig")

local SkinCorKey = "_SkinCor"
local SkinBgKey = "_SkinBgImage"
local SkinUnityBgKey = "_SkinEnableUnityBg"
local SkinStyleKey  = "_SkinStyle"
local SkinItemBgKey = "_SkinItemBg"
local SkinItemHoverBgKey = "_SkinItemHoverBg"
local SkinItemPlayBgKey  = "_SkinItemPlayBg"

function CSkinConfig:__ctor()
	self.m_dwCor = 0xFFFFFFFF
	self.m_bEnableUnityBg = false

	self.m_sStyle = ""
	self.m_sBg = ""
	self.m_sItemBg = ""
	self.m_sItemHoverBg = ""
	self.m_sItemPlayBg = ""
end

function CSkinConfig:Load()
	self:UpdateDB()
	local theDB = self:GetDB()
	assert(theDB)
	if not theDB then
		return false
	end
	self.m_dwCor = theDB:GetDictInt(SkinCorKey)
	self.m_bEnableUnityBg = theDB:GetDictBool(SkinUnityBgKey)
	self.m_sStyle = theDB:GetDictString(SkinStyleKey)
	self.m_sBg = theDB:GetDictString(SkinBgKey)
	self.m_sItemBg = theDB:GetDictString(SkinItemBgKey)
	self.m_sItemHoverBg = theDB:GetDictString(SkinItemHoverBgKey)
	self.m_sItemPlayBg = theDB:GetDictString(SkinItemPlayBgKey)
	return true
end
function CSkinConfig:Save()
	local theDB = self:GetDB()
	assert(theDB)
	if not theDB then
		return false
	end
	if not self:IsModify() then
		return true
	end

	theDB:BeginTransaction()

	theDB:DeleteDict(SkinCorKey)
	theDB:DeleteDict(SkinBgKey)
	theDB:DeleteDict(SkinUnityBgKey)
	theDB:DeleteDict(SkinStyleKey)
	theDB:DeleteDict(SkinItemBgKey)
	theDB:DeleteDict(SkinItemHoverBgKey)
	theDB:DeleteDict(SkinItemPlayBgKey)

	theDB:InsertDictInt(SkinCorKey, self.m_dwCor)
	theDB:InsertDictString(SkinBgKey, self.m_sBg)
	theDB:InsertDictBool(SkinUnityBgKey, self.m_bEnableUnityBg)
	theDB:InsertDictString(SkinStyleKey, self.m_sStyle)
	theDB:InsertDictString(SkinItemBgKey, self.m_sItemBg)
	theDB:InsertDictString(SkinItemHoverBgKey, self.m_sItemHoverBg)
	theDB:InsertDictString(SkinItemPlayBgKey, self.m_sItemPlayBg)

	theDB:Commit()
	self:Modify(false)

	return true
end

function CSkinConfig:GetColor()
	return self.m_dwCor
end
function CSkinConfig:SetColor(value)
	if self.m_dwCor ~= value then
		self.m_dwCor = value
		self.m_sBg = ""
		self:Modify(true)
	end
end

function CSkinConfig:GetBg()
	return self.m_sBg
end
function CSkinConfig:SetBg(value)
	if self.m_sBg ~= value then
		self.m_sBg = value
		self:Modify(true)
	end
end

function CSkinConfig:GetStyleName()
	return self.m_sStyle
end
function CSkinConfig:SetStyleName(value)
	if self.m_sStyle ~= value then
		self.m_sStyle = value
		self:Modify(true)
	end
end

function CSkinConfig:IsEnableUnityBg()
	return self.m_bEnableUnityBg
end
function CSkinConfig:EnableUnityBg(value)
	if self.m_bEnableUnityBg ~= value then
		self.m_bEnableUnityBg = value
		self:Modify(true)
	end
end

function CSkinConfig:GetItemBg()
	return self.m_sItemBg
end
function CSkinConfig:SetItemBg(value)
	if self.m_sItemBg ~= value then
		self.m_sItemBg = value
		self:Modify(true)
	end
end

function CSkinConfig:GetItemHoverBg()
	return self.m_sItemHoverBg
end
function CSkinConfig:SetItemHoverBg(value)
	if self.m_sItemHoverBg ~= value then
		self.m_sItemHoverBg = value
		self:Modify(true)
	end
end

function CSkinConfig:GetItemPlayBg()
	return self.m_sItemPlayBg
end
function CSkinConfig:GetItemPlayBg(value)
	if self.m_sItemPlayBg ~= value then
		self.m_sItemPlayBg = value
		self:Modify(true)
	end
end

return CSkinConfig