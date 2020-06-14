local CPlayListMgr = FLua.Class("CPlayListMgr")

--播放模式
_G.EM_MODE = Enum.make
{
	"EM_LIST_ORDER",
	"EM_LIST_LOOP",
--	EM_SINGLE,
	"EM_SINGLE_LOOP",
	"EM_RAMDON",
}

function CPlayListMgr:__ctor()
	self.m_spSongListMap = {}
	self.m_spRandomId = {}
	self.m_spCurListID = -1 --记录当前播放的列表
	self.m_spCurSong = nil --记录当前播放的歌曲
	self.m_playMode = EM_MODE.EM_LIST_ORDER
end

function CPlayListMgr:SetPlayMode(mode)
	self.m_playMode = mode
end
function CPlayListMgr:GetPlayMode()
	return self.m_playMode
end
function CPlayListMgr:GetAllPlayList()
	return self.m_spSongListMap
end
function CPlayListMgr:GetSongListByListID(u_listId)
	if self.m_spSongListMap[u_listId] then
		return self.m_spSongListMap[u_listId]
	end
	assert(false)
end
function CPlayListMgr:GetSongInfo(u_songId, u_listId)
	local songList = self:GetSongListByListID(u_listId)
	for _, spSong in ipairs(songList) do
		if spSong.m_nLocalID == u_songId then
			return spSong
		end
	end
	return nil
end

function CPlayListMgr:LoadAllPlayList()
	print("[Begin]LoadAllPlayList")
	self:Reset()
	local runtime = _G.theApp:GetRuntimeState()
	local theDbMgr = runtime:GetDbMgr()
	if runtime:GetLogUserInfo() then
		theDbMgr:SetCurrentUser(runtime:GetLogUserInfo())
	end
	--获取播放列表
	local PlayList = theDbMgr:GetUserDB():GetAllPlayLists()
	for i,v in ipairs(PlayList) do
		print("添加列表:", v.m_sName)
		self.m_spSongListMap[v.m_nLocalID] = {}
		--根据列表ID获取歌曲列表
		local SongList = theDbMgr:GetUserDB():GetPlayListSongs(v.m_nLocalID)
		for _, songInfo in ipairs(SongList) do
			local song_list = self.m_spSongListMap[v.m_nLocalID]
			table.insert(song_list, songInfo)
		end
	end
	print("[End]LoadAllPlayList")
	return #PlayList
end
function CPlayListMgr:AddList(spList)
	--添加到数据库
	local userDB = _G.theApp:GetRuntimeState():GetDbMgr():GetUserDB()
	if not userDB:AddPlayList(spList) then
		return false
	end

	self.m_spSongListMap[spList.m_nLocalID] = {}
	return true
end

function CPlayListMgr:AddSongToList(spSong, u_listId)
	local userDB = _G.theApp:GetRuntimeState():GetDbMgr():GetUserDB()
	--添加到数据库
	if not userDB:AddSongsToPlayList(spSong, u_listId) then
		return false
	end

	--添加到播放列表
	local song_list = self.m_spSongListMap[u_listId]
	table.insert(song_list, spSong)
	return true
end

function CPlayListMgr:RemoveList(u_listId)
	--从数据库删除
	local userDB = _G.theApp:GetRuntimeState():GetDbMgr():GetUserDB()
	if not userDB:RemovePlayList(u_listId) then
		return false
	end
	--从列表删除
	self.m_spSongListMap[u_listId] = nil
	return true
end
function CPlayListMgr:RemoveSongFromList(u_songId, u_listId)
	--从数据库删除
	local userDB = _G.theApp:GetRuntimeState():GetDbMgr():GetUserDB()
	if userDB:RemovePlayListSongs(u_songId, u_listId) then
		local song_list = self.m_spSongListMap[u_listId]
		if song_list then
			for i, _ in ipairs(song_list) do
				--从列表删除
				table.remove(song_list, i)
				break
			end
		end
		return true
	end
	return false
end
function CPlayListMgr:RemoveAllSongFromList(u_listId)
	--从数据库删除
	local userDB = _G.theApp:GetRuntimeState():GetDbMgr():GetUserDB()
	if userDB:RemoveAllPlayListSongs(u_listId) then
		--从列表删除
		self.m_spSongListMap[u_listId] = {}
		return true
	end
	return false
end

function CPlayListMgr:GetCurListID()
	return self.m_spCurListID
end

function CPlayListMgr:GetCurSong()
	return self.m_spCurSong
end
function CPlayListMgr:SetCurListID(spListId)
	self.m_spCurListID = spListId
end
function CPlayListMgr:SetCurSong(spSong)
	self.m_spCurSong = spSong
end

function CPlayListMgr:MakeRandId(uRandNum)
	if uRandNum < 1 then
		return
	end
	local uSizeOld = #(self.m_spRandomId)
	local uSizeNew = uSizeOld + uRandNum
	for i=uSizeOld+1, uSizeNew do
		local t = math.random(i, uSizeNew)
		self.m_spRandomId[i] = t
	end
end
function CPlayListMgr:ClearRand()
	print("ClearRand")
	self.m_spRandomId = {}
end

