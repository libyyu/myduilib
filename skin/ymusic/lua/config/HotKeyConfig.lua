--************************************************************************
--*                        热键设置                                       *
--************************************************************************
local CConfigBase = require "config.ConfigBase"
local CHotKeyConfig = FLua.Class(CConfigBase, "CHotKeyConfig")

local ChangeModeHotKey = "_ChangeModeKey"
local LrcWindowHotKey = "_OpenLrcKey"
local PlayControl = "_PlayControlKey"
local PlayPreSong = "_PlayPreSongKey"
local PlayNextSong = "_PlayNextSongKey"
local UpVolKey = "_UpVolKey"
local DownVolKey = "_DownVolKey"
local MusicLibWindowHotKey = "_OpenMusicLibKey"

function CHotKeyConfig:__ctor()
	self._dwChangeModeHotKey = 1617
	self._dwLrcWindowHotKey = 1604
	self._dwPlayControlHotKey = 1652
	self._dwPreSongHotKey = 3621
	self._dwNextSongHotKey = 3623
	self._dwUpVolHotKey = 3622
	self._dwDownVolHotKey = 3624
	self._dwMusiclibWindowHotKey = 3628
end
function CHotKeyConfig:Load()
	self:UpdateDB()
	local theDB = self:GetDB()
	assert(theDB)
	if not theDB then
		return false
	end
	self._dwChangeModeHotKey = theDB:GetDictInt(ChangeModeHotKey)
	self._dwLrcWindowHotKey = theDB:GetDictInt(LrcWindowHotKey)
	self._dwPlayControlHotKey  = theDB:GetDictInt(PlayControl)
	self._dwPreSongHotKey = theDB:GetDictInt(PlayPreSong)
	self._dwNextSongHotKey = theDB:GetDictInt(PlayNextSong)
	self._dwUpVolHotKey = theDB:GetDictInt(UpVolKey)
	self._dwDownVolHotKey = theDB:GetDictInt(DownVolKey)
	self._dwMusiclibWindowHotKey = theDB:GetDictInt(MusicLibWindowHotKey)
	return true
end
function CHotKeyConfig:Save()
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
		{ChangeModeHotKey,self._dwChangeModeHotKey},
		{LrcWindowHotKey,self._dwLrcWindowHotKey},
		{PlayControl,self._dwPlayControlHotKey},
		{PlayPreSong,self._dwPreSongHotKey},
		{PlayNextSong,self._dwNextSongHotKey},
		{UpVolKey,self._dwUpVolHotKey},
		{DownVolKey,self._dwDownVolHotKey},
		{MusicLibWindowHotKey,self._dwMusiclibWindowHotKey}
	}

	theDB:BeginTransaction()
	for i=1, #keyValues do
		theDB:DeleteDict(keyValues[i][1])
		theDB:InsertDictInt(keyValues[i][1], keyValues[i][2])
	end
	theDB:Commit()
	self:Modify(false)

	return true
end

function CHotKeyConfig:RegAllHotKey()
	if not self:RegHotKey(self._dwChangeModeHotKey) then
		self._dwChangeModeHotKey = 0
	end
	if not self:RegHotKey(self._dwLrcWindowHotKey) then
		self._dwLrcWindowHotKey = 0
	end
	if not self:RegHotKey(self._dwPlayControlHotKey) then
		self._dwPlayControlHotKey = 0
	end
	if not self:RegHotKey(self._dwPreSongHotKey) then
		self._dwPreSongHotKey = 0
	end
	if not self:RegHotKey(self._dwNextSongHotKey) then
		self._dwNextSongHotKey = 0
	end
	if not self:RegHotKey(self._dwUpVolHotKey) then
		self._dwUpVolHotKey = 0
	end
	if not self:RegHotKey(self._dwDownVolHotKey) then
		self._dwDownVolHotKey = 0
	end
	if not self:RegHotKey(self._dwMusiclibWindowHotKey) then
		self._dwMusiclibWindowHotKey = 0
	end
end

