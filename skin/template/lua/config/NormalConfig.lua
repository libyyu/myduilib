--************************************************************************
--*                        常规设置                                       *
--************************************************************************
local CConfigBase = require "config.ConfigBase"
local CNormalConfig = FLua.Class(CConfigBase, "CNormalConfig")

local CloseModeKey= "_CloseMode" --关闭按钮行为
local PlayModeKey = "_PlayMode" --播放模式
local VolumeKey = "_Volume" --声音大小
local VolumeOffKey = "_VolumeOff" --声音开关
local KaraokModeKey = "_KaraokMode" --卡拉ok模式
local AutoLoginKey = "_AutoLoginKey" --自动登陆
local AutoStartKey = "_AutoStartKey" --开机启动
local AutoPlayKey = "_AutoPlayKey" --自动播放
local AutoLrcKey = "_AutoOpenLrcKey" --自动打开桌面歌词
local InsertModeKey = "_InsertModeKey" --插入方式
local PlayRoamingKey = "_PlayRoaming" --动播放漫游列表		
local ShowInfoKey = "_AutoShowInfoKey"
local RelateAACKey = "_AACKey"
local RelateAPEKey = "_APEKey"
local RelateFLACKey = "_FLACKey"
local RelateMP3Key = "_MP3Key"
local RelateOGGKey = "_OGGKey"
local RelateWAVKey = "_WAVKey"
local EnableUrlProtoKey = "_EnableUrlProtoKey"

function CNormalConfig:__ctor()
	self.m_bToTray = true --关闭到托盘
	self.m_bVolOff = false
	self.m_nVol =100
	self.m_nPlayMode = 0
	self.m_bKaraok = true
	self.m_bAutoLogin = true
	self.m_bAutoStart = true
	self.m_bAutoPlay = false
	self.m_bAutoLrc = false
	self.m_nInsertMode = 2
	self.m_bShowInfo = false
	self.m_bRelateAAC = false
	self.m_bRelateAPE = false
	self.m_bRelateFLAC = false
	self.m_bRelateMP3 = false
	self.m_bRelateOGG = false
	self.m_bRelateWAV = false	
	self.m_bEnableUrlProto = false
	self.m_bPlayRoaming = true
	self.m_sAppName = self:GetAppName()
end
function CNormalConfig:Load()
	self:UpdateDB()
	local theDB = self:GetDB()
	assert(theDB)
	if not theDB then
		return false
	end

	self.m_bToTray = theDB:GetDictBool(CloseModeKey)
	self.m_nPlayMode = theDB:GetDictInt(PlayModeKey) --播放模式
	self.m_bVolOff = theDB:GetDictBool(VolumeOffKey)
	self.m_nVol = theDB:GetDictInt(VolumeKey) --音量
	self.m_bAutoLogin = theDB:GetDictBool(AutoLoginKey) --自动登录
	self.m_bAutoStart = theDB:GetDictBool(AutoStartKey) --开机启动
	self.m_bAutoPlay = theDB:GetDictBool(AutoPlayKey) --自动播放
	self.m_bAutoLrc = theDB:GetDictBool(AutoLrcKey) --自动下载歌词
	self.m_bKaraok = theDB:GetDictBool(KaraokModeKey)
	self.m_nInsertMode = theDB:GetDictInt(InsertModeKey) 

	self.m_bShowInfo = theDB:GetDictBool(ShowInfoKey)
	self.m_bRelateAAC = theDB:GetDictBool(RelateAACKey)
	self.m_bRelateAPE = theDB:GetDictBool(RelateAPEKey)
	self.m_bRelateFLAC = theDB:GetDictBool(RelateFLACKey)
	self.m_bRelateMP3 = theDB:GetDictBool(RelateMP3Key)
	self.m_bRelateOGG = theDB:GetDictBool(RelateOGGKey)
	self.m_bRelateWAV = theDB:GetDictBool(RelateWAVKey)
	self.m_bPlayRoaming = theDB:GetDictBool(PlayRoamingKey)

	return true
