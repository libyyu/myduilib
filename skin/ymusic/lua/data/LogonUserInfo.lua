local UserInfo = require "data.UserInfo"
local CLogonUserInfo = FLua.Class(UserInfo, "CLogonUserInfo")

function CLogonUserInfo:__ctor()
	self.sLogonEmail = ""
	self.sPasswordMD5 = ""
	self.bKeepLogged = true
end

return CLogonUserInfo

