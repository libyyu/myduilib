local MainContent = FLua.Class("MainContent")

function MainContent:__constructor()
	self.pOwnerUI_ = nil
	self.pTabBar = nil 
	self.pTabLayout = nil
	self.pStatusBar = nil
end

function MainContent.new(pOwnerUI)
	local obj = MainContent()
	obj.pOwnerUI_ = pOwnerUI

	return obj
end

function MainContent:OnOpenTmpl(tmpl)
	local nCount = self.pTabLayout:GetCount()
	for i=1, nCount do
		local pLayout = self.pTabLayout:GetItemAt(i-1)
		if pLayout then
			local tmpl_ = pLayout:GetUserData2("tmpl")
			if tmpl_ == tmpl then
				local curSel = self.pTabLayout:GetCurSel()
				if curSel ~= i-1 then
					print("open tmpl ", i)
					self.pTabLayout:SelectItem(i-1)
				end
				self:OnTmplReady(tmpl)
				return
			end
		end
	end	
	print("open tmpl", tmpl)
	--標題
	local dlg = DuiLib.CDialogBuilder()
    local pBar = dlg:Create("xml/item/tabbar.xml", self.pOwnerUI_:GetPaintMgr(), function(className)
		return self.pOwnerUI_:CreateControl(className)
	end)
    assert(pBar)
    pBar:SetName("bar_" .. tmpl:GetMessageName())
    pBar:SetText(tmpl:GetTmplName())
	local ret = self.pTabBar:Add(pBar)
	assert(ret)
	pBar:SetUserData2("tmpl", tmpl)

	local dlg = DuiLib.CDialogBuilder()
	local pLayout = dlg:Create("xml/item/tmpl_layout.xml", self.pOwnerUI_:GetPaintMgr(), function(className)
		return self.pOwnerUI_:CreateControl(className)
	end)
	assert(pLayout)
	local ret = self.pTabLayout:Add(pLayout)
	assert(ret)
	print("add layout", pLayout)
	local pList = pLayout:FindSubControl("layout_list")
	pList:SetUserData2("tmpl", tmpl)
	pLayout:SetUserData2("tmpl", tmpl)

	self.pTabLayout:SelectItem(nCount)
	pBar:OnNotifyAdd(function(msg)
        if msg.sType == "menu" then
            self:OnMenuTabBar(msg.pSender:GetUserData2("tmpl"))
        elseif msg.sType == "click" then
        	self:OnOpenTmpl(msg.pSender:GetUserData2("tmpl"))
        end
    end)
			
	-- _G.coro.start(function()
	 	local curSel = self.pTabLayout:GetCurSel()
	 	local pLayout = self.pTabLayout:GetItemAt(curSel)
	 	local pList = pLayout:FindSubControl("layout_list")
	 	local tmpl = pList:GetUserData2("tmpl")
	 	local data = tmpl:SerializeToTable()
	 	for i=1, #data do
	 		local pObj = pList:AddOne()
	 		pObj:FindSubControl("filed_name"):SetText(data[i].full_name)
	 		pObj:FindSubControl("field_comment"):SetText(data[i].comment)
	 		pObj:FindSubControl("field_value"):SetText(tostring(data[i].value))
	 		--_G.coro.yield()
	 	end
	--end)

	self:OnTmplReady(tmpl)
end

function MainContent:OnCloseTmpl(tmpl)
	print("close tmpl", tmpl)
	local nCount = self.pTabLayout:GetCount()
	local curSel = self.pTabLayout:GetCurSel()
	for i=1, nCount do
		local pLayout = self.pTabLayout:GetItemAt(i-1)
		if pLayout then
			local tmpl_ = pLayout:GetUserData2("tmpl")
			if tmpl_ == tmpl then
				local nextpLayout
				if i < nCount then
					nextpLayout = self.pTabLayout:GetItemAt(i)
				elseif i == nCount and i-1>0 then
					nextpLayout = self.pTabLayout:GetItemAt(i-1-1)
				end
				print("remove tmpl", i, curSel+1, nCount, tmpl, nextpLayout, nextpLayout and nextpLayout:GetUserData2("tmpl"))
				local pList = pLayout:FindSubControl("layout_list")
				pList:RemoveAll()
				self.pTabBar:RemoveAt(i - 1)
				self.pTabLayout:Remove(pLayout)

				if curSel == i-1 and nextpLayout and nextpLayout:GetUserData2("tmpl") then
					self:OnOpenTmpl(nextpLayout:GetUserData2("tmpl"))
				end

				return
			end
		end
	end
end

function MainContent:OnTmplReady(tmpl)
	local pStatusBarFile = self.pStatusBar:FindSubControl("message_file")
	local pStatusBarName = self.pStatusBar:FindSubControl("message_name")
	pStatusBarFile:SetText("模板文件:" .. tmpl:GetFilePath())
	pStatusBarName:SetText("模板名:" .. tmpl:GetMessageName())
end

function MainContent:OnCreate()
	theApp:GetRuntimeState():GetEventMgr():AddEvent(self, _G.Event.OnOpenTmpl)
	self.pTabBar = self.pOwnerUI_:FindControl("tmpl_tabbar") 
	self.pTabLayout = self.pOwnerUI_:FindControl("tmpl_tablayout") 
	self.pStatusBar = self.pOwnerUI_:FindControl("staus_bar")
end
function MainContent:OnDestroy()
	theApp:GetRuntimeState():GetEventMgr():DelEvent(self, _G.Event.OnOpenTmpl)
	self.pTabBar = nil 
	self.pTabLayout = nil
	self.pStatusBar = nil
end

function MainContent:OnMenuTabBar(tmpl)
	if not tmpl then
		return
	end
	self:OnCloseTmpl(tmpl)
end

return MainContent