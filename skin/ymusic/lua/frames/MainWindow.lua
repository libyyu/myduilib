local IBaseWindow = require "frames.IBaseWindow"
local ProtoUtil = require "utility.ProtoUtil"

local _uTaskbarCreatedMsg
local _uTaskbarButtonCreateMsg
local _hOnlineIcon
local _hOfflineIcon

local MainWindow = FLua.Class(IBaseWindow,"MainWindow")
function MainWindow.new()
	print("new MainWindow")
	local obj = MainWindow()
	return obj
end

function MainWindow:__ctor()
	self.m_pPlayList = nil
	self.m_pSongList = nil
	self.m_PlayListInfo = {}
	self.m_SongListInfo = {}
	self.m_bAddingSongs = false
	self.m_pPlayProgress = nil
	self.m_pTimeUsed = nil
	self.m_pTimeTotal = nil

	theApp:GetRuntimeState():GetEventMgr():AddEvent(self, _G.Event.BeginPlaying)
	theApp:GetRuntimeState():GetEventMgr():AddEvent(self, _G.Event.PlayingPosChanged)
	theApp:GetRuntimeState():GetEventMgr():AddEvent(self, _G.Event.PlayingEndReached)
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
	return "xml/main.xml"
end

function MainWindow:Release()
	self.m_pPlayList = nil
	self.m_pSongList = nil
end

function MainWindow:OnDestroy(wParam,lParam)
	if _hOnlineIcon then
		win32.DestroyIcon(_hOnlineIcon)
		_hOnlineIcon = nil
	end
	if _hOfflineIcon then
		win32.DestroyIcon(_hOfflineIcon)
		_hOfflineIcon = nil
	end
	self:Stop()
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
function MainWindow:ProcessWindowMessage(uMsgId,wParam,lParam)
	local MsgArgs = DuiLib.MsgArgs
	--[[if uMsgId == MsgArgs.WM_USER + 2500 then
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
		return true
	end]]

	return false
end

function MainWindow:OnMenuCommand(pMsg)
	if pMsg.nMenuTag == _G.emMenuType.EMT_SONGLIST then
		self:OnMenuCommandSongList(pMsg)
	end
end

function MainWindow:GetItemText(pListElement,iIndex,iSubItem)
	--print("GetItemText", pListElement, iIndex, self._message_list[iIndex+1])
	if iSubItem == 0 then
		return tostring(iIndex+1)
	elseif iSubItem == 1 then
		return "what+why"
	else
		local szBuf = self._message_list[iIndex+1] --ProtoUtil.GetMessageComment(self._message_list[iIndex+1])
		return szBuf
	end
end

function MainWindow:OnTimer(wParam,lParam)
end

function MainWindow:OnNotify(msg)
	local win = self.m_hWin
	if not msg or msg.isnil then return end
	if not msg.pSender then return end
	local msgType = msg.sType
	local sender = msg.pSender
	if not sender or sender.isnil then return end
	-- if msgType ~= "timer" then
	-- 	warn("888888", msgType, sender)
	-- end
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
end

function MainWindow:OnInitWindow()
	Application.SetMainWindow(self.m_hWin)


	local PaintManagerUI = DuiLib.CPaintManagerUI
	local path = PaintManagerUI.GetResourcePath() .. "/res"
	print(path)
	local hIcon = win32.LoadIconFromFile(path.."/YMusic.ico", nil)
	-- if hIcon then
	-- 	self:SendMessage(DuiLib.WM_SETICON, true, hIcon)
	-- 	self:SendMessage(DuiLib.WM_SETICON, false, hIcon)
	-- end

	_uTaskbarCreatedMsg 	 = win32.RegisterWindowMessage("TaskbarCreated")
	_uTaskbarButtonCreateMsg = win32.RegisterWindowMessage("TaskbarButtonCreated")
	_hOnlineIcon        	 = win32.LoadIconFromFile(path.."/YMusic.ico", nil)
	_hOfflineIcon       	 = win32.LoadIconFromFile(path.."/YMusic.ico", nil)
	--增加托盘图标
	self:AddTracyIcon()
	--初始化播放列表
	self:UpdatePlayList()

	-- 强制进行一次垃圾收集
	collectgarbage("collect")
	
	Timer.AddGlobalTimer(2, function()
		print("1111>2222")
		MainThreadTask.tick()
	end)

	--self:ShowInfo("Hello", 1)
