--************************************************************************
--*                        歌词设置                                       *
--************************************************************************
local CConfigBase = require "config.ConfigBase"
local CLrcConfig = FLua.Class(CConfigBase, "CLrcConfig")

local LrcPanelFontNameKey = "_LrcPanelFontName"
local LrcPanelFontStyleKey = "_LrcPanelFontStyle"
local LrcPanelFontSizeKey = "_LrcPanelFontSize"

local LrcPanelPlayedCorKey = "_LrcPanelPlayedCor"
local LrcPanelUnPlayedCorKey = "_LrcPanelUnPlayedCor"
local LrcPanelPlayingCorKey = "_LrcPanelPlayingCor"

local LrcDeskFontNameKey = "_LrcDeskFontName"
local LrcDeskFontStyleKey = "_LrcDeskFontStyle"
local LrcDeskFontSizeKey = "_LrcDeskFontSize"
local LrcDeskPlayedCorKey = "_LrcDeskPlayedCor"
local LrcDeskUnPlayedCorKey = "_LrcDeskUnPlayedCor"

local LrcFontStyle = 
{
	NormalFont = 0,
	BoldFont = 1,
	ItalicFont = 2,
	BoldItalicFont = 3,
}

function CLrcConfig:__constructor()
	self.m_sLrcPanelFontName = ""
	self.m_dLrcPanelFontStyle = LrcFontStyle.NormalFont
	self.m_dLrcPanelFontSize = 10
	self.m_dLrcPanelPlayedCor = 0
	self.m_dLrcPanelUnPlayedCor = 0
	self.m_dLrcPanelPlayingCor = 0
	
	self.m_sLrcDeskFontName = ""
	self.m_dLrcDeskFontStyle = LrcFontStyle.NormalFont
	self.m_dLrcDeskFontSize = 50
	self.m_dLrcDeskPlayedCor = 0
	self.m_dLrcDeskUnPlayedCor = 0
end
function CLrcConfig:Load()
	self:UpdateDB()
	local theDB = self:GetDB()
	assert(theDB)
	if not theDB then
		return false
	end
	self.m_sLrcPanelFontName = theDB:GetDictString(LrcPanelFontNameKey)
	self.m_dLrcPanelFontStyle = theDB:GetDictInt(LrcPanelFontStyleKey)
	self.m_dLrcPanelFontSize  = theDB:GetDictInt(LrcPanelFontSizeKey)
	self.m_dLrcPanelPlayedCor  = theDB:GetDictInt(LrcPanelPlayedCorKey)
	self.m_dLrcPanelUnPlayedCor  = theDB:GetDictInt(LrcPanelUnPlayedCorKey)
	self.m_dLrcPanelPlayingCor  = theDB:GetDictInt(LrcPanelPlayingCorKey)

	self.m_sLrcDeskFontName = theDB:GetDictString(LrcDeskFontNameKey)
	self.m_dLrcDeskFontStyle = theDB:GetDictInt(LrcDeskFontStyleKey)
	self.m_dLrcDeskFontSize  = theDB:GetDictInt(LrcDeskFontSizeKey)
	self.m_dLrcDeskPlayedCor  = theDB:GetDictInt(LrcDeskPlayedCorKey)
	self.m_dLrcDeskUnPlayedCor  = theDB:GetDictInt(LrcDeskUnPlayedCorKey)

	return true
end
function CLrcConfig:Save()
	local theDB = self:GetDB()
	assert(theDB)
	if not theDB then
		return false
	end
	if not self:IsModify() then
		return true
	end

	local keyValues =
	{
		{LrcPanelFontNameKey,self.m_sLrcPanelFontName, "InsertDictString"},
		{LrcDeskFontNameKey,self.m_sLrcDeskFontName, "InsertDictString"},
		{LrcPanelFontStyleKey,self.m_dLrcPanelFontStyle, "InsertDictInt"},
		{LrcPanelFontSizeKey,self.m_dLrcPanelFontSize, "InsertDictInt"},
		{LrcPanelPlayedCorKey,self.m_dLrcPanelPlayedCor, "InsertDictInt"},
		{LrcPanelPlayingCorKey,self.m_dLrcPanelPlayingCor, "InsertDictInt"},
		{LrcPanelUnPlayedCorKey,self.m_dLrcPanelUnPlayedCor, "InsertDictInt"},
		{LrcDeskFontStyleKey,self.m_dLrcDeskFontStyle, "InsertDictInt"},
		{LrcDeskFontSizeKey,self.m_dLrcDeskFontSize, "InsertDictInt"},
		{LrcDeskPlayedCorKey,self.m_dLrcDeskPlayedCor, "InsertDictInt"},
		{LrcDeskUnPlayedCorKey,self.m_dLrcDeskUnPlayedCor, "InsertDictInt"}
	}

	theDB:BeginTransaction()
	for i=1, #keyValues do
		theDB:DeleteDict(keyValues[i][1])
		local func = theDB:tryget(keyValues[i][3])
		func(theDB, keyValues[i][1], keyValues[i][2])
	end
	theDB:Commit()
	self:Modify(false)

	return true
