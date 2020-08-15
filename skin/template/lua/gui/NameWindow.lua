local IBaseWindow = require "gui.IBaseWindow"

local NameWindow = FLua.Class(IBaseWindow,"NameWindow")
local l_instance

function NameWindow:__constructor()
	self.m_init_name = ""
	self.m_init_prompt = ""
	self.m_callback = nil
end

function NameWindow:WindowClass()
	return "NameWindowClass"
end
function NameWindow:WindowName()
	return "NameWindow"
end
function NameWindow:SkinFile()
	return "xml/gui/name_ui.xml"
end

function NameWindow:OnNotify(msg)
	local win = self.m_hWin
	if not msg or msg.isnil then return end
	if not msg.pSender then return end
	local msgType = msg.sType
	local sender = msg.pSender
	if not sender or sender.isnil then return end
	if msgType ~= "timer" then
		print(">>>>>>>>>>>>>>>>>>>>>msgType", msgType, sender)
	end
	self.m_bHandleNotify = false
	if msgType == "windowinit" then
		self:OnInitWindow()
	elseif msgType == "click" then
		self:OnClick(msg)
	end

	if not self.m_bHandleNotify then
		IBaseWindow.OnNotify(self, msg)
	end
end

function NameWindow:OnInitWindow()
	self.m_bHandleNotify = true
	local pEdit = self:FindControl("input_name")
	pEdit:SetPrompt(self.m_init_prompt)
	pEdit:SetText(self.m_init_name)
end

function NameWindow:OnClick(msg)
	local sender = msg.pSender
	if sender:IsName("submit") then
		self.m_bHandleNotify = true
		self:DoSubmit()
	end
end

function NameWindow:DoSubmit()
	if self.m_callback then
		if self.m_callback(self:FindControl("input_name"):GetText()) then
			self:Close()
			return
		end
	else
		self:Close()
	end
end

function NameWindow:Popup(name, prompt, callback)
	self.m_init_name = name
	self.m_init_prompt = prompt or ""
	self.m_callback = callback
	self:CreateWindow(theApp:GetMainWindow())
	assert(self:IsValid())
	self:CenterWindow()
	self:ShowModal()
end

function NameWindow.Instance()
	if not l_instance then
		l_instance = NameWindow()
	end
	return l_instance
end

return NameWindow