end

function MainWindow:OnClick(msg)
	local win = self.m_hWin
	local sender = msg.pSender
	if sender:IsName("closebtn") then
		win:MsgBox("确定退出吗？","警告",0x00000030,function(ret)
			if ret == 1 then
				self:Close()
			end
		end)
	elseif sender:IsName("maxbtn") then
		win:SendMessage(DuiLib.MsgArgs.WM_SYSCOMMAND, DuiLib.SC_MAXIMIZE, 0)
		return;
	elseif sender:IsName("minbtn") then
		win:SendMessage(DuiLib.MsgArgs.WM_SYSCOMMAND, DuiLib.SC_MINIMIZE, 0)
		return
	elseif sender:IsName("btn") then
		self:OnSearch()
	elseif sender:IsName("menubtn") then
		self:OnShowMainMenu()
	elseif sender:IsName("new_list") then
		self:OnAddPlayList(true)
	elseif sender:IsName("next") then
		self:PlayNext()
	elseif sender:IsName("prev") then
		self:PlayPrev()
	elseif sender:IsName("play") then
		self:PlayOrPause(true)	
	elseif sender:IsName("pause") then
		self:PlayOrPause(false)	
	elseif sender:IsName("GoBack") then
		self:OnGoBackLayout()
	elseif sender:IsName("GoNext") then
		self:OnGoNextLayout()
	elseif sender:IsName("normenu") then
		self:OnShowSongItemMenu(msg)
	end
end

function MainWindow:OnSelectChanged(msg)
	local win = self.m_hWin
	local sender = msg.pSender
	if sender:IsName("lrc") then
		local pTab = win:FindControl("switch")
		pTab:SelectItem(sender:GetTag())
	elseif sender:IsName("volume") then
		local bChecked = sender:IsSelected()
		local theConfigMgr = theApp:GetRuntimeState():GetConfigMgr()
		theApp:GetRuntimeState():GetPlayCenter():Volume(theConfigMgr:GetNormalConfig():GetVolume())
		sender:SetToolTip(bChecked and "取消静音" or "静音")
		theConfigMgr:GetNormalConfig():SetVolumeOff(bChecked)
		local sInfoTip
		if bChecked then
			sInfoTip = "当前已静音"
		else
			local pVolumeSlider = win:FindControl("volume_slider")
			sInfoTip = string.format("当前音量%d%%", pVolumeSlider:GetValue())
		end
		--CInfoWindow::ShowInfoWindow(theMainWin->GetHWND(),_ui._pVolumeBtn->GetPos(),sInfoTip);
	end
end

function MainWindow:OnItemActivate(msg)
	local win = self.m_hWin
	local sender = msg.pSender
	--print(">>>>>>>>>>>>sender", sender:GetName())
	if sender:IsClass("TemplListItem") then
		local name = sender:GetName()
		if name:startswith("song-") then
			local thePlayListMgr = theApp:GetRuntimeState():GetPlayListMgr()
			local u_listId = self.m_pSongList:GetUserData2("__play_list_id")
			local songItem = sender:GetUserData2("__song_info")
			if songItem and u_listId then
				self:Stop()
				thePlayListMgr:PreparePlay(songItem:GetSongInfo(), u_listId)
				self:PlayOrPause(true)
			end
		elseif name:startswith("playlist-") then
			local pListItem = self:GetCurrentPlayListInfo()
			if not pListItem then
				return
			end
			local u_listId = pListItem:GetID() 
			local thePlayListMgr = theApp:GetRuntimeState():GetPlayListMgr()
			self:Stop()
			thePlayListMgr:PreparePlay(thePlayListMgr:GetNextSongToPlay(u_listId), u_listId)
			self:PlayOrPause(true)
		end
	end
end

function MainWindow:OnItemSelect(msg)
	local win = self.m_hWin
	local sender = msg.pSender
	if sender:IsName("play_list") then
		local pListItem = self:GetCurrentPlayListInfo()
		if not pListItem then
			return
		end
		if not self.m_pSongList then
			self.m_pSongList = win:FindControl("song_list")
		end
		local list_id = pListItem:GetID()
		if list_id ~= self.m_pSongList:GetUserData2("__play_list_id") then
			self.m_pSongList:SetUserData2("__play_list_id", list_id)
			self:UpdateSongList(list_id)
			self.m_pSongList:NeedUpdate()
		end
	end
end