end

function CLrcConfig:GetLrcPanelFontName()
	return self.m_sLrcPanelFontName
end
function CLrcConfig:SetLrcPanelFontName(value)
	if self.m_sLrcPanelFontName ~= value then
		self.m_sLrcPanelFontName = value
		self:Modify(true)
	end
end

function CLrcConfig:GetLrcPanelFontStyle()
	return self.m_dLrcPanelFontStyle
end
function CLrcConfig:SetLrcPanelFontStyle(value)
	if self.m_dLrcPanelFontStyle ~= value then
		self.m_dLrcPanelFontStyle = value
		self:Modify(true)
	end
end

function CLrcConfig:GetLrcPanelFontSize()
	return self.m_dLrcPanelFontSize
end
function CLrcConfig:SetLrcPanelFontSize(value)
	if self.m_dLrcPanelFontSize ~= value then
		self.m_dLrcPanelFontSize = value
		self:Modify(true)
	end
end

function CLrcConfig:GetLrcPanelPlayedColor()
	return self.m_dLrcPanelPlayedCor
end
function CLrcConfig:GetLrcPanelPlayedColor(value)
	if self.m_dLrcPanelPlayedCor ~= value then
		self.m_dLrcPanelPlayedCor = value
		self:Modify(true)
	end
end

function CLrcConfig:GetLrcPanelUnPlayedColor()
	return self.m_dLrcPanelUnPlayedCor
end
function CLrcConfig:SetLrcPanelUnPlayedColor(value)
	if self.m_dLrcPanelUnPlayedCor ~= value then
		self.m_dLrcPanelUnPlayedCor = value
		self:Modify(true)
	end
end

function CLrcConfig:GetLrcPanelPlayingColor()
	return self.m_dLrcPanelPlayingCor
end
function CLrcConfig:SetLrcPanelPlayingColor(value)
	if self.m_dLrcPanelPlayingCor ~= value then
		self.m_dLrcPanelPlayingCor = value
		self:Modify(true)
	end
end

function CLrcConfig:GetLrcDeskFontName()
	return self.m_sLrcDeskFontName
end
function CLrcConfig:SetLrcDeskFontName(value)
	if self.m_sLrcDeskFontName ~= value then
		self.m_sLrcDeskFontName = value
		self:Modify(true)
	end
end

function CLrcConfig:GetLrcDeskFontStyle()
	return self.m_dLrcDeskFontStyle
end
function CLrcConfig:SetLrcDeskFontStyle(value)
	if self.m_dLrcDeskFontStyle ~= value then
		self.m_dLrcDeskFontStyle = value
		self:Modify(true)
	end
end

function CLrcConfig:GetLrcDeskFontSize()
	return self.m_dLrcDeskFontSize
end
function CLrcConfig:SetLrcDeskFontSize(value)
	if self.m_dLrcDeskFontSize ~= value then
		self.m_dLrcDeskFontSize = value
		self:Modify(true)
	end
end

function CLrcConfig:GetLrcDeskPlayedColor()
	return self.m_dLrcDeskPlayedCor
end
function CLrcConfig:SetLrcDeskPlayedColor(value)
	if self.m_dLrcDeskPlayedCor ~= value then
		self.m_dLrcDeskPlayedCor = value
		self:Modify(true)
	end
end

function CLrcConfig:GetLrcDeskUnPlayedColor()
	return self.m_dLrcDeskUnPlayedCor
end
function CLrcConfig:SetLrcDeskUnPlayedColor(value)
	if self.m_dLrcDeskUnPlayedCor ~= value then
		self.m_dLrcDeskUnPlayedCor = value
		self:Modify(true)
	end
end

return CLrcConfig