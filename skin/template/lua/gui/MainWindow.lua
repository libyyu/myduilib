local IBaseWindow = require "gui.IBaseWindow"
local ProtoUtil = require "proto.ProtoUtil"
local MainContent = require "gui.MainContent"

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
	self.m_TimerId = nil
	self.m_pTreeView = nil
	self.m_config_type_list = {}
	self.m_pContent = MainContent.new(self)

	_uTaskbarCreatedMsg 	 = Application.RegisterWindowMessage("TaskbarCreated")
	_uTaskbarButtonCreateMsg = Application.RegisterWindowMessage("TaskbarButtonCreated")
end

function MainWindow:__destructor()

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
	self.m_pTreeView = nil
	self.m_config_type_list = nil
	IBaseWindow.Release(self)
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

	theApp:GetRuntimeState():GetEventMgr():DelEvent(self, _G.Event.AddTreeNode)
	self.m_pContent:OnDestroy()
	self.m_pContent = nil

	IBaseWindow.OnDestroy(self,wParam,lParam)
end

function MainWindow:OnSysCommand(wParam,lParam)
	warn("OnSysCommand", wParam, lParam, type(wParam),type(DuiLib.SC_CLOSE))
	local sc_close = make_i64(DuiLib.SC_CLOSE)
	print("sc_close", sc_close)
	if sc_close:equal(wParam) then
		_G.CLOSING = true
		self:PostMessage(DuiLib.MsgArgs.WM_QUIT, 0, 0)
	end
end
function MainWindow:GetProcessWindowMessage()
	for i,v in ipairs({
		_uTaskbarCreatedMsg,
		_uTaskbarButtonCreateMsg,
		DuiLib.MsgArgs.WM_COMMAND,
		DuiLib.MsgArgs.WM_TIMER,
	}) do
		print("----->", i, v)
	end
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

function MainWindow:GetItemText(pListElement,iIndex,iSubItem)
	return self.m_message_list[iIndex+1]
end

function MainWindow:OnWinTimer(wParam,lParam)
	print("OnWinTimer", wParam, lParam)
	if wParam == lib64.IntToUInt64(100) or wParam == 100 then
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
	if msgType ~= "timer" then
		print(">>>>>>>>>>>>>>>>>>>>>msgType", msgType, sender)
	end
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
	theApp:GetRuntimeState():GetEventMgr():AddEvent(self, _G.Event.AddTreeNode)
	self.m_pContent:OnCreate()

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

	--local win = self.m_hWin
	--self.m_pList = win:FindControl("config_list")
	--self.m_pList:RemoveAll()
	--win:SetListCallback(self.m_pList)
	--self.m_TimerId = win:SetTimer(100, 0.1)

	local TreeView = require "gui.treeview.TreeView"
	local pList = self:FindControl("treeview_list")
	self.m_pTreeView = TreeView.new(pList)
	
	self:OnInitTemplateTree()
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

	local cls = FLua.ForwardClass("FolderItem")
	local cls2 = FLua.ForwardClass("TemplateItem")

	if sender:IsName("treeview_list") then
		local clientpos = win:ScreenToClient(Application.GetCursorPos())
		local pControl = self.m_hWin:PaintMgr():FindSubControlByPoint(sender, clientpos)
		if pControl and pControl:GetTagCtrl() then
			local pOwnerCtrl = pControl:GetTagCtrl()
			if pOwnerCtrl:IsClass("ListContainerElement") then
				local node = pOwnerCtrl:GetUserData2("node")
				if node and node:is(cls) then
					local pMenu = DuiLib.CMenuWnd.New(self:GetHWND())
					pMenu:SetTag(_G.emMenuType.EMT_TEMPLATE_TREE_ROOT)
					pMenu:Init(nil, "xml/menu/menu-template-root.xml", "", Application.GetCursorPos())
					pMenu:SetUserData2("node", node)
				else
					local pMenu = DuiLib.CMenuWnd.New(self:GetHWND())
					pMenu:SetTag(_G.emMenuType.EMT_TEMPLATE_TREE_ITEM)
					pMenu:Init(nil, "xml/menu/menu-template-item.xml", "", Application.GetCursorPos())
					pMenu:SetUserData2("node", node)
				end
			end
		end
	end
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
	
	return IBaseWindow.CreateControl(self, pstrClass)
end

function MainWindow:OnMenuCommand(pMsg)
	print("OnMenuCommand", pMsg.sItemName, pMsg.nMenuTag)
	if pMsg.nMenuTag == _G.emMenuType.EMT_TEMPLATE_TREE_ROOT then
		self:OnMenuCommandTemplateTreeRoot(pMsg)
	end
end

function MainWindow:OnMenuCommandTemplateTreeRoot(msg)
	local pMenu = msg.pMenuWnd
	if msg.sItemName == "new_folder" then
		self:OnHandleNewTemplateFolder(pMenu:GetUserData2("node"))
	elseif msg.sItemName == "new_template" then
		self:OnHandleNewTemplateFile(pMenu:GetUserData2("node"))
	end
end