function MainWindow:OnMenu(msg)
	local win = self.m_hWin
	local sender = msg.pSender

	if sender:IsName("song_list") then
		self:OnShowSongListMenu(msg)
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

	if sender:IsName("info") then
		win:PaintMgr():KillTimer(sender)
		sender:SetVisible(false)
	end
end

function MainWindow:AddTracyIcon()
	DuiLib.CTrayIconUI.CreateTrayIconFromIcon(self:GetHWND(), _G.IDI_APP, _hOnlineIcon, "逸听--聆听最美的音乐!", 0, self.m_hWin:PaintMgr())
	DuiLib.CTrayIconUI.ShowInfotip(" 逸听--聆听最美的音乐!", "欢迎使用")
end

function MainWindow:GetCurrentPlayListInfo()
	local index = self.m_pPlayList:GetCurSel()
	if index <0 then
		warn("index is -1")
		return nil
	end
	local pItemObj = self.m_pPlayList:GetItemAt(index)
	if not pItemObj then
		warn("can't get obj")
		return nil
	end
	local pListItem = pItemObj:GetUserData2("__list_info")
	if not pListItem then
		warn("can get __list_info")
		return nil
	end
	return pListItem
end

function MainWindow:OnAddPlayList(bSelect)
	print("添加新列表")
	local userDB = _G.theApp:GetRuntimeState():GetDbMgr():GetUserDB()
	local UserPlayListArray = userDB:GetAllPlayListsByType(_G.PlayListType.plt_user)
	if #UserPlayListArray >= MAX_USER_LIST then
		self.m_hWin:MsgBox("您的标签列表已满，请整理后再试！\n", "逸听", 16)
		return -1
	end
	local sListName = "新建列表"
	if userDB:IsPlayListExists(sListName) then
		local iIndex = 1
		sListName = string.format("新建列表%d", iIndex)
		while userDB:IsPlayListExists(sListName) do
			iIndex = iIndex + 1
			sListName = string.format("新建列表%d", iIndex)
		end
	end

	--添加列表到数据库
	local PlayListInfoT = require "data.PlayListInfo"
	local PlayListInfo = PlayListInfoT()
	PlayListInfo.m_sName = sListName
	PlayListInfo.m_sComment = sListName
	PlayListInfo.m_eType = _G.PlayListType.plt_user
	PlayListInfo.m_bShow = true
	if not _G.theApp:GetRuntimeState():GetPlayListMgr():AddList(PlayListInfo) then
		self.m_hWin:MsgBox("添加列表失败！\n", "逸听", 16)
		return -1
	end

	--添加到UI显示
	local PlayListItemT = require "frames.PlayListItem"
	local pListItem = self.m_pPlayList:AddOne()
	pListItem:SetName("list-"..PlayListInfo.m_nLocalID)

	local item = PlayListItemT.new(PlayListInfo.m_nLocalID, PlayListInfo)
	item:Init(pListItem)
	pListItem:SetUserData2("__list_info", item)
	self.m_PlayListInfo[PlayListInfo.m_nLocalID] = PlayListInfo
	local delta = not self.m_bAddingSongs and 2000 or nil
	self:ShowInfo("添加列表【"..sListName.."】成功", delta)
	if bSelect then
		self.m_pPlayList:SelectItem(self.m_pPlayList:GetCount()-1, true, true)
	end
	return PlayListInfo.m_nLocalID
end

--更新播放列表
function MainWindow:UpdatePlayList()
	print("更新播放列表")
	local win = self.m_hWin
	if not self.m_pPlayList then
		self.m_pPlayList = win:FindControl("play_list")
	end
	local thePlayListMgr = _G.theApp:GetRuntimeState():GetPlayListMgr()
	local listMap = thePlayListMgr:GetAllPlayList()
	local nCount = 0
	self.m_pPlayList:RemoveAll()
	local PlayListItemT = require "frames.PlayListItem"
	local default_list_id
	for list_id, song_list in pairs(listMap) do
		nCount = nCount + 1
		if not default_list_id then
			default_list_id = list_id
		end
		local spListInfo = _G.theApp:GetRuntimeState():GetDbMgr():GetUserDB():GetPlayList(list_id)
		assert(spListInfo)
		local pListItem = self.m_pPlayList:AddOne()
		pListItem:SetName("playlist-"..list_id)

		local item = PlayListItemT.new(list_id, spListInfo)
		item:Init(pListItem)
		pListItem:SetUserData2("__list_info", item)
		self.m_PlayListInfo[list_id] = spListInfo
	end
	if nCount == 0 then
		return
	end
	local nSelectIndex = self.m_pPlayList:GetCurSel()
	if nSelectIndex < 0 then
		print("选中列表", default_list_id)
		self.m_pPlayList:SelectItem(0)
	end