end
function CNormalConfig:Save()
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
		{VolumeKey,self.m_nVol, "InsertDictInt"},
		{VolumeOffKey,self.m_bVolOff, "InsertDictBool"},
		{PlayModeKey,self.m_nPlayMode, "InsertDictInt"},
		{KaraokModeKey,self.m_bKaraok, "InsertDictBool"},
		{AutoLoginKey,self.m_bAutoLogin, "InsertDictBool"},
		{AutoStartKey,self.m_bAutoStart, "InsertDictBool"},
		{AutoPlayKey,self.m_bAutoPlay, "InsertDictBool"},
		{AutoLrcKey,self.m_bAutoLrc, "InsertDictBool"},
		{InsertModeKey,self.m_nInsertMode, "InsertDictInt"},
		{ShowInfoKey,self.m_bShowInfo, "InsertDictBool"},
		{RelateAACKey,self.m_bRelateAAC, "InsertDictBool"},
		{RelateAPEKey,self.m_bRelateAPE, "InsertDictBool"},
		{RelateFLACKey,self.m_bRelateFLAC, "InsertDictBool"},
		{RelateMP3Key,self.m_bRelateMP3, "InsertDictBool"},
		{RelateOGGKey,self.m_bRelateOGG, "InsertDictBool"},
		{RelateWAVKey,self.m_bRelateWAV, "InsertDictBool"},
		{PlayRoamingKey,self.m_bPlayRoaming, "InsertDictBool"},
		{CloseModeKey,self.m_bToTray, "InsertDictBool"}
	}
	theDB:BeginTransaction()
	for _, v in ipairs(keyValues) do
		theDB:DeleteDict(v[1])
		local func = theDB:tryget(v[3])
		func(theDB, v[1], v[2])
	end
	theDB:Commit()
	self:Modify(false)
	return true
end


function CNormalConfig:IsCloseToTray()
	return self.m_bToTray
end
function CNormalConfig:SetCloseMode(bToTray)
	if bToTray == nil then bToTray = true end
	if self.m_bToTray ~= bToTray then
		self.m_bToTray = bToTray
		self:Modify(true)
	end
end
function CNormalConfig:GetPlayMode()
	return self.m_nPlayMode
end
function CNormalConfig:SetPlayMode(nMode)
	if self.m_nPlayMode == nMode then
		return
	end
	self.m_nPlayMode = nMode
	local theDB = self:GetDB()
	assert(theDB)
	if not theDB then
		return
	end
	theDB:DeleteDict(PlayModeKey);
	theDB:InsertDictInt(PlayModeKey, nMode)
end
function CNormalConfig:IsVolumeOff()
	return self.m_bVolOff
end
function CNormalConfig:SetVolumeOff(bOff)
	if bOff == nil then bOff = true end
	if self.m_bVolOff == bOff then
		return
	end
	self.m_bVolOff = bOff
	local theDB = self:GetDB()
	assert(theDB)
	if not theDB then
		return
	end
	theDB:DeleteDict(VolumeOffKey);
	theDB:InsertDictBool(VolumeOffKey, bOff)
end
function CNormalConfig:GetVolume()
	return self.m_nVol
end
function CNormalConfig:SetVolume(nVol)
	if self.m_nVol == nVol then
		return
	end
	self.m_nVol = nVol
	local theDB = self:GetDB()
	assert(theDB)
	if not theDB then
		return
	end
	theDB:DeleteDict(VolumeKey);
	theDB:InsertDictInt(VolumeKey, nVol)
end
function CNormalConfig:IsKaraokMode()
	return self.m_bKaraok
end
function CNormalConfig:SetKaraokMode(bKaraok)
	if bKaraok == nil then bKaraok = true end
	if self.m_bKaraok ~= bKaraok then
		self.m_bKaraok = bKaraok
		self:Modify(true)
	end
end
function CNormalConfig:IsEnableAutoLogin()
	return self.m_bAutoLogin
end
function CNormalConfig:EnableAutoLogin(bEnable)
	if bEnable == nil then bEnable = true end
	if self.m_bAutoLogin ~= bEnable then
		self.m_bAutoLogin = bEnable
		self:Modify(true)
	end
end
function CNormalConfig:IsEnableAutoStart()
	return self.m_bAutoStart
end
function CNormalConfig:EnableAutoStart(bEnable)
	if bEnable == nil then bEnable = true end
	if self.m_bAutoStart ~= bEnable then
		self.m_bAutoStart = bEnable
		self:SendRunPacket(bEnable)
		self:Modify(true)
	end
end
function CNormalConfig:IsEnableAutoPlay()
	return self.m_bAutoPlay
end
function CNormalConfig:EnableAutoPlay(bEnable)
	if bEnable == nil then bEnable = true end
	if self.m_bAutoPlay ~= bEnable then
		self.m_bAutoPlay = bEnable
		self:Modify(true)
	end
end
function CNormalConfig:IsEnableAutoOpenLrc()
	return self.m_bAutoLrc
end
function CNormalConfig:EnableAutoOpenLrc(bEnable)
	if bEnable == nil then bEnable = true end
	if self.m_bAutoLrc ~= bEnable then
		self.m_bAutoLrc = bEnable
		self:Modify(true)
	end
end

-- // 		enum InsertMode
-- // 		{
-- // 			im_begin = 0,  //从列表头插入;
-- // 			im_current = 1,//从当前播放歌曲后插入;
-- // 			im_end = 2       //从末尾插入;
-- // 		};
function CNormalConfig:GetInsertMode()
	return self.m_nInsertMode
