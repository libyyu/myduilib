local CUserContext = FLua.Class("CUserContext")

function CUserContext:__constructor()
	self._runtimeState = nil
	self._user = nil
	self._userDB = nil
	self._tag = 0
end

function CUserContext:Create(runtime, user, userdb, sFlag)
	self._runtimeState = runtime
	self._user = user
	self._userDB = userdb
	assert(self:IsValid())
end

function CUserContext:IsValid()
	return self._runtimeState and self._userDB and true or false
end
function CUserContext:GetRuntimeState()
	return self._runtimeState
end
function CUserContext:GetUser()
	return self._user
end
function CUserContext:GetUserId()
	local theUserInfo = self:GetUser()
	return theUserInfo and theUserInfo.sUserID or ""
end
function CUserContext:IsUserLogined()
	return not not self._user
end
function CUserContext:GetUserDB()
	return self._userDB
end
function CUserContext:IsMainThread()
	return self:GetRuntimeState():IsMainThread()
end
function CUserContext:IsSameContext(ctx)
	assert(ctx)
	if not ctx then
		return false
	end
	return 
		(ctx:GetRuntimeState() == self:GetRuntimeState()) and
		(ctx:GetUser() == self:GetUser()) and
		(ctx:GetUserDB() == self:GetUserDB())
end
function CUserContext:IsCurrentContext()
	return self:IsSameContext(self:GetRuntimeState():GetCurrentContext(nil))
end

function CUserContext:SetTag(tag)
	self._tag = tag
end
function CUserContext:GetTag()
	return self._tag
end

return CUserContext