end
--更新歌曲列表
function MainWindow:UpdateSongList(list_id)
	print("更新歌曲列表", list_id)
	local win = self.m_hWin
	if not self.m_pSongList then
		self.m_pSongList = win:FindControl("song_list")
	end
	if not self.m_pSongList then
		assert(false)
		return
	end
	self.m_pSongList:RemoveAll()
	self.m_SongListInfo = {}

	local SongListItemT = require "frames.SongListItem"

	local thePlayListMgr = _G.theApp:GetRuntimeState():GetPlayListMgr()
	local song_list = thePlayListMgr:GetSongListByListID(list_id)
	for i, songInfo in ipairs(song_list) do
		local item = SongListItemT.new(songInfo)
		local pSongItem = self.m_pSongList:AddOne()
		pSongItem:SetName("song-"..item:GetLocalID())

		item:Init(pSongItem, self.m_pSongList)
		pSongItem:SetUserData2("__song_info", item)
		self.m_SongListInfo[item:GetLocalID()] = item
	end

	local pCountLabel = win:FindControl("show_count")
	pCountLabel:SetText(string.format("共%d首歌曲", self.m_pSongList:GetCount()))
end

function MainWindow:OnShowMainMenu()
	local win = self.m_hWin
	local pMenu = DuiLib.CMenuWnd.New(self:GetHWND())
	pMenu:SetTag(_G.emMenuType.EMT_MAIN)
	pMenu:Init(nil, "xml/main-menu.xml", "", win32.GetCursorPos())
end
function MainWindow:OnShowSongListMenu(msg)
	local win = self.m_hWin
	local pMenu = DuiLib.CMenuWnd.New(self:GetHWND())
	pMenu:SetTag(_G.emMenuType.EMT_SONGLIST)
	pMenu:Init(nil, "xml/song-list-menu.xml", "", win32.GetCursorPos())
end
function MainWindow:OnShowSongItemMenu(msg)
	local win = self.m_hWin
	local pMenu = DuiLib.CMenuWnd.New(self:GetHWND())
	pMenu:SetTag(_G.emMenuType.EMT_SONGITEM)
	pMenu:Init(nil, "xml/song-item-menu.xml", "", win32.GetCursorPos())
end

