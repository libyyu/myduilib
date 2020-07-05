local IBaseWindow = require "frames.IBaseWindow"
local ProtoUtil = require "proto.ProtoUtil"

local _uTaskbarCreatedMsg
local _uTaskbarButtonCreateMsg
local _hOnlineIcon
local _hOfflineIcon

local MainWindow = FLua.Class(IBaseWindow,"MainWindow")
function MainWindow.new()
	local obj = MainWindow()
	return obj
end

function MainWindow:__constructor()
	self.m_pGlobalTimer = nil
	self.m_bHandleNotify = false
	self.m_TimerId = nil
	self.m_pList = nil
	self.m_pTreeView = nil
	self.m_message_list = {}


	_uTaskbarCreatedMsg 	 = Application.RegisterWindowMessage("TaskbarCreated")
	_uTaskbarButtonCreateMsg = Application.RegisterWindowMessage("TaskbarButtonCreated")
end

function MainWindow:WindowClass()
	return "MainWindowClass"
end
function MainWindow:WindowName()
	return "MainWindow"
end

-- function MainWindow:WindowInitPos()
-- 	return {
-- 		left = 0,
-- 		right = 600,
-- 		top = 0,
-- 		bottom = 600
-- 	}
-- end
function MainWindow:SkinFile()
	return "xml/gui/main_ui.xml"
end

function MainWindow:Release()
	self.m_pList = nil
	self.m_message_list = nil
end

function MainWindow:OnDestroy(wParam,lParam)
	if self.m_TimerId then
		self.m_hWin:KillTimer(self.m_TimerId)
		self.m_TimerId = nil
	end
	if _hOnlineIcon then
		Application.DestroyIcon(_hOnlineIcon)
		_hOnlineIcon = nil
	end
	if _hOfflineIcon then
		Application.DestroyIcon(_hOfflineIcon)
		_hOfflineIcon = nil
	end

	if self.m_pGlobalTimer then
		Timer.RemoveGlobalTimer(self.m_pGlobalTimer)
		self.m_pGlobalTimer = nil
	end
	IBaseWindow.OnDestroy(self,wParam,lParam)
end

function MainWindow:OnSysCommand(wParam,lParam)
	--warn("OnSysCommand", wParam, lParam, type(wParam),DuiLib.SC_CLOSE,DuiLib.IntToUInt64(DuiLib.SC_CLOSE) )
	if type(wParam) == "number" and wParam == DuiLib.SC_CLOSE then
		_G.CLOSING = true
		self:PostMessage(DuiLib.MsgArgs.WM_QUIT, 0, 0)
	elseif( wParam == helper.IntToUInt64(DuiLib.SC_CLOSE) ) then
		_G.CLOSING = true
		self:PostMessage(DuiLib.MsgArgs.WM_QUIT, 0, 0)
	end
end
function MainWindow:GetProcessWindowMessage()
	return {
		_uTaskbarCreatedMsg,
		_uTaskbarButtonCreateMsg,
		DuiLib.MsgArgs.WM_COMMAND,
		DuiLib.MsgArgs.WM_TIMER,
	}
end
function MainWindow:ProcessWindowMessage(uMsgId,wParam,lParam)
	print("ProcessWindowMessage", uMsgId, wParam, lParam)
	local MsgArgs = DuiLib.MsgArgs
	if uMsgId == MsgArgs.WM_USER + 2500 then
		self:OnAddListItem(wParam, lParam)
	elseif uMsgId == MsgArgs.WM_TIMER then
		self:OnWinTimer(wParam, lParam)
	--[[elseif uMsgId == MsgArgs.WM_QUIT then
		print("WM_QUIT")
	elseif uMsgId == MsgArgs.WM_KEYDOWN then
		print("uMsgId,WM_KEYDOWN", wParam)
		if wParam == 0x1B then
			self:Close()
		end
	elseif uMsgId == MsgArgs.WM_MOVING or uMsgId == MsgArgs.WM_SIZING then
		-- if not self._pViewInfo or not self._pViewInfo:IsValid() then
		-- 	return
		-- end
		-- if not self._pViewInfo:IsVisible() then
		-- 	return
		-- end
		-- local rtRect = self:GetWindowRect()
		-- local rt = self._pViewInfo:GetWindowRect()
		-- self._pViewInfo:MoveWindow(rtRect.right,rtRect.top,rt.right-rt.left,rt.bottom-rt.top,true)
		return true]]
	elseif uMsgId == _uTaskbarCreatedMsg then
		print("_uTaskbarCreatedMsg", _uTaskbarCreatedMsg)
	end

	return false
end
function MainWindow:OnCommand(wParam,lParam)
	print("OnCommand", wParam, lParam, helper.HIWORD(wParam))
	if THBN_CLICKED ~= helper.HIWORD(wParam) then
		return false
	end
	return false
end
function MainWindow:OnMenuCommand(pMsg)
end

function MainWindow:GetItemText(pListElement,iIndex,iSubItem)
	return self.m_message_list[iIndex+1]
end

function MainWindow:OnWinTimer(wParam,lParam)
	print("OnWinTimer", wParam, lParam)
	if wParam == helper.IntToUInt64(100) or wParam == 100 then
		local index = self.m_pList:GetCount()
		if self.m_message_list[index+1] then
			local CListTextElementUI = DuiLib.CListTextElementUI
			local pListElement = CListTextElementUI.New()
			pListElement:SetTag(index)
			--pListElement:SetAttribute("align", "center")
			pListElement:SetText2(0, self.m_message_list[index + 1])
			pListElement:SetText(self.m_message_list[index + 1])
			self.m_pList:Add(pListElement)
			self.m_pList:EndDown()
		else
			self.m_hWin:KillTimer(self.m_TimerId)
			self.m_TimerId = nil
		end
	end