end
function CNormalConfig:SetInsertMode(nMode)
	if self.m_nInsertMode ~= nMode then
		self.m_nInsertMode = nMode
		self:Modify(true)
	end
end
function CNormalConfig:IsPlayRoaming()
	return self.m_bPlayRoaming
end
function CNormalConfig:EnablePlayRoaming(enable)
	if self.m_bPlayRoaming ~= enable then
		self.m_bPlayRoaming = enable
		self:Modify(true)
	end
end
function CNormalConfig:IsEnableAutoShowSongInfo()
	return self.m_bShowInfo
end
function CNormalConfig:EnableAutoShowSongInfo(bEnable)
	if bEnable == nil then bEnable = true end
	if self.m_bShowInfo ~= bEnable then
		self.m_bShowInfo = bEnable
		self:Modify(true)
	end
end
function CNormalConfig:IsEnableRelateAAC()
	return self.m_bRelateAAC
end
function CNormalConfig:EnableRelateAAC(bEnable)
	if bEnable == nil then bEnable = true end
	if self.m_bRelateAAC ~= bEnable then
		self.m_bRelateAAC = bEnable
		self:Modify(true)
			
		self:SendRelationPacket(".aac", bEnable)
		self:SendRelationPacket(".m4a", bEnable)
	end
end
function CNormalConfig:IsEnableRelateAPE()
	return self.m_bRelateAPE
end
function CNormalConfig:EnableRelateAPE(bEnable)
	if bEnable == nil then bEnable = true end
	if self.m_bRelateAPE ~= bEnable then
		self.m_bRelateAPE = bEnable
		self:Modify(true)
			
		self:SendRelationPacket(".ape", bEnable)
		self:SendRelationPacket(".cue", bEnable)
	end
end
function CNormalConfig:IsEnableRelateFLAC()
	return self.m_bRelateFLAC
end
function CNormalConfig:EnableRelateFLAC(bEnable)
	if bEnable == nil then bEnable = true end
	if self.m_bRelateFLAC ~= bEnable then
		self.m_bRelateFLAC = bEnable
		self:Modify(true)
			
		self:SendRelationPacket(".flac", bEnable)
	end
end
function CNormalConfig:IsEnableRelateMP3()
	return self.m_bRelateMP3
end
function CNormalConfig:EnableRelateMP3(bEnable)
	if bEnable == nil then bEnable = true end
	if self.m_bRelateMP3 ~= bEnable then
		self.m_bRelateMP3 = bEnable
		self:Modify(true)
			
		self:SendRelationPacket(".mp3", bEnable)
	end
end
function CNormalConfig:IsEnableRelateOGG()
	return self.m_bRelateOGG
end
function CNormalConfig:EnableRelateOGG(bEnable)
	if bEnable == nil then bEnable = true end
	if self.m_bRelateOGG ~= bEnable then
		self.m_bRelateOGG = bEnable
		self:Modify(true)
			
		self:SendRelationPacket(".ogg", bEnable)
	end
end
function CNormalConfig:IsEnableRelateWAV()
	return self.m_bRelateWAV
end
function CNormalConfig:EnableRelateWAV(bEnable)
	if bEnable == nil then bEnable = true end
	if self.m_bRelateWAV ~= bEnable then
		self.m_bRelateWAV = bEnable
		self:Modify(true)
			
		self:SendRelationPacket(".wav", bEnable)
	end
end
function CNormalConfig:IsAllRelate()
	return self:IsEnableRelateAAC() and
			self:IsEnableRelateAPE() and
			self:IsEnableRelateFLAC() and
			self:IsEnableRelateMP3() and
			self:IsEnableRelateOGG() and
			self:IsEnableRelateWAV()
end
function CNormalConfig:EnableAllRelate(bEnable)
	if bEnable == nil then bEnable = true end
	self:EnableRelateAAC(bEnable)
	self:EnableRelateAPE(bEnable)
	self:EnableRelateFLAC(bEnable)
	self:EnableRelateMP3(bEnable)
	self:EnableRelateOGG(bEnable)
	self:EnableRelateWAV(bEnable)
end
function CNormalConfig:IsEnableUrlProto()
	return self.m_bEnableUrlProto
end
function CNormalConfig:EnableUrlProto(bEnable)
	if bEnable == nil then bEnable = true end
	if bEnable ~= self.m_bEnableUrlProto then
		self.m_bEnableUrlProto = bEnable
		self:SendEnableUrlProtoPacket(bEnable)
	end
end
--private:
function CNormalConfig:SendAgentPacket(index, packet)
end
function CNormalConfig:SendRelationPacket(strExtension, bRelate)
end
function CNormalConfig:SendRunPacket(bAutoRun)
end
function CNormalConfig:SendEnableUrlProtoPacket(bEnable)
	if bEnable == nil then bEnable = true end
end

return CNormalConfig