function MainWindow:OnAddSongsToList(song_list)
	self.m_bAddingSongs = true
	self.m_pSongList:SetEnabled(false)
	self.m_pSongList:SetMouseEnabled(false)
	self.m_pSongList:SetMouseChildEnabled(false)
	self.m_pPlayList:SetEnabled(false)
	self.m_pPlayList:SetMouseEnabled(false)
	self.m_pPlayList:SetMouseChildEnabled(false)
	local pRoot = self.m_hWin:PaintMgr():GetRoot()
	pRoot:SetMouseChildEnabled(false)
	local pAddList = self.m_hWin:FindControl("new_list")
	pAddList:SetMouseEnabled(false)
	local pSaveAsList = self.m_hWin:FindControl("store_new_list")
	pSaveAsList:SetMouseEnabled(false)
	local win = self.m_hWin
	local pInfoLayout = win:FindControl("info")
	local pInfoLabel = win:FindControl("info_label")
	pInfoLayout:SetVisible(true)
	pInfoLabel:SetText("搜索歌曲中...")

	local pCountLabel = win:FindControl("show_count")
	local SongListItemT = require "frames.SongListItem"
	--当前列表id
	local pListItem = self:GetCurrentPlayListInfo()
	local u_listId = pListItem:GetID() 
	local thePlayListMgr = _G.theApp:GetRuntimeState():GetPlayListMgr()
	local userDB = _G.theApp:GetRuntimeState():GetDbMgr():GetUserDB()
	--已经添加的歌曲数量
	local iSearched = 0
	local iLeft = #song_list
	while iLeft > 0 do
		local filepath = song_list[1]
		pInfoLabel:SetText("解析歌曲-"..filepath)
		
		--当前拥有的数量
		local nHaveSong = self.m_pSongList:GetCount()
		if nHaveSong < nMaxSongNumber then
			print("PraseFromPath", filepath)
			local spSongInfo = thePlayListMgr:PraseFromPath(filepath)
			if spSongInfo then
				--添加到数据库
				if thePlayListMgr:AddSongToList(spSongInfo, u_listId) then
					--添加到ui
					local item = SongListItemT.new(spSongInfo)
					local pSongItem = self.m_pSongList:AddOne()
					pSongItem:SetName("song-"..item:GetLocalID())
					item:Init(pSongItem, self.m_pSongList)
					pSongItem:SetUserData2("__song_info", item)
					self.m_SongListInfo[item:GetLocalID()] = item

					iSearched = iSearched + 1
					pInfoLabel:SetText("成功导入-"..filepath)
				else
					pInfoLabel:SetText("导入失败-"..filepath)
				end
			else
				pInfoLabel:SetText("解析失败-"..filepath)
			end
			iLeft = iLeft - 1
			table.remove(song_list, 1)
			pCountLabel:SetText(string.format("共%d首歌曲", self.m_pSongList:GetCount()))
		else
			u_listId = self:OnAddPlayList(true)
			if u_listId >0 then
				local spSongInfo = thePlayListMgr:PraseFromPath(filepath)
				if spSongInfo then
					--添加到数据库
					if thePlayListMgr:AddSongToList(spSongInfo, u_listId) then
						--添加到ui
						local item = SongListItemT.new(spSongInfo)
						local pSongItem = self.m_pSongList:AddOne()
						pSongItem:SetName("song-"..item:GetLocalID())
						item:Init(pSongItem, self.m_pSongList)
						pSongItem:SetUserData2("__song_info", item)
						self.m_SongListInfo[item:GetLocalID()] = item

						iSearched = iSearched + 1
						pInfoLabel:SetText("成功导入-"..filepath)
					else
						pInfoLabel:SetText("导入失败-"..filepath)
					end
				else
					pInfoLabel:SetText("解析失败-"..filepath)
				end
				iLeft = iLeft - 1
				table.remove(song_list, 1)
				pCountLabel:SetText(string.format("共%d首歌曲", self.m_pSongList:GetCount()))
			end
		end
	end

	self:ShowInfo(string.format("成功导入%d首歌曲", iSearched), 3000)
	self.m_bAddingSongs = false
	self.m_pSongList:SetEnabled(true)
	self.m_pSongList:SetMouseEnabled(true)
	self.m_pSongList:SetMouseChildEnabled(true)
	self.m_pPlayList:SetEnabled(true)
	self.m_pPlayList:SetMouseEnabled(true)
	self.m_pPlayList:SetMouseChildEnabled(true)
	pRoot:SetMouseChildEnabled(true)
	pAddList:SetMouseEnabled(true)
	pSaveAsList:SetMouseEnabled(true)
end

function MainWindow:OnMenuCommandSongList(msg)
	if msg.sItemName == "addfile" then
		print("[Begin]新增歌曲")
		local pFileDialog = DuiLib.CFileDialog("添加歌曲")
		pFileDialog:SetParentWnd(self:GetHWND())
		pFileDialog:SetMultiSel(true)
		pFileDialog:SetDefExt("*.mp3")
		pFileDialog:SetFilter("添加音频文件(*.mp3;*.ape;*.m4a;*.aac;*.ogg;*.flac;*.wav;*wma;*.cue)$*.mp3;*.ape;*.m4a;*.aac;*.ogg;*.flac;*.wav;*wma;*.cue$$")
		local b = pFileDialog:ShowOpenFileDlg()
		local files
		if b then
			files = pFileDialog:GetAllFileList()
		end
		pFileDialog:delete()
		pFileDialog = nil

		local nNewCount = files and #files or 0 
		if nNewCount == 0 then
			self:ShowInfo("目录下未搜索到歌曲文件", 2000)
			print("[End]新增歌曲")
			return
		end
		-- do --TEST
		-- 	theApp:GetRuntimeState():GetPlayCenter():PlayFile(files[1])
		-- 	return 
		-- end
		local nHaveSong = self.m_pSongList:GetCount()
		local nCount = nNewCount + nHaveSong
		if nCount>nMaxSongNumber then
			local sTip = string.format("为了考虑速率因素,每个列表长度最大为<f 1><c #F27D30>%d</c></f>\r\n多余的部分将自动添加到新建列表中", nMaxSongNumber)
			self.m_hWin:MsgBox(sTip, "逸听", 0x00000030, function(ret)
				if ret == 1 then
					self:OnAddSongsToList(files)
					print("[End]新增歌曲")
				end
			end)
		else
			self:OnAddSongsToList(files)
			print("[End]新增歌曲")
		end
	elseif msg.sItemName == "addfolder" then
		local tempfiles = win32.SHBrowseForFolder(self:GetHWND(), "选择本地歌曲目录")

	end
