local SongListItem = FLua.Class("SongListItem")
function SongListItem.new(songInfo)
	local obj = SongListItem()
	obj.songInfo = songInfo
	return obj
end
function SongListItem:__ctor()
	self._status = -1
	self.songInfo = nil
	self.pItemObj = nil
	self.pOwnerList = nil
	self._pNorLabelIndex = nil
	self._pNorLabelName = nil
	self._pNorLabelAritst = nil
	self._pNormalLayout = nil
	self._pNorButtonMenu = nil
	self._pItemIco = nil
	self._pPushLabelIndex = nil
	self._pPushLabelName = nil
	self._pPushLabelAritst = nil
	self._pPushLabelAlbum = nil
	self._pPushDownLoad = nil
	self._pPushCollect = nil
	self._pPushShare = nil
	self._pPushRemove = nil
	self._pPushLayout = nil
end
function SongListItem:GetLocalID()
	return self.songInfo.m_nLocalID
end
function SongListItem:GetSongInfo()
	return self.songInfo
end
local nSongItemHeight = 25
local nSongItemExpHeight = 40

function SongListItem:Init(pItemObj, pOwnerList)
	self.pItemObj = pItemObj
	self.pOwnerList = pOwnerList

	self._pNorLabelIndex = pItemObj:FindSubControl("norindexlab")
	self._pNorLabelName = pItemObj:FindSubControl("norname")
	self._pNorLabelAritst = pItemObj:FindSubControl("noraritst")
	self._pNormalLayout = pItemObj:FindSubControl("songitemnormallayout")
	self._pNorButtonMenu = pItemObj:FindSubControl("normenu")
	self._pItemIco = pItemObj:FindSubControl("song_ico")
	self._pPushLabelIndex = pItemObj:FindSubControl("pusindexlab")
	self._pPushLabelName = pItemObj:FindSubControl("pusname")
	self._pPushLabelAritst = pItemObj:FindSubControl("pusaritst")
	self._pPushLabelAlbum = pItemObj:FindSubControl("pusalbum")
	self._pPushDownLoad = pItemObj:FindSubControl("pusdownbtn")
	self._pPushCollect = pItemObj:FindSubControl("pusheartbtn")
	self._pPushShare = pItemObj:FindSubControl("pussharebtn")
	self._pPushRemove = pItemObj:FindSubControl("pusremovebtn")
	self._pPushLayout = pItemObj:FindSubControl("songitemexplayout")

	-- self._spListName:SetTagCtrl(pListItem)
	-- self._spRenameEdit:SetTagCtrl(pListItem)
	-- self._spPlayIco:SetTagCtrl(pListItem)
	pItemObj:OnPaintAdd(function(pSelf)
		self:DoPaint()
	end)
	-- pItemObj:OnEventAdd(function(pEvent)
	-- 	if pEvent.Type==DuiLib.EVENTTYPE_UI.UIEVENT_MOUSELEAVE then
	-- 		self._pNorButtonMenu:SetVisible(false)
	-- 	else
	-- 		self._pNorButtonMenu:SetVisible(true)
	-- 	end
	-- end)

	self._pNorLabelIndex:SetBindEventCtrl(self.pItemObj)
	self._pNorLabelName:SetBindEventCtrl(self.pItemObj)
	self._pNorLabelAritst:SetBindEventCtrl(self.pItemObj)
	self._pNormalLayout:SetBindEventCtrl(self.pItemObj)
	--self._pNorButtonMenu:SetBindEventCtrl(self.pItemObj)

	self:Update()
end

function SongListItem:Update()
	if self.songInfo then
		self._pNorLabelName:SetText(self.songInfo.m_sSongName)
		self._pNorLabelAritst:SetText(self.songInfo.m_sArtistName)
		self._pNorLabelName:SetToolTip(self.songInfo.m_sSongName)
		self._pNorLabelAritst:SetToolTip(self.songInfo.m_sArtistName)
		
		self._pPushLabelName:SetText(self.songInfo.m_sSongName)
		self._pPushLabelAritst:SetText(self.songInfo.m_sArtistName)
		self._pPushLabelAlbum:SetText(self.songInfo.m_sAlbumName)
		self._pPushLabelName:SetToolTip(self.songInfo.m_sSongName)
		self._pPushLabelAritst:SetToolTip(self.songInfo.m_sArtistName)
		self._pPushLabelAlbum:SetToolTip(self.songInfo.m_sAlbumName)
	else
		
	end
end

function SongListItem:DoPaint()
	local spCurListID = theApp:GetRuntimeState():GetPlayListMgr():GetCurListID()
	local spCurSong = theApp:GetRuntimeState():GetPlayListMgr():GetCurSong()
	local u_list_id = self.pOwnerList:GetUserData2("__play_list_id")
	if spCurListID ~= -1 and spCurSong and spCurSong == self.songInfo and spCurListID == u_list_id then
		if self.pItemObj:GetFixedHeight() ~= nSongItemExpHeight then
			self.pItemObj:SetFixedHeight(nSongItemExpHeight)
			self._pNormalLayout:SetVisible(false)
			self._pPushLayout:SetVisible(true)

			self._pPushDownLoad:SetVisible(spCurSong.m_bFromNet)
			self._pPushShare:SetVisible(spCurSong.m_bFromNet)
		end
		local dwSelBkColor = self.pOwnerList:GetSelectedItemBkColor()
		self.pItemObj:SetBkColor(dwSelBkColor)

		self:ChangeIcon(theApp:GetRuntimeState():GetPlayCenter():GetStatus())
	else
		if self.pItemObj:IsSelected() and self.pOwnerList:GetSelCount() == 1 then
			self._pNormalLayout:SetVisible(true)
			self._pPushLayout:SetVisible(false)
		else
			self._pNormalLayout:SetVisible(true)
			self._pPushLayout:SetVisible(false)
		end
		if self.pItemObj:GetFixedHeight() ~= nSongItemHeight then
			self.pItemObj:SetFixedHeight(nSongItemHeight)
		end
		self:ChangeIcon(_G.em_play_status.em_stop)
	end
	local nIndex = self.pOwnerList:GetItemIndex(self.pItemObj)
	self._pNorLabelIndex:SetText(tostring(nIndex+1))
	self._pPushLabelIndex:SetText(tostring(nIndex+1))
end

function SongListItem:ChangeIcon(status)
	if self._status == status then
		return
	end
	self._status = status
	local sPlayImg = self._pItemIco:GetBkImage()
	local sPauseImg = self._pItemIco:GetUserData()
	if status == _G.em_play_status.em_play then
		self._pItemIco:SetVisible(true)
		self._pItemIco:SetBkImage(sPlayImg)
		self._pItemIco:SetUserData(sPauseImg)
		self._pPushLabelIndex:SetVisible(false)
	elseif status == _G.em_play_status.em_pause then
		self._pItemIco:SetVisible(true)
		self._pItemIco:SetBkImage(sPauseImg)
		self._pItemIco:SetUserData(sPlayImg)
		self._pPushLabelIndex:SetVisible(false)
	else
		self._pItemIco:SetVisible(false)
		self._pPushLabelIndex:SetVisible(true)
	end
end

return SongListItem