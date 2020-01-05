local CAVPlayer = FLua.Class("CAVPlayer")

_G.em_play_status =
{
	em_stop = 0,
	em_play = 1,
	em_pause = 2,
};
local thePlayer
function CAVPlayer:__ctor()
	self._pPlayer = nil
	self._StreamInfo = nil
	thePlayer = self
end

function CAVPlayer:Init()
	self:Release()
	self._pPlayer = AVPlayer.New({
		OnPlaying = function(event) self:OnPlaying(event) end,
		OnPosChanged = function(event) self:OnPosChanged(event) end,
		OnEndReached = function(event) self:OnEndReached(event) end,
	})
	return self:IsValid()
end
function CAVPlayer:Release()
	if self._pPlayer then
		self._pPlayer:Release()
		self._pPlayer = nil
	end
end
function CAVPlayer:IsValid()
	assert(self._pPlayer)
	return self._pPlayer:IsValid()
end

function CAVPlayer:PlayFile(strFile, strURL)
	print("Play file=", strFile, "url=", strURL and strURL or "<local>")
	assert(self._pPlayer)
	local ret
	local isLocal
	if win32.IsFileExist(strFile) then
		ret = self._pPlayer:PlayFile(strFile)
		isLocal = true
	else
		assert(strURL)
		isLocal = false
		ret = self._pPlayer:PlayURL(strURL)
	end	
	self._StreamInfo = {}
	self._StreamInfo.isLocal = isLocal
	self._StreamInfo.strFile = strFile
	self._StreamInfo.strURL = strURL
	return ret
end

function CAVPlayer:Play()
	assert(self._pPlayer)
	return self._pPlayer:Play()
end
function CAVPlayer:Pause()
	assert(self._pPlayer)
	self._pPlayer:Pause()
end
function CAVPlayer:Stop()
	assert(self._pPlayer)
	self._pPlayer:Stop()
end

function CAVPlayer:Volume(iVol)
	assert(self._pPlayer)
	self._pPlayer:Volume(iVol)
end
function CAVPlayer:VolumeIncrease()
	assert(self._pPlayer)
	self._pPlayer:VolumeIncrease()
end
function CAVPlayer:VolumeReduce()
	assert(self._pPlayer)
	self._pPlayer:VolumeReduce()
end

function CAVPlayer:SeekTo(iValue)
	assert(self._pPlayer)
	self._pPlayer:SeekTo(iValue)
end
function CAVPlayer:SeekForward(seconds)
	assert(self._pPlayer)
	self._pPlayer:SeekForward(seconds)
end
function CAVPlayer:SeekBackward(seconds)
	assert(self._pPlayer)
	self._pPlayer:SeekBackward(seconds)
end

function CAVPlayer:SetHWND(hWnd)
	assert(self._pPlayer)
	self._pPlayer:SetHWND(hWnd)
end

function CAVPlayer:GetStatus()
	assert(self._pPlayer)
	return self._pPlayer:GetStatus()
end
function CAVPlayer:IsOpen()
	assert(self._pPlayer)
	return self._pPlayer:IsOpen()
end
function CAVPlayer:IsPlaying()
	assert(self._pPlayer)
	return self._pPlayer:IsPlaying()
end
function CAVPlayer:GetPos()
	assert(self._pPlayer)
	return self._pPlayer:GetPos()
end
function CAVPlayer:GetVolume()
	assert(self._pPlayer)
	return self._pPlayer:GetVolume()
end
function CAVPlayer:GetTotalTime()
	assert(self._pPlayer)
	return self._pPlayer:GetTotalTime()
end
function CAVPlayer:GetTime()
	assert(self._pPlayer)
	return self._pPlayer:GetTime()
end

function CAVPlayer:OnPlaying(event)
	collectgarbage("collect")
	print(self:GetTotalTime())
	theApp:GetRuntimeState():GetEventMgr():Fire(_G.Event.BeginPlaying, event)
end
function CAVPlayer:OnPosChanged(event)
	theApp:GetRuntimeState():GetEventMgr():Fire(_G.Event.PlayingPosChanged, event)
end
function CAVPlayer:OnEndReached(event)
	collectgarbage("collect")
	theApp:GetRuntimeState():GetEventMgr():Fire(_G.Event.PlayingEndReached, event)
end

_G.OnPlaying = function(event) thePlayer:OnPlaying(event) end
_G.OnPosChanged = function(event) thePlayer:OnPosChanged(event) end
_G.OnEndReached = function(event) thePlayer:OnEndReached(event) end


return CAVPlayer