end

function MainWindow:ShowInfo(tip, delay)
	local win = self.m_hWin
	local pInfoLayout = win:FindControl("info")
	local pInfoLabel = win:FindControl("info_label")
	pInfoLayout:SetVisible(true)
	pInfoLabel:SetText(tip)
	if delay then
		win:PaintMgr():SetTimer(pInfoLayout,emTimerType.EMT_SHOWINFO_TIMERID,delay)
	end
end
--播放控制
function MainWindow:ShowSongInfo(songInfo)
	local win = self.m_hWin
	if songInfo then
		win:FindControl("songname"):SetText(songInfo.m_sSongName .. " - " .. songInfo.m_sArtistName)
		win:FindControl("songname"):SetToolTip(songInfo.m_sSongName .. " - " .. songInfo.m_sArtistName)
		win:FindControl("author"):SetText(songInfo.m_sArtistName)
		win:FindControl("author"):SetToolTip(songInfo.m_sArtistName)
		win:FindControl("playpro"):SetMouseEnabled(true)
		win:FindControl("playpro"):SetEnabled(true)
		win:FindControl("collect"):SetVisible(true)
		win:FindControl("share"):SetVisible(true)
		--专辑图片
	else
		win:FindControl("songname"):SetText("逸听音乐,最好听的音乐")
		win:FindControl("songname"):SetToolTip("逸听音乐,最好听的音乐")
		win:FindControl("author"):SetText("听逸听,感受声音魅丽")
		win:FindControl("author"):SetToolTip("听逸听,感受声音魅丽")
		win:FindControl("time_used"):SetText("00:00丽")
		win:FindControl("time_total"):SetText("00:00")
		win:FindControl("playpro"):SetValue(0)
		win:FindControl("playpro"):SetMouseEnabled(false)
		win:FindControl("playpro"):SetEnabled(false)
		win:FindControl("collect"):SetVisible(false)
		win:FindControl("share"):SetVisible(false)
	end
end
function MainWindow:OnSongStatusChanged(status)
	local win = self.m_hWin
	local pPlay = win:FindControl("play")
	local pPause = win:FindControl("pause")
	pPlay:SetVisible(_G.em_play_status.em_play ~= status)
	pPause:SetVisible(_G.em_play_status.em_play == status)
end
function MainWindow:PlayOrPause(bPlay)
	if bPlay then
		self:Play()
	else
		self:Pause()
	end
	self.m_pPlayList:NeedUpdate()
	self.m_pSongList:NeedUpdate()
	self:ShowSongInfo(theApp:GetRuntimeState():GetPlayListMgr():GetCurSong())
	self:OnSongStatusChanged(theApp:GetRuntimeState():GetPlayCenter():GetStatus())
end
function MainWindow:Play()
	local playCenter = theApp:GetRuntimeState():GetPlayCenter()
	playCenter:SetHWND(self.m_hWin:GetHWND())

	--当前是暂停
	if playCenter:GetStatus() == _G.em_play_status.em_pause then
		playCenter:Play()
		return
	end

	local pListItem = self:GetCurrentPlayListInfo()
	if not pListItem then
		self:Stop()
		return
	end
	local u_listId = pListItem:GetID() 
	local thePlayListMgr = theApp:GetRuntimeState():GetPlayListMgr()
	if thePlayListMgr:GetCurListID() < 0 then
		thePlayListMgr:SetCurListID(u_listId)
	end
	if not thePlayListMgr:GetCurSong() then
		thePlayListMgr:SetCurSong(thePlayListMgr:GetNextSongToPlay(thePlayListMgr:GetCurListID()))
	end
	if not thePlayListMgr:GetCurSong() then
		self:Stop()
		return
	end
	theApp:GetRuntimeState():GetPlayCenter():PlayFile(thePlayListMgr:GetCurSong().m_sLocalPath)
