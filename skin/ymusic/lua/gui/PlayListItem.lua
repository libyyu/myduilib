local PlayListItem = FLua.Class("PlayListItem")
function PlayListItem.new(list_id, lsit_info)
	local obj = PlayListItem()
	obj.list_id = list_id
	obj.spListInfo = lsit_info
	return obj
end
function PlayListItem:__constructor()
	self.list_id = -1
	self.spListInfo = nil
	self.pItemObj = nil
	self._bReName = false
	self._spListName = nil 
	self._spRenameEdit = nil
	self._spPlayIco = nil
	self._status = -1
end
function PlayListItem:__destructor()
end
function PlayListItem:GetID()
	return self.list_id
end
function PlayListItem:GetListInfo()
	return self.spListInfo
end
function PlayListItem:SetListInfo(spListInfo)
	self.spListInfo = spListInfo
end
function PlayListItem:Update()
	if self.spListInfo then
		self._spListName:SetText(self.spListInfo.m_sName)
		self._spListName:SetToolTip(self.spListInfo.m_sName)
		self._spRenameEdit:SetText(self.spListInfo.m_sName)
	else
		self._spListName:SetText("Unknow")
		self._spListName:SetToolTip("Unknow")
		self._spRenameEdit:SetText("Unknow")
	end
end
function PlayListItem:Init(pListItem)
	self.pItemObj = pListItem
	self._spListName = pListItem:FindSubControl("list-name")
	self._spRenameEdit = pListItem:FindSubControl("rename")
	self._spPlayIco = pListItem:FindSubControl("list_ico")
	assert(self._spListName)
	assert(self._spRenameEdit)
	assert(self._spPlayIco)
	self._spListName:SetTagCtrl(pListItem)
	self._spRenameEdit:SetTagCtrl(pListItem)
	self._spPlayIco:SetTagCtrl(pListItem)
	pListItem:OnPaintAdd(function(pSelf)
		self:DoPaint()
	end)
	self._spListName:SetBindEventCtrl(self.pItemObj)
	self:Update()
end

function PlayListItem:DoPaint()
	local spListName = self._spListName
	if self.spListInfo then
		local sText = self.spListInfo.m_sName
		local spCurListID = theApp:GetRuntimeState():GetPlayListMgr():GetCurListID()
		if self.spListInfo and self.spListInfo.m_nLocalID == spCurListID and theApp:GetRuntimeState():GetPlayCenter():GetStatus() ~= _G.em_play_status.em_stop then
			local szBuf = string.format("<f 1><c #F27D30>%s</c></f>", sText)
			spListName:SetText(szBuf)
			self:ChangeIcon(theApp:GetRuntimeState():GetPlayCenter():GetStatus())
		else
			local szBuf = string.format("<f 0>%s</f>", sText)
			spListName:SetText(szBuf)
			self:ChangeIcon(_G.em_play_status.em_stop)
		end
	end
end

function PlayListItem:ChangeIcon(status)
	if self._status == status then
		return
	end
	self._status = status
	local sPlayImg = self._spPlayIco:GetBkImage()
	local sPauseImg = self._spPlayIco:GetUserData()
	if status == _G.em_play_status.em_play then
		self._spPlayIco:SetVisible(true)
		self._spPlayIco:SetBkImage(sPlayImg)
		self._spPlayIco:SetUserData(sPauseImg)
	elseif status == _G.em_play_status.em_pause then
		self._spPlayIco:SetVisible(true)
		self._spPlayIco:SetBkImage(sPauseImg)
		self._spPlayIco:SetUserData(sPlayImg)
	else
		self._spPlayIco:SetVisible(false)
	end
end


return PlayListItem