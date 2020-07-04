--************************************************************************
--*                        提醒设置                                       *
--************************************************************************
local CConfigBase = require "config.ConfigBase"
local CAlertConfig = FLua.Class(CConfigBase, "CAlertConfig")

local PrivateMsgKey = "_PrivateMsgKey"
local CommentMsgKey = "_CommentMsgKey"
local PraiseMsgKey = "_PraiseMsgKey"

function CAlertConfig:__ctor()
	self._enablePrivateMsg = false
	self._enableCommentMsg = false
	self._enablePraiseMsg  = false
end
function CAlertConfig:Load()
	self:UpdateDB()
	local theDB = self:GetDB()
	assert(theDB)
	if not theDB then
		return false
	end
	self._enablePrivateMsg = theDB:GetDictBool(PrivateMsgKey)
	self._enableCommentMsg = theDB:GetDictBool(CommentMsgKey)
	self._enablePraiseMsg  = theDB:GetDictBool(PraiseMsgKey)
	return true
end
function CAlertConfig:Save()
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
		{PrivateMsgKey, self._enablePrivateMsg},
		{CommentMsgKey,self._enableCommentMsg},
		{PraiseMsgKey,self._enablePraiseMsg}
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

function CAlertConfig:IsEnablePrivateMsg()
	return self._enablePrivateMsg
end
function CAlertConfig:EnablePrivateMsg(enable)
	if enable == nil then enable = true end
	if self._enableCommentMsg ~= enable then
		self._enableCommentMsg = enable
		self:Modify()
	end
end
function CAlertConfig:IsEnablePraiseMsg()
	return self._enablePraiseMsg
end
function CAlertConfig:EnablePraiseMsg(enable)
	if enable == nil then enable = true end
	if self._enablePraiseMsg ~= enable then
		self._enablePraiseMsg = enable
		self:Modify()
	end
end
function CAlertConfig:IsEnableCommentMsg()
	return self._enableCommentMsg
end
function CAlertConfig:EnableCommentMsg(enable)
	if enable == nil then enable = true end
	if self._enableCommentMsg ~= enable then
		self._enableCommentMsg = enable
		self:Modify()
	end
end

return CAlertConfig
