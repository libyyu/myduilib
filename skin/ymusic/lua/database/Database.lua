local CDatabase = FLua.Class("CDatabase")

local function fformat(fmt, ...)
	local v = ...
	if v then
		return string.format(fmt, ...)
	else
		return fmt
	end
end
---------------------------------------------------------
function CDatabase:__constructor()
	self.m_pDB = ldb.New(
	{
		OnCreateTables = function() return self:OnCreateTables() end,
		OnUpdateTables = function(oldVer, newVer) return self:OnUpdateTables(oldVer, newVer) end
	})
end
function CDatabase:Release()
	self:CloseDB()
	if self.m_pDB then
		self.m_pDB = nil
	end
end
function CDatabase:GetDBVersion()
	return self.m_pDB:GetDBVersion()
end
function CDatabase:BackupDB(sFileName)
	self:CloseDB()
	return self.m_pDB:BackupDB(sFileName)
end
function CDatabase:OpenDB(sFileName)
	self:CloseDB()
	self.m_pDB:OpenDB(sFileName)
	return self:IsOpen()
end
function CDatabase:CloseDB()
	if not self:IsOpen() then
		return
	end
	self.m_pDB:CloseDB()
end
function CDatabase:IsOpen()
	return self.m_pDB and self.m_pDB:IsOpen()
end

function CDatabase:ExecScalar(fmt, ... )
	local str = fformat(fmt, ...)
	return self.m_pDB:ExecScalar(str)
end
function CDatabase:ExecSql(fmt, ... )
	local str = fformat(fmt, ...)
	return self.m_pDB:ExecSql(str)
end
function CDatabase:QuerySql(fmt, ... )
	local str = fformat(fmt, ...)
	return self.m_pDB:QuerySql(str)
end
function CDatabase:GetDictString(sKey )
	return self.m_pDB:GetDictString(sKey)
end
function CDatabase:GetDictInt(sKey )
	return self.m_pDB:GetDictInt(sKey)
end
function CDatabase:GetDictBool(sKey )
	return self.m_pDB:GetDictBool(sKey)
end
function CDatabase:SetDictString(sKey, sValue)
	return self.m_pDB:SetDictString(sKey, sValue)
end
function CDatabase:SetDictInt(sKey, sValue)
	return self.m_pDB:SetDictInt(sKey, sValue)
end
function CDatabase:SetDictBool(sKey, sValue)
	return self.m_pDB:SetDictBool(sKey, sValue)
end
function CDatabase:InsertDictString(sKey, sValue)
	return self.m_pDB:InsertDictString(sKey, sValue)
end
function CDatabase:InsertDictInt(sKey, sValue)
	return self.m_pDB:InsertDictInt(sKey, sValue)
end
function CDatabase:InsertDictBool(sKey, sValue)
	return self.m_pDB:InsertDictBool(sKey, sValue)
end
function CDatabase:DeleteDict(sKey)
	return self.m_pDB:DeleteDict(sKey)
end

function CDatabase:IsExistTable(tablename)
	return self.m_pDB:IsExistTable(tablename)
end

function CDatabase:DeleteTable(tablename)
	return self.m_pDB:DeleteTable(tablename)
end

function CDatabase:BeginTransaction()
	return self.m_pDB:BeginTransaction()
end

function CDatabase:Commit()
	return self.m_pDB:Commit()
end

function CDatabase:RollBack()
	return self.m_pDB:Commit()
end

function CDatabase:GetErrorMsg()
	return self.m_pDB:GetErrorMsg()
end

function CDatabase:OnCreateTables()
	return true
end

function CDatabase:OnUpdateTables(oldVer, newVer)
	return true
end

return CDatabase