function MainWindow:OnHandleNewTemplateFolder(node)
	if not node then
		return
	end
	local cls = FLua.ForwardClass("FolderItem")
	assert(node and node:is(cls))
	local NameWindow = require "gui.NameWindow"

	local data_path = node:GetPath()
	local tmpl_cls_name = node:GetName()
	local new_folder_name = "新建目录"
	local base_folder_name = new_folder_name
	local pos = 0
	while true do
		local f = helper.UTF8ToMB(data_path .. tmpl_cls_name .. "/" .. new_folder_name)
		local attr = lfs.attributes (f)
		if attr and attr.mode == "directory" then
			pos = pos + 1
			new_folder_name = base_folder_name .. pos
		else
			break
		end
	end

	NameWindow.Instance():Popup(new_folder_name, "请输入目录名", function(input)
		if #input == 0 then
			warn("please input folder name")
			return false 
		end
		if Application.IsDirectoryExist(data_path .. tmpl_cls_name .. "/" .. input) then
			warn("folder exist")
			return false
		end
		print(">>>>>>>>>>>>", data_path .. tmpl_cls_name .. "/" .. input)
		if Application.CreateDirectory(data_path .. tmpl_cls_name) and 
			Application.CreateDirectory(data_path .. tmpl_cls_name .. "/" .. input) then

			theApp:GetRuntimeState():GetEventMgr():Fire(_G.Event.AddTreeNode, node, true, input)

			return true
		else
			return false
		end
	end)
end
function MainWindow:OnHandleNewTemplateFile(node)
	if not node then
		return
	end

	local cls = FLua.ForwardClass("FolderItem")
	assert(node and node:is(cls))
	local NameWindow = require "gui.NameWindow"

	local data_path = node:GetPath()
	local tmpl_cls_name = node:GetName()
	local new_file_name = "新建文件"
	local base_file_name = new_file_name
	local pos = 0
	while true do
		local f = helper.UTF8ToMB(data_path .. tmpl_cls_name .. "/" .. new_file_name .. ".tmpl")
		local attr = lfs.attributes (f)
		if attr and attr.mode == "file" then
			pos = pos + 1
			new_file_name = base_file_name .. tostring(pos)
		else
			break
		end
	end
	NameWindow.Instance():Popup(new_file_name, "请输入文件名", function(input)
		if #input == 0 then
			warn("please input file name")
			return false 
		end
		if Application.IsFileExist(data_path .. tmpl_cls_name .. "/" .. input.. ".tmpl") then
			warn("file exist")
			return false
		end
		local filepath = data_path .. tmpl_cls_name .. "/" .. input.. ".tmpl"
		print(">>>>>>>>>>>>", filepath)
		if Application.CreateDirectory(data_path .. tmpl_cls_name) then 
			local config_type_info = node:GetUserData()
			local class_name = config_type_info.class_name
			local msg_class = config_common:GetTmplClass(class_name)

			local Tmpl = require "data.Tmpl"
			local tmpl = Tmpl.new(filepath, msg_class) 

			if not tmpl:Save() then
				return false
			end
			
			theApp:GetRuntimeState():GetEventMgr():Fire(_G.Event.AddTreeNode, node, false, input, tmpl)

			return true
		else
			return false
		end
	end)
end

function MainWindow:AddTreeNode(node_parent, folder, input, tmpl)
	if folder then
		local cls = FLua.ForwardClass("FolderItem")
		local nodein = cls.new("xml/item/node_0.xml", input)
		nodein:SetUserData(node_parent:GetUserData())
		nodein:SetPath(node_parent:GetPath() .. node_parent:GetName().."/")
		self.m_pTreeView:Add(nodein, node_parent)
	else
		local cls = FLua.ForwardClass("TemplateItem")
		local nodein = cls.new("xml/item/node.xml", input)
		nodein:SetUserData(node_parent:GetUserData())
		nodein:SetTmpl(tmpl)
		self.m_pTreeView:Add(nodein, node_parent)
	end
end

function MainWindow:OnInitTemplateTree()
	self.m_config_type_list = {}
	local all_templates = config_common:GetAllTemplate()
	local id = config_common:GetMessageExtensionValue("ConfigType")
	for _, v in ipairs(all_templates) do
		local uf = v:unknow_field_by_value(id)
		if uf then 
			local msg_id = uf:varint()
			local msg_id_64 = make_i64(msg_id)
			msg_id = msg_id_64:todouble()
			local l, t = v:GetSourceLocation()
	        if l and #l >0 then
	        	table.insert(self.m_config_type_list, {config_type=msg_id, display_name=l, class_name=v:GetName()})
	        else
	        	l, t = config_common:GetEnumSourceLocation("CONFIG_TYPE", msg_id)
	        	if t and #t >0 then
	        		table.insert(self.m_config_type_list, {config_type=msg_id, display_name=t, class_name=v:GetName()})
	        	else
					table.insert(self.m_config_type_list, {config_type=msg_id, display_name=v:GetName(), class_name=v:GetName()})
	        	end
	        end
	    end
	end

	local cls = FLua.ForwardClass("FolderItem")
	local cls2 = FLua.ForwardClass("TemplateItem")
	local data_path = theApp:GetRuntimeState():GetFilePathMgr():GetTmplRootPath()
	for i=1, #self.m_config_type_list do
		local nodein = cls.new("xml/item/node_0.xml", self.m_config_type_list[i].display_name)
		nodein:SetUserData(self.m_config_type_list[i])
		nodein:SetPath(data_path)
		self.m_pTreeView:Add(nodein)
	end 
end

return MainWindow