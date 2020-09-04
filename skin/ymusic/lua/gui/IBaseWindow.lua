local IBaseWindow = FLua.Abstract("IBaseWindow")

function IBaseWindow:__constructor()
	self.m_parent = nil
	self.m_hWin = nil
	self.m_msgTable = nil
	self.m_bHandleNotify = false
end

function IBaseWindow:WindowClass()
	return "IBaseWindow"
end
function IBaseWindow:WindowName()
	return "IBaseWindow"
end
function IBaseWindow:WindowStyle()
	return lib64.bor(DuiLib.UI_WNDSTYLE_FRAME, DuiLib.WS_CLIPCHILDREN)
end
function IBaseWindow:WindowStyleEx()
	return lib64.bor(DuiLib.WS_EX_WINDOWEDGE,DuiLib.WS_EX_OVERLAPPEDWINDOW)
end
function IBaseWindow:WindowInitPos()
	return _G.DEFAULT_POS
end
function IBaseWindow:SkinFile()
	return ""
end

function IBaseWindow:GetParent()
	return self.m_parent
end

function IBaseWindow:GetHWND()
	if self:IsValid() then
		return self.m_hWin:GetHWND()
	end
	return nil
end

function IBaseWindow:IsValid()
	if not self.m_hWin then return false end
	return self.m_hWin and not self.m_hWin.isnil and self.m_hWin:IsValid() ~= 0  and true or false
end
function IBaseWindow:IsVisible()
	assert(self:IsValid())
	return self.m_hWin:IsWindowVisible() ~= 0
end
function IBaseWindow:GetWindowRect()
	assert(self:IsValid())
	return self.m_hWin:GetWindowRect()
end
function IBaseWindow:GetClientRect()
	assert(self:IsValid())
	return self.m_hWin:GetClientRect()
end
function IBaseWindow:MoveWindowEx(rect, repaint)
	assert(self:IsValid())
	self.m_hWin:MoveWindow(rect, repaint)
end
function IBaseWindow:MoveWindow(x,y,w,h, repaint)
	assert(self:IsValid())
	self.m_hWin:MoveWindow(x,y,w,h, repaint)
end
function IBaseWindow:InvokeMethod(name, ...)
	local func = self:tryget(name)
	if func and type(func) == "function" then
		return func(self, ...)
	end
end

function IBaseWindow:OnCreate (wParam,lParam)
	local win = self.m_hWin
	win:ModifyStyle(DuiLib.WS_CAPTION, lib64.bor(DuiLib.WS_CLIPSIBLINGS,DuiLib.WS_CLIPCHILDREN), 0)

	if not win:RegisterSkin(self:SkinFile()) then
		DuiLib.MsgBox(nil,"Error")
		return false
	end

	--print("AddNotifier")
	-- local pm = win:PaintMgr()
	-- pm:AddPreMessageFilter(win:cast("IMessageFilterUI"))
	-- pm:AddMessageFilter(win:cast("IMessageFilterUI"))
	-- pm:AddNotifier(win:cast("INotifyUI"))
	--win:AddPreMessageFilter()
	--win:AddMessageFilter()
	--win:AddNotifier()
	return true
end

function IBaseWindow:OnFinalMessage()
	print("OnFinalMessage", self:WindowName())
end

function IBaseWindow:OnDestroy (wParam,lParam)
	print("================","OnDestroy", self:WindowName())
	self.m_hWin = nil
	_G.FRAME_MAPS[self] = nil
end
function IBaseWindow:SendMessage(uMsgID, wParam, lParam)
	if self:IsValid() then
		self.m_hWin:SendMessage(uMsgID, wParam, lParam)
	else
		warn("Can not SendMessage because panel is destroyed.")
	end
end
function IBaseWindow:PostMessage(uMsgID, wParam, lParam)
	if self:IsValid() then
		self.m_hWin:PostMessage(uMsgID, wParam, lParam)
	else
		warn("Can not PostMessage because panel is destroyed.")
	end
end
function IBaseWindow:Close()
	self:SendMessage(DuiLib.MsgArgs.WM_SYSCOMMAND, DuiLib.SC_CLOSE, 0)
end
function IBaseWindow:Release()
end

