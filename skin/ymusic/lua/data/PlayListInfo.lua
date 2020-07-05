local CPlayListInfo = FLua.Class("CPlayListInfo")

function CPlayListInfo:__constructor()
	self.m_nLocalID = 0
	self.m_nID = 0
	self.m_sName = ""
	self.m_sComment = ""
	self.m_eType = PlayListType.plt_user
	self.m_bShow = true
	self.m_nFlag = 0
	self.m_nTag = 0
end

function CPlayListInfo.new()
	local obj = CPlayListInfo()
	return obj
end

return CPlayListInfo