local CConfigBase = FLua.Class("CConfigBase")

function CConfigBase:__constructor()
	self.m_bModify = false
	self.m_spUserDataBase = nil
end
function CConfigBase:Load()
end
function CConfigBase:Save()
end
function CConfigBase:GetDB()
	if not self.m_spUserDataBase then
		self:UpdateDB()
	end
	return self.m_spUserDataBase
end
function CConfigBase:UpdateDB()
	self.m_spUserDataBase = theApp:GetRuntimeState():GetDbMgr():GetUserDB()
end
function CConfigBase:IsModify()
	return self.m_bModify
end
function CConfigBase:Modify(b)
	if b == nil then b = true end
	self.m_bModify = b
end
function CConfigBase:RegSet(hKey, cs_subkey, cs_value_name, cs_value)
	return Application.RegSet(hKey, cs_subkey, cs_value_name, cs_value)
end
--注册表设置，先检测是否存在，若存在则不写;
function CConfigBase:RegSetEx(hKey, cs_subkey, cs_value_name, cs_value)
	return Application.RegSetEx(hKey, cs_subkey, cs_value_name, cs_value)
end
function CConfigBase:RegDelete( cs_subkey, cs_value_name)
	return Application.RegDelete(cs_subkey, cs_value_name)
end
--注册表项查询
function CConfigBase:RegQuery(cs_subkey, cs_value_name)
	return Application.RegQuery(cs_subkey, cs_value_name)
end
function CConfigBase:GetAppName()
	return Application.GetAppName()
end


return CConfigBase