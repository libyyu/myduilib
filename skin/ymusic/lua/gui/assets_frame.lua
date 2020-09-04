local IBaseWindow = require "gui.IBaseWindow"
--local ProtoUtil = require "utility.ProtoUtil"

local AssetsFrame = FLua.Class(IBaseWindow,"AssetsFrame")

function AssetsFrame.new()
	print("new AssetsFrame")
	local obj = AssetsFrame()
	return obj
end

function AssetsFrame:__constructor()
	self.mTimerId = nil
	self._pList = nil
	self._message_name = ""
	self._pListHeader = nil
end

function AssetsFrame:ShowMessage(name)
	self._message_name = name
	assert(self:IsValid())
end

function AssetsFrame:WindowClass()
	return "AssetsFrame"
end
function AssetsFrame:WindowName()
	return "AssetsFrame"
end

function AssetsFrame:SkinFile()
	return "xml/Frame/assets_view.xml"
end

function AssetsFrame:Release()
	self._pList = nil
	self._message_list = nil
	self.m_hWin:KillTimer(self.mTimerId)
	self.mTimerId = 0
end

function AssetsFrame:OnDestroy(wParam,lParam)
	IBaseWindow.OnDestroy(self,wParam,lParam)
end

function AssetsFrame:OnSysCommand(wParam,lParam)
	if( wParam == DuiLib.SC_CLOSE ) then
		print("=================OnSysCommand", wParam)
	end
end
function AssetsFrame:ProcessWindowMessage(uMsgId,wParam,lParam)
	local MsgArgs = DuiLib.MsgArgs
	if uMsgId == MsgArgs.WM_USER + 2500 then
		self:OnAddListItem(wParam,lParam)
	elseif uMsgId == MsgArgs.WM_TIMER then
		self:OnTimer(wParam,lParam);
	elseif uMsgId == MsgArgs.WM_QUIT then
		print("WM_QUIT")
	elseif uMsgId == MsgArgs.WM_KEYDOWN then
		print("uMsgId,WM_KEYDOWN", wParam)
		if wParam == 0x1B then
			self:Close()
		end
	end
end

function AssetsFrame:GetItemText(pControl,iIndex,iSubItem)
	local szBuf = ProtoUtil.GetMessageComment(self._message_list[iIndex+1])
    pControl:SetUserData(szBuf);
end

function AssetsFrame:OnAddListItem(wParam,lParam)
	local win = self.m_hWin
	local pListElement = win:ControlFromPtr(lParam)
	self._pList:Add(pListElement)
	self._pList:EndDown()
end
local index = 0
function AssetsFrame:OnTimer(wParam,lParam)
	local win = self.m_hWin
	if wParam == 100 then
		if self._message_list[index + 1] then
			local ListTextElementUI = DuiLib.ListTextElementUI
			local pControl = ListTextElementUI.New()
			pControl:SetTag(index)
			win:ListenUIDestroy(pControl)
			win:SendMessage(DuiLib.MsgArgs.WM_USER + 2500,0,pControl)
			index = index + 1
		else
			win:KillTimer(self.mTimerId)
			self.mTimerId = 0
			index = 0
		end
	end
end

function AssetsFrame:OnNotify(msg)
	local win = self.m_hWin
	local msgType = msg:getType()
	local sender = msg:getSender()
	if sender:isnil() then return end
	--print("OnNotify",win,sender:GetName(),sender:GetClass(),msg:getType())
	if msg:isType("click") then
		if msg:isControl("closebtn") then
			self:Close()
		elseif msg:isControl("maxbtn") then
			win:SendMessage(DuiLib.MsgArgs.WM_SYSCOMMAND, DuiLib.SC_MAXIMIZE, 0)
			return;
		elseif msg:isControl("minbtn") then
			win:SendMessage(DuiLib.MsgArgs.WM_SYSCOMMAND, DuiLib.SC_MINIMIZE, 0)
			return;
		elseif msg:isControl("btn") then
			self:OnSearch()
		end
	elseif msg:isType("windowinit") then
		self:OnInitWindow()
	elseif msg:isType("itemactivate") then
		local iIndex = sender:GetTag()
		local sMessage = "Click: " .. self._message_list[iIndex+1]
		DuiLib.MsgBox(win,sMessage,"提示(by ldf)",0)
	end
end

function AssetsFrame:OnInitWindow()
	--self._message_list = ProtoUtil.GetAllMessage()
	local win = self.m_hWin
	self._pListHeader = win:FindControl("assets_header")
	self._pList = win:FindControl("assets_info")
	self._pList:RemoveAll()
	win:SetListCallback(self._pList)
	--self.mTimerId = win:SetTimer(100, 0.1)
	local fields = ProtoUtil.GetFiledList(self._message_name)
	warn(_G.Utils.Value2String(fields))
end

return AssetsFrame