end
function MainWindow:Pause()
	theApp:GetRuntimeState():GetPlayCenter():Pause()
end
function MainWindow:Stop()
	theApp:GetRuntimeState():GetPlayCenter():Stop()
end

function MainWindow:PlayNext()
	self:Stop()
	local thePlayListMgr = theApp:GetRuntimeState():GetPlayListMgr()
	if thePlayListMgr:GetCurListID() < 0 then
		local pListItem = self:GetCurrentPlayListInfo()
		if not pListItem then
			self:Stop()
			return
		end
		local u_listId = pListItem:GetID() 
		thePlayListMgr:SetCurListID(u_listId)
	end
	local u_listId = thePlayListMgr:GetCurListID()
	thePlayListMgr:PreparePlay(thePlayListMgr:GetNextSongToPlay(u_listId), u_listId)
	self:PlayOrPause(true)
end
function MainWindow:PlayPrev()
	self:Stop()
	local thePlayListMgr = theApp:GetRuntimeState():GetPlayListMgr()
	if thePlayListMgr:GetCurListID() < 0 then
		local pListItem = self:GetCurrentPlayListInfo()
		if not pListItem then
			self:Stop()
			return
		end
		local u_listId = pListItem:GetID() 
		thePlayListMgr:SetCurListID(u_listId)
	end
	local u_listId = thePlayListMgr:GetCurListID()
	thePlayListMgr:PreparePlay(thePlayListMgr:GetPrevSongToPlay(u_listId), u_listId)
	self:PlayOrPause(true)
end

function MainWindow:BeginPlaying(...)
	warn("BeginPlaying")
	local timeTotal = theApp:GetRuntimeState():GetPlayCenter():GetTotalTime() / 1000
	local tmTotal = helper.gmtime(timeTotal)
	local szTotal = helper.strftime(tmTotal)
	if not self.m_pTimeTotal then
		self.m_pTimeTotal = self.m_hWin:FindControl("time_total")
	end
	self.m_pTimeTotal:SetText(szTotal)
end
function MainWindow:PlayingPosChanged(...)
	if not self.m_pPlayProgress then
		self.m_pPlayProgress = self.m_hWin:FindControl("playpro")
	end
	local p = theApp:GetRuntimeState():GetPlayCenter():GetPos()
	self.m_pPlayProgress:SetValue(p)

	local timeCurrent = theApp:GetRuntimeState():GetPlayCenter():GetTime() / 1000
	
	local tmCurrent = helper.gmtime(timeCurrent)
	
	local szCurrent = helper.strftime(tmCurrent)
	
	if not self.m_pTimeUsed then
		self.m_pTimeUsed = self.m_hWin:FindControl("time_used")
	end
	
	self.m_pTimeUsed:SetText(szCurrent)

	local timeTotal = theApp:GetRuntimeState():GetPlayCenter():GetTotalTime() / 1000
	local tmTotal = helper.gmtime(timeTotal)
	local szTotal = helper.strftime(tmTotal)
	if not self.m_pTimeTotal then
		self.m_pTimeTotal = self.m_hWin:FindControl("time_total")
	end
	self.m_pTimeTotal:SetText(szTotal)
end
function MainWindow:PlayingEndReached(...)
	warn("PlayingEndReached")
end

function MainWindow:OnGoBackLayout()
	local win = self.m_hWin
	local pLrcViewTab = win:FindControl("lrcview_layout")
	local pTabLayout = win:FindControl("switch")
	if pLrcViewTab:GetTag() == 0 then
		pTabLayout:SelectItem(0)
	else
		pLrcViewTab:SetTag(pLrcViewTab:GetTag()-1)
		pLrcViewTab:SelectItem(pLrcViewTab:GetTag())
		if pLrcViewTab:GetTag() + 2 == pLrcViewTab:GetCount() then
			local pUI = win:FindControl("GoNext")
			pUI:SetVisible(true)
		end
	end
end

function MainWindow:OnGoNextLayout()
	local win = self.m_hWin
	local pLrcViewTab = win:FindControl("lrcview_layout")
	local nIndex = pLrcViewTab:GetTag()+1
	if nIndex == pLrcViewTab:GetCount()-1 then
		local pUI = win:FindControl("GoNext")
		pUI:SetVisible(false)
	end
	pLrcViewTab:SetTag(nIndex)
	pLrcViewTab:SelectItem(pLrcViewTab:GetTag())
end

return MainWindow