function CPlayListMgr:GetNextSongToPlay(u_listId)
	print("GetNextSongToPlay list_id", u_listId)
	if u_listId < 0 then
		return nil
	end
	local song_list = self.m_spSongListMap[u_listId]
	if not song_list or #song_list == 0 then
		return nil
	end
	local spSong 
	if self.m_playMode == EM_MODE.EM_LIST_ORDER then--顺序播放
		if not self.m_spCurSong then
			spSong = song_list[1]
		else
			local nIndex = self:GetCurIndexByCurSong(song_list)
			nIndex = nIndex + 1
			if nIndex <= #song_list then
				spSong = song_list[nIndex]
			end
		end
	elseif self.m_playMode == EM_MODE.EM_LIST_LOOP then--列表循环
		if not self.m_spCurSong then
			spSong = song_list[1]
		else
			local nIndex = self:GetCurIndexByCurSong(song_list)
			nIndex = nIndex + 1
			if nIndex <= #song_list then
				spSong = song_list[nIndex]
			else
				spSong = song_list[1]
			end
		end
	elseif self.m_playMode == EM_MODE.EM_SINGLE_LOOP then--单曲循环
		if not self.m_spCurSong then
			spSong = song_list[1]
		else
			local nIndex = self:GetCurIndexByCurSong(song_list)
			spSong = song_list[nIndex]
		end
	elseif self.m_playMode == EM_MODE.EM_RAMDON then--随机播放
		if #(self.m_spRandomId) == 0 then
			self:MakeRandId(#song_list)
		end
		local nIndex = table.remove(self.m_spRandomId, 1)
		spSong = song_list[nIndex]
	end

	return spSong
end
function CPlayListMgr:GetPrevSongToPlay(u_listId)
	print("GetPrevSongToPlay list_id", u_listId)
	if u_listId < 0 then
		return nil
	end
	local song_list = self.m_spSongListMap[u_listId]
	if not song_list or #song_list == 0 then
		return nil
	end
	local spSong 
	if self.m_playMode == EM_MODE.EM_LIST_ORDER then--顺序播放
		if not self.m_spCurSong then
			spSong = song_list[1]
		else
			local nIndex = self:GetCurIndexByCurSong(song_list)
			nIndex = nIndex - 1
			if nIndex > 0 then
				spSong = song_list[nIndex]
			end
		end
	elseif self.m_playMode == EM_MODE.EM_LIST_LOOP then--列表循环
		if not self.m_spCurSong then
			spSong = song_list[1]
		else
			local nIndex = self:GetCurIndexByCurSong(song_list)
			nIndex = nIndex - 1
			if nIndex > 0 then
				spSong = song_list[nIndex]
			else
				spSong = song_list[#song_list]
			end
		end
	elseif self.m_playMode == EM_MODE.EM_SINGLE_LOOP then--单曲循环
		if not self.m_spCurSong then
			spSong = song_list[1]
		else
			local nIndex = self:GetCurIndexByCurSong(song_list)
			spSong = song_list[nIndex]
		end
	elseif self.m_playMode == EM_MODE.EM_RAMDON then--随机播放
		if #(self.m_spRandomId) == 0 then
			self:MakeRandId(#song_list)
		end
		local nIndex = table.remove(self.m_spRandomId, 1)
		spSong = song_list[nIndex]
	end

	return spSong
end

--protected:
function CPlayListMgr:GetCurIndexByCurSong(song_list)
	for i, v in ipairs(song_list) do
		if v.m_nLocalID == self.m_spCurSong.m_nLocalID then
			return i
		end
	end
	return 0
end
function CPlayListMgr:Reset()
	self.m_spSongListMap = {}
	self.m_spRandomId = {}
	self.m_spCurSong = nil
	self.m_spCurListID = -1
end

function CPlayListMgr:PraseFromPath(filepath)
	local SongInfoT = require "data.SongInfo"
	local song
	local ptagInfo = AudioTagInfo.New()
	if ptagInfo:init(filepath) then
		song = SongInfoT()
		song.m_sSongName = ptagInfo:get_title()
		if string.len(song.m_sSongName) == 0 then
			song.m_sSongName = path.basename(filepath)
		end
		song.m_sAlbumName = ptagInfo:get_album()
		song.m_sArtistName = ptagInfo:get_artist()
		song.m_sLocalPath = filepath
		song.m_bFromNet = false
		song.m_lAddTimeGMT = os.time()
		--song.SetFileSize(mci::get_file_size(path));
		song.m_nStartPos = 0
		--song.SetFileExt(mci::rip_file_ext(path,true));
		song.m_sRealArtistName = ptagInfo:get_artist()
		song.m_bAble = true
	end
	ptagInfo:release()
	ptagInfo = nil
	return song
end

function CPlayListMgr:CheckSong(spSong)
	assert(spSong)
	if not spSong then
		return
	end
	if spSong.m_sAlbumName == "" then
		spSong.m_sAlbumName = "未知"
	end
	if spSong.m_sArtistName == "" then
		spSong.m_sArtistName = "未知"
	end
	if spSong.m_sRealArtistName == "" then
		spSong.m_sRealArtistName = "未知"
	end
	if spSong.m_sSongName == "" then
		spSong.m_sSongName = "未知"
	end
end

function CPlayListMgr:PreparePlay(spSongInfo, u_listId)
	if self.m_spCurListID < 0 or self.m_spCurListID ~= u_listId then
		self:ClearRand()
	end
	self:SetCurListID(u_listId)
	self:SetCurSong(spSongInfo)
	if spSongInfo then
		print("PreparePlay:", u_listId, spSongInfo.m_sSongName)
	end

	return spSongInfo
end

return CPlayListMgr