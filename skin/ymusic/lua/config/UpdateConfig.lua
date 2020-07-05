--************************************************************************
--*                        提醒设置                                       *
--************************************************************************
local CConfigBase = require "config.ConfigBase"
local CUpdateConfig = FLua.Class(CConfigBase, "CUpdateConfig")

local AutoUpdateKey = "_AutoUpdate"

function CUpdateConfig:__constructor()
	self.m_autoUpdate = false
end
function CUpdateConfig:Load()
	self:UpdateDB()
	local theDB = self:GetDB()
	assert(theDB)
	if not theDB then
		return false
	end
	self.m_autoUpdate = theDB:GetDictBool(AutoUpdateKey)
	return true
end
function CUpdateConfig:Save()
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
		{AutoUpdateKey, self.m_autoUpdate},
	}

	theDB:BeginTransaction()
	for i=1, #keyValues do
		theDB:DeleteDict(keyValues[i][1])
		theDB:InsertDictBool(keyValues[i][1], keyValues[i][2])
	end
	theDB:Commit()
	self:Modify(false)

	return true
end

function CUpdateConfig:IsAutoUpdate()
	return self.m_autoUpdate
end

function CUpdateConfig:SetAutoUpdate(enable)
	if self.m_autoUpdate ~= enable then
		self.m_autoUpdate = enable
		self:Modify(true)
	end
end

return CUpdateConfig