function CHotKeyConfig:RegHotKey(dwkey)
	return win32.RegisterHotKey(theApp:GetMainWindow():GetHWND(), dwkey)
end
function CHotKeyConfig:UnRegHotKey(dwkey)
	return win32.UnregisterHotKey(theApp:GetMainWindow():GetHWND(), dwkey)
end

function CHotKeyConfig:GetChangeModeHotKey()
	return self._dwChangeModeHotKey
end

function CHotKeyConfig:SetChangeModeHotKey(dwkey)
	if self._dwChangeModeHotKey ~= dwkey then
		--取消原先的注册热键;
		self:UnRegHotKey(self._dwChangeModeHotKey)
		self._dwChangeModeHotKey = dwkey
		self:RegHotKey(dwkey)
		self:Modify(true)
	end
end
function CHotKeyConfig:GetLrcWindowHotKey()
	return self._dwLrcWindowHotKey
end

function CHotKeyConfig:SetLrcWindowHotKey(dwkey)
	if self._dwLrcWindowHotKey ~= dwkey then
		--取消原先的注册热键;
		self:UnRegHotKey(self._dwLrcWindowHotKey)
		self._dwLrcWindowHotKey = dwkey
		self:RegHotKey(dwkey)
		self:Modify(true)
	end
end

function CHotKeyConfig:GetPlayControlHotKey()
	return self._dwPlayControlHotKey
end
function CHotKeyConfig:SetPlayControlHotKey(dwkey)
	if self._dwPlayControlHotKey ~= dwkey then
		--取消原先的注册热键;
		self:UnRegHotKey(self._dwPlayControlHotKey)
		self._dwPlayControlHotKey = dwkey
		self:RegHotKey(dwkey)
		self:Modify(true)
	end
end

function CHotKeyConfig:GetPlayPreHotKey()
	return self._dwPreSongHotKey
end
function CHotKeyConfig:SetPlayPreHotKey(dwkey)
	if self._dwPreSongHotKey ~= dwkey then
		--取消原先的注册热键;
		self:UnRegHotKey(self._dwPreSongHotKey)
		self._dwPreSongHotKey = dwkey
		self:RegHotKey(dwkey)
		self:Modify(true)
	end
end

function CHotKeyConfig:GetPlayNextHotKey()
	return self._dwNextSongHotKey
end
function CHotKeyConfig:SetPlayNextHotKey(dwkey)
	if self._dwNextSongHotKey ~= dwkey then
		--取消原先的注册热键;
		self:UnRegHotKey(self._dwNextSongHotKey)
		self._dwNextSongHotKey = dwkey
		self:RegHotKey(dwkey)
		self:Modify(true)
	end
end

function CHotKeyConfig:GetUpVolHotKey()
	return self._dwUpVolHotKey
end
function CHotKeyConfig:SetUpVolHotKey(dwkey)
	if self._dwUpVolHotKey ~= dwkey then
		--取消原先的注册热键;
		self:UnRegHotKey(self._dwUpVolHotKey)
		self._dwUpVolHotKey = dwkey
		self:RegHotKey(dwkey)
		self:Modify(true)
	end
end

function CHotKeyConfig:GetDownVolHotKey()
	return self._dwDownVolHotKey
end
function CHotKeyConfig:SetDownVolHotKey(dwkey)
	if self._dwDownVolHotKey ~= dwkey then
		--取消原先的注册热键;
		self:UnRegHotKey(self._dwDownVolHotKey)
		self._dwDownVolHotKey = dwkey
		self:RegHotKey(dwkey)
		self:Modify(true)
	end
end

function CHotKeyConfig:GetMusiclibHotKey()
	return self._dwMusiclibWindowHotKey
end
function CHotKeyConfig:SetMusiclibHotKey(dwkey)
	if self._dwMusiclibWindowHotKey ~= dwkey then
		--取消原先的注册热键;
		self:UnRegHotKey(self._dwMusiclibWindowHotKey)
		self._dwMusiclibWindowHotKey = dwkey
		self:RegHotKey(dwkey)
		self:Modify(true)
	end
end

return CHotKeyConfig