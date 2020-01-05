local CUserInfo = FLua.Class("CUserInfo")

function CUserInfo:__ctor()
	self.sUserID = ""
	self.sNickName = ""
	self.sArea = ""
	self.sAvaterUrl = ""
	self.sYear = ""
	self.nFollowCount = 0
	self.nFollowerCount = 0
	self.nFlag = 0
	self.sSex = 0
	self.sLevel = 0
	self.sRegTime = 0
	self.nCollect = 0
	self.sSignature = ""
end

return CUserInfo