function IBaseWindow:TouchMsgTable()
	local msgt =
	{
		OnCreate = self:tryget("OnCreate"),
		OnDestroy = self:tryget("OnDestroy"),
		OnSysCommand = self:tryget("OnSysCommand"),
		OnMenuCommand = self:tryget("OnMenuCommand"),
		OnFinalMessage = self:tryget("OnFinalMessage"),
		MessageHandler = self:tryget("MessageHandler"),
		ProcessWindowMessage = self:tryget("ProcessWindowMessage"),
		GetProcessWindowMessage = self:tryget("GetProcessWindowMessage"),
		OnNotify = self:tryget("OnNotify"),
		OnUIEvent = self:tryget("OnUIEvent"),
		OnUIDestroy = self:tryget("OnUIDestroy"),
		OnUISize = self:tryget("OnUISize"),
		OnUINotify = self:tryget("OnUINotify"),
		OnUIPaint = self:tryget("OnUIPaint"),
		GetSkinPath = self:tryget("GetSkinPath"),
		GetItemText = self:tryget("GetItemText"),
		CreateControl = self:tryget("CreateControl"),
	}
	self.m_msgTable = msgt
	self:MsgTableExtend()
	self.m_hWin:BindLuaScript(self.m_msgTable, self)
end
function IBaseWindow:MsgTableExtend()
end
function IBaseWindow:CreateWindow(parent)
	print("CreateWindow ", self:WindowName(), parent)
	local win = DuiLib.CLuaWindow.New(self:WindowClass())
	self.m_hWin = win
	self.m_parent = parent
	self:TouchMsgTable()

	local hParentWnd = self.m_parent and self.m_parent:GetHWND() or nil
	local pos = self:WindowInitPos()
	win:Create(hParentWnd, self:WindowName(),self:WindowStyle(),self:WindowStyleEx(), pos)

	assert(self:IsValid())
	if not self:IsValid() then
		win:MsgBox("Create Error","Error")
		return false
	end
	_G.FRAME_MAPS[self] = true
	return true
end
function IBaseWindow:ShowWindow(bShow, bTakeFocus)
	assert(self:IsValid())
	self.m_hWin:ShowWindow(bShow, bTakeFocus)
end
function IBaseWindow:CenterWindow()
	assert(self:IsValid())
	self.m_hWin:CenterWindow()
end
function IBaseWindow:ShowModal()
	assert(self:IsValid())
	self.m_hWin:ShowModal()
end

function IBaseWindow:FindControl(name)
	assert(self:IsValid())
	return self.m_hWin:FindControl(name)
end

function IBaseWindow:GetPaintMgr()
	assert(self:IsValid())
	return self.m_hWin:PaintMgr()
end

function IBaseWindow:OnNotify(msg)
	local win = self.m_hWin
	if not msg or msg.isnil then return end
	if not msg.pSender then return end
	local msgType = msg.sType
	local sender = msg.pSender
	if not sender or sender.isnil then return end

	if msgType == "click" then
		if sender:IsName("closebtn") then
			self:SendMessage(DuiLib.MsgArgs.WM_SYSCOMMAND, DuiLib.SC_CLOSE, 0)
		elseif sender:IsName("minbtn") then
			self:SendMessage(DuiLib.MsgArgs.WM_SYSCOMMAND, DuiLib.SC_MINIMIZE, 0)
		elseif sender:IsName("restorebtn") then
			self:SendMessage(DuiLib.MsgArgs.WM_SYSCOMMAND, DuiLib.SC_RESTORE, 0)
		elseif sender:IsName("maxbtn") then
			self:SendMessage(DuiLib.MsgArgs.WM_SYSCOMMAND, DuiLib.SC_MAXIMIZE, 0)
		end
	elseif msgType == "maxmin" then
		local is_max = make_i64(1)
		local is_restore = make_i64(0)
		local pMax = self:FindControl("maxbtn")
		local pRestore = self:FindControl("restorebtn")
		local wParam = msg.wParam

		if is_max:equal(wParam) then
			if pMax and pRestore then
				pMax:SetVisible(false)
				pRestore:SetVisible(true)
			end
		else
			if pMax and pRestore then
				pMax:SetVisible(true)
				pRestore:SetVisible(false)
			end
		end
	end
end

function IBaseWindow:CreateControl(pstrClass)
	local pControl = MyControl.CreateControl(pstrClass)
	if pControl then
		return pControl
	end
	return nil
end

return IBaseWindow