end

function MainWindow:OnNotify(msg)
	local win = self.m_hWin
	if not msg or msg.isnil then return end
	if not msg.pSender then return end
	local msgType = msg.sType
	local sender = msg.pSender
	if not sender or sender.isnil then return end
	self.m_bHandleNotify = false
	if msgType == "click" then
		self:OnClick(msg)
	elseif msgType == "windowinit" then
		self:OnInitWindow()
	elseif msgType == "itemactivate" then
		self:OnItemActivate(msg)
	elseif msgType == "selectchanged" then
		self:OnSelectChanged(msg)
	elseif msgType == "itemselect" then
		self:OnItemSelect(msg)
	elseif msgType == "menu" then
		self:OnMenu(msg)
	elseif msgType == "valuechanged" then
		self:OnValueChanged(msg)
	elseif msgType == "killfocus" then
		self:OnKillFocus(msg)
	elseif msgType == "return" then
		self:OnReturn(msg)
	elseif msgType == "timer" then
		self:OnDuiTimer(msg)
	end
	if not self.m_bHandleNotify then
		IBaseWindow.OnNotify(self, msg)
	end
end

function MainWindow:OnInitWindow()
	Application.SetMainWindow(self.m_hWin)

	local PaintManagerUI = DuiLib.CPaintManagerUI
	local path = PaintManagerUI.GetResourcePath()
	local hIcon = Application.LoadIconFromFile(path.."/logo.ico", nil)
	if hIcon then
		self:SendMessage(DuiLib.MsgArgs.WM_SETICON, true, hIcon)
		self:SendMessage(DuiLib.MsgArgs.WM_SETICON, false, hIcon)
	end

	_hOnlineIcon        	 = Application.LoadIconFromFile(path.."/logo.ico", nil)
	_hOfflineIcon       	 = Application.LoadIconFromFile(path.."/logo.ico", nil)
	--增加托盘图标
	self:AddTracyIcon()

	-- 强制进行一次垃圾收集
	collectgarbage("collect")

	self.m_message_list = {}
	local message_list = config_common:GetAllTemplate()
	for i, m in ipairs(message_list) do
		if m:FindFieldDescriptor('index') and m:FindFieldDescriptor('version') then
			local l,t = m:GetSourceLocation(m.__pDescriptor)
			table.insert(self.m_message_list, l and #l >0 and l or m:GetName())
		end
	end

	local win = self.m_hWin
	self.m_pList = win:FindControl("config_list")
	self.m_pList:RemoveAll()
	win:SetListCallback(self.m_pList)
	--self.m_TimerId = win:SetTimer(100, 0.1)

	local TreeView = require "frames.treeview.TreeView"
	local pList = win:FindControl("treeview_list")
	local cls = FLua.ForwardClass("FolderItem")
	local cls2 = FLua.ForwardClass("TemplateItem")
	self.m_pTreeView = TreeView.new(pList)
	for i=1, 5 do
		local nodein = cls.new("xml/item/node_0.xml", ">"..i)
		local node = self.m_pTreeView:Add(nodein)
		if i== 1 then
			local nodein2 = cls.new("xml/item/node_0.xml", "sub>"..i)
			self.m_pTreeView:Add(nodein2, node)

			local nodein3 = cls.new("xml/item/node_0.xml", "sub>"..i)
			node = self.m_pTreeView:Add(nodein3, node)

			local nodein4 = cls.new("xml/item/node_0.xml", "sub>sub>"..i)
			node = self.m_pTreeView:Add(nodein4, node)
		end
		for j=1, 5 do
			local nodein2 = cls2.new("xml/item/node.xml", "sub>"..i)
			self.m_pTreeView:Add(nodein2, node)
		end
	end
	
	self.m_pGlobalTimer = Timer.AddGlobalTimer(100, function()
		MainThreadTask.tick()
	end)

end

function MainWindow:OnClick(msg)
	local win = self.m_hWin
	local sender = msg.pSender
	if sender:IsName("closebtn") then
		self.m_bHandleNotify = true
		win:MsgBox("确定退出吗？","警告",0x00000030,function(ret)
			if ret == 1 then
				self:Close()
			end
		end)
	end
end

function MainWindow:OnSelectChanged(msg)
	local win = self.m_hWin
	local sender = msg.pSender
end

function MainWindow:OnItemActivate(msg)
	local win = self.m_hWin
	local sender = msg.pSender
	--print(">>>>>>>>>>>>sender", sender:GetName())
	if sender:IsClass("TemplListItem") then
		local name = sender:GetName()
	end
end

function MainWindow:OnItemSelect(msg)
	local win = self.m_hWin
	local sender = msg.pSender
end

function MainWindow:OnMenu(msg)
	local win = self.m_hWin
	local sender = msg.pSender
end

function MainWindow:OnValueChanged(msg)
end

function MainWindow:OnKillFocus(msg)
end

function MainWindow:OnReturn(msg)
end

function MainWindow:OnDuiTimer(msg)
	local win = self.m_hWin
	local sender = msg.pSender

end

function MainWindow:AddTracyIcon()
	DuiLib.CTrayIconUI.CreateTrayIconFromIcon(self:GetHWND(), _G.IDI_APP, _hOnlineIcon, "Template編輯器!", 0, self.m_hWin:PaintMgr())
	DuiLib.CTrayIconUI.ShowInfotip(" Template編輯器!", "欢迎使用")
end

function MainWindow:CreateControl(pstrClass)
	print("CreateControl -->", pstrClass)
	if pstrClass == "XTreeView" then
		local pList = DuiLib.CListUI.New()
		return pList
	end
	local pControl = MyControl.CreateControl(pstrClass)
	if pControl then
		return pControl
	end
	return nil
end

return MainWindow