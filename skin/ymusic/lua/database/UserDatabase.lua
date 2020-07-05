local CDatabase = require "database.Database"
local CUserDBase = FLua.Class(CDatabase, "CUserDBase")

local function printf(fmt, ...)
	local v = ...
	if v then
		print(string.format(fmt, ...))
	else
		print(fmt)
	end
end
local function warnf(fmt, ...)
	local v = ...
	if v then
		warn(string.format(fmt, ...))
	else
		warn(fmt)
	end
end
local function errorf(fmt, ...)
	local v = ...
	if v then
		error(string.format(fmt, ...), 1)
	else
		error(fmt, 1)
	end
end
---------------------------------------------------------
function CUserDBase:__constructor()
	self.PlayListTableName 			= "play_lists" 			--列表                 --存储用户本地所有列表
	self.SongListTableName 			= "song_lists" 			--歌曲表               --每个列表都对应一个歌曲表
	self.FavoritesTableName 		= "favorites" 			--收藏				   --用户收藏表，需网络同步
	self.PlayRecordTableName 		= "play_record" 		--播放记录			   --用户本地播放记录
	self.DetailPlayRecordTableName 	= "play_record_detail" 	--详细播放记录
	self.LocalMusicPacketTableName 	= "local_music_packet" 	--本地音乐包
	self.OperationTableName 		= "operation" 			--操作				  --用户操作历史
end


--获取指定Id的列表信息
function CUserDBase:GetPlayList(nPlayListID)
	printf("[Begin]获取指定<local_id:%d>播放列表<table:%s>----数据库",nPlayListID,self.PlayListTableName)
	local sSql = "select local_id,web_id,list_name,type,is_show,flag from %s where local_id = %u;"
	local rs = self:QuerySql(sSql,self.PlayListTableName,nPlayListID)
	local PlayListInfoT = require "data.PlayListInfo"
	local info = nil
	if rs and not rs:eof() then
		if not info then
			info = PlayListInfoT()
		end
		info.m_nLocalID = rs:getIntField(0)
		info.m_nID = rs:getIntField(1)
		info.m_sName = rs:getStringField(2)
		info.m_eType = rs:getIntField(3)
		info.m_bShow = rs:getIntField(4) == 1
		info.m_nTag = rs:getIntField(5)
	end
	if rs then rs:close() end
	printf("[End]获取指定<local_id:%d>播放列表<table:%s>----数据库",nPlayListID,self.PlayListTableName)
	return info
end
--获取所有列表信息
function CUserDBase:GetAllPlayLists()
	printf("[Begin]获取播放列表<table:%s>----数据库",self.PlayListTableName)
	local sSql = "select local_id,web_id,list_name,type,is_show,flag from %s;";
	local PlayListInfoT = require "data.PlayListInfo"
	local PlayList = {}
	local rs = self:QuerySql(sSql,self.PlayListTableName)
	while true do
		if not rs or rs:eof() then
			break
		end
		local info = PlayListInfoT()
		info.m_nLocalID = rs:getIntField(0)
		info.m_nID = rs:getIntField(1)
		info.m_sName = rs:getStringField(2)
		info.m_eType = rs:getIntField(3)
		info.m_bShow = rs:getIntField(4) == 1
		info.m_nTag = rs:getIntField(5)
		PlayList[#PlayList+1] = info

		rs:nextRow()
	end
	if rs then rs:close() end
	printf("[End]获取播放列表<table:%s>----数据库",self.PlayListTableName)
	return PlayList
end 
--获取指定类型的列表
function CUserDBase:GetAllPlayListsByType(nType)
	printf("[Begin]获取<type:%d>播放列表<table:%s>----数据库",nType,self.PlayListTableName);
	local sSql = "select local_id,web_id,list_name,type,is_show,flag from %s where type=%d;"
	local PlayListInfoT = require "data.PlayListInfo"
	local PlayList = {}
	local rs = self:QuerySql(sSql,self.PlayListTableName,nType)
	while true do
		if not rs or rs:eof() then
			break
		end
		local info = PlayListInfoT()
		info.m_nLocalID = rs:getIntField(0)
		info.m_nID = rs:getIntField(1)
		info.m_sName = rs:getStringField(2)
		info.m_eType = rs:getIntField(3)
		info.m_bShow = rs:getIntField(4) == 1
		info.m_nTag = rs:getIntField(5)
		PlayList[#PlayList+1] = info

		rs:nextRow()
	end
	if rs then rs:close() end
	printf("[End]获取<type:%d>播放列表<table:%s>----数据库",nType,self.PlayListTableName)
	return PlayList
end
--添加用户列表
function CUserDBase:AddPlayList(listInfo)
	assert(listInfo)
	if not listInfo then return false end

	printf("[Begin]新建播放列表<table:%s>----数据库, %s",self.PlayListTableName, listInfo.m_sName)

	local sSql = [[insert into %s(web_id,list_name,type,is_show,gmt_create,gmt_last_modify,flag) values (%u,"%s",%u,%u,%u,%u,%u);]]

	local sNameUTF8    = listInfo.m_sName
	local gmt = os.time()
	local nErrCode = self:ExecSql(
		sSql,
		self.PlayListTableName,
		listInfo.m_nID,
		sNameUTF8,
		listInfo.m_eType,
		listInfo.m_bShow and 1 or 0,
		gmt,
		gmt,
		listInfo.m_nTag)
	local bResult = -1 ~= nErrCode
	if bResult then
		local rs = self:QuerySql("select last_insert_rowid() from %s;", self.PlayListTableName)
		if not rs or rs:eof()then
			errorf("[End]新建播放列表<table:%s>----数据库", self.PlayListTableName)
			return false
		end
		listInfo.m_nLocalID = rs:getIntField(0)
		local szBuf = string.format("%s_%d", self.SongListTableName, listInfo.m_nLocalID)
		if rs then rs:close() end
		bResult = self:CreateTableSongLists(szBuf)
	end
	printf("[End]新建播放列表<table:%s>----数据库",self.PlayListTableName)
	return bResult
end
--更新列表信息
function CUserDBase:UpdatePlayList(listInfo)
	assert(listInfo)
	if not listInfo then return false end

	printf("[Begin]更新指定<local_id:%d>播放列表<table:%s>----数据库",listInfo.m_nLocalID,self.PlayListTableName)

	local sSql = [[update %s set web_id = %u ,list_name = "%s" ,is_show = %u,gmt_last_modify = %u where local_id = %u;]]

	local sNameUTF8    = listInfo.m_sName
	local gmt = os.time()

	local nErrCode = self:ExecSql(
		sSql,
		self.PlayListTableName,
		listInfo.m_nID,
		sNameUTF8,
		listInfo.m_bShow and 1 or 0,
		gmt,
		listInfo.m_nLocalID)
	printf("[End]更新指定<local_id:%d>播放列表<table:%s>----数据库",listInfo.m_nLocalID,self.PlayListTableName)

	return -1 ~= nErrCode
end
--移除指定列表
function CUserDBase:RemovePlayList(nPlayListID)
	printf("[Begin]移除指定<local_id:%d>播放列表<table:%s>----数据库",nPlayListID,self.PlayListTableName)
	local sSql = "delete from %s where local_id = %u;"

	--self:RemoveAllPlayListSongs(nPlayListID)
	--先删除对于Id的歌曲列表
	self:DeletePlayList( nPlayListID )

	local nErrCode = self:ExecSql(sSql,self.PlayListTableName,nPlayListID)

	printf("[End]移除指定<local_id:%d>播放列表<table:%s>----数据库",nPlayListID,self.PlayListTableName)

	return -1 ~= nErrCode
end
--重命名指定列表
function CUserDBase:RenamePlayList(nPlayListID, newName)
	printf("[Begin]更新指定<local_id:%d>播放列表<table:%s>----数据库",nPlayListID,self.PlayListTableName)

	local sSql = [[update %s set list_name = "%s" gmt_last_modify = %u where local_id = %u;]]

	local sNameUTF8    = newName
	local gmt = os.time()

	local nErrCode = self:ExecSql(
		sSql,
		self.PlayListTableName,
		sNameUTF8,
		gmt,
		nPlayListID)
	printf("[End]更新指定<local_id:%d>播放列表<table:%s>----数据库",nPlayListID,self.PlayListTableName);

	return -1 ~= nErrCode
end
--检查某个名称的列表是否存在
function CUserDBase:IsPlayListExists(sPlayListName)
	printf("[Begin]检查指定<list_name:%s>播放列表<table:%s>----数据库",sPlayListName,self.PlayListTableName)
	local sSql = [[select local_id,web_id,list_name from %s where  list_name = "%s";]]
	local rs = self:QuerySql(sSql,self.PlayListTableName, sPlayListName)
	local bFind = false
	if rs and not rs:eof() then
		bFind =  true
	else 
		bFind =  false
	end
	if rs then rs:close() end

	printf("[End]检查指定<list_name:%s>播放列表<table:%s>----数据库",sPlayListName,self.PlayListTableName)
	return bFind
end
--根据列表名称获取列表信息
function CUserDBase:GetPlayListByName(sPlayListName)
	printf("[Begin]获取指定<list_name:%s>播放列表<table:%s>----数据库",sPlayListName,self.PlayListTableName)
	local sSql = [[select local_id,web_id,list_name,type,is_show,flag from %s where list_name = "%s";]]
	local PlayListInfoT = require "data.PlayListInfo"
	local PlayListInfo = nil
	local rs = self:QuerySql(sSql,self.PlayListTableName,sPlayListName)
	if rs and not rs:eof() then
		if not PlayListInfo then
			PlayListInfo = PlayListInfoT()
		end
		PlayListInfo.m_nLocalID = rs:getIntField(0)
		PlayListInfo.m_nID = rs:getIntField(1)
		PlayListInfo.m_sName = rs:getStringField(2)
		PlayListInfo.m_eType = rs:getIntField(3)
		PlayListInfo.m_bShow = rs:getIntField(4) == 1
		PlayListInfo.m_nTag = rs:getIntField(5)
	end
	if rs then rs:close() end
	printf("[End]检查指定<list_name:%s>播放列表<table:%s>----数据库",sPlayListName,self.PlayListTableName)
	return PlayListInfo
end

--获取指定列表Id的歌曲列表, nPlayListID default is 0
function CUserDBase:GetPlayListSongs(nPlayListID)
	nPlayListID = nPlayListID or 0
	local szBuf = string.format("%s_%d", self.SongListTableName,nPlayListID)
	printf("[Begin]获取列表内容<table:%s>----数据库",szBuf)

	local sSql = [[select song_local_id,song_id,song_name,artist_id,artist_name,album_id,album_name,
		cover_url,cover_path,listen_url,audio_length,is_disable,lrc_url,audio_type,audio_size,audio_start_pos,
		md5,local_path,is_fromnet,gmt_last_play,gmt_last_modify,gmt_add_time,play_count from %s;]]
	
	local SongInfoT = require "data.SongInfo"
	local SongList = {}
	local rs = self:QuerySql(sSql,szBuf)
	while true do
		if not rs or rs:eof() then
			break
		end
		local spInfo = SongInfoT()
		
		spInfo.m_nLocalID = rs:getIntField(0)
		spInfo.m_nSongID = rs:getIntField(1)
		spInfo.m_sSongName = rs:getStringField(2)
		spInfo.m_nArtistID = rs:getIntField(3)
		spInfo.m_sArtistName = rs:getStringField(4)
		spInfo.m_nAlbumID = rs:getIntField(5)
		spInfo.m_sAlbumName = rs:getStringField(6)
		spInfo.m_sCoverUrl = rs:getStringField(7)
		spInfo.m_sCoverLocalPath = rs:getStringField(8)
		spInfo.m_sListenFileUrl = rs:getStringField(9)
		spInfo.m_nAudioLength = rs:getIntField(10)
		spInfo.m_bAble = rs:getIntField(11) == 1
		spInfo.m_sLrcUrl = rs:getStringField(12)
		spInfo.m_nFileType = rs:getIntField(13)
		spInfo.m_nFileSize = rs:getIntField(14)
		spInfo.m_nStartPos = rs:getIntField(15)
		spInfo.m_sMD5 = rs:getStringField(16)
		spInfo.m_sLocalPath = rs:getStringField(17)
		spInfo.m_bFromNet = rs:getIntField(18) == 1
		spInfo.m_lLastPlayTimeGMT = rs:getIntField(19)
		spInfo.m_lLastModifyTimeGMT = rs:getIntField(20)
		spInfo.m_lAddTimeGMT = rs:getIntField(21)
		spInfo.m_nPlayCount = rs:getIntField(22)

		SongList[#SongList+1] = spInfo
		
		rs:nextRow()
	end
	rs:close()
	printf("[End]获取列表内容<table:%s>----数据库",szBuf)
	return SongList
end
--添加歌曲到指定列表 nPlayListID is 0
function CUserDBase:AddSongsToPlayList(songInfoOrSongList, nPlayListID)
	nPlayListID = nPlayListID or 0
	local function AddOneSong(songInfo)
		local szBuf = string.format("%s_%d",self.SongListTableName,nPlayListID)
		printf("[Begin]新增歌曲<table:%s>----数据库",szBuf)

		local sSql = [[insert into %s(song_id,song_name,artist_id,artist_name,album_id,
			album_name,cover_url,cover_path,listen_url,audio_length,is_disable,lrc_url,audio_type,audio_size,
			audio_start_pos,md5,local_path,is_fromnet,gmt_add_time,play_count)
			values(%u,"%s",%u,"%s",%u,"%s","%s","%s","%s",%u,%u,"%s",%u,%u,%u,"%s","%s",%u,%u,%u);]]

		local nErrCode = self:ExecSql(sSql,
			szBuf,
			songInfo.m_nSongID,
			songInfo.m_sSongName,
			songInfo.m_nArtistID,
			songInfo.m_sArtistName,
			songInfo.m_nAlbumID,
			songInfo.m_sAlbumName,
			songInfo.m_sCoverUrl,
			songInfo.m_sCoverLocalPath,
			songInfo.m_sListenFileUrl,
			songInfo.m_nAudioLength,
			songInfo.m_bAble and 1 or 0,
			songInfo.m_sLrcUrl,
			songInfo.m_nFileType,
			songInfo.m_nFileSize,
			songInfo.m_nStartPos,
			songInfo.m_sMD5,
			songInfo.m_sLocalPath,
			songInfo.m_bFromNet and 1 or 0,
			os.time(),
			songInfo.m_nPlayCount)

		if nErrCode >0 then
			local rs = self:QuerySql("select last_insert_rowid() from %s",szBuf)
			if not rs or rs:eof() then
				warnf("[End]新增歌曲<table:%s>----数据库",szBuf)
				return false
			end
			songInfo.m_nLocalID = rs:getIntField(0)
			if rs then rs:close() end
		end
		printf("[End]新增歌曲<table:%s>----数据库",szBuf)
		return true
	end
	local SongInfoT = require "data.SongInfo"
	if songInfoOrSongList:is(SongInfoT) then
		return AddOneSong(songInfoOrSongList)
	else
		if not self:BeginTransaction() then
			assert(false)
			return false
		end
		for _, songInfo in ipairs(songInfoOrSongList) do
			AddOneSong(songInfo)
		end
		if not self:Commit() then
			assert(false)
			return false 
		end
		return true
	end
end
function CUserDBase:AddSongToPlayList(songPath, nPlayListID, spSongInfo)
end
--更新指定列表Id的歌曲信息 nPlayListID default is 0
function CUserDBase:UpdatePlayListSongInfo(spSongInfoOrSongArray, nPlayListID)
	nPlayListID = nPlayListID or 0
	local function UpdateOneSong(spSongInfo)
		assert(spSongInfo)
		local szBuf = string.format("%s_%d",self.SongListTableName,nPlayListID)
		printf("[Begin]更新歌曲<table:%s>----数据库",szBuf);

		local sSql =[[update %s set song_id = %u, song_name = "%s",
			artist_id = %u,artist_name = %s,album_id = %u, album_name = %s,
			cover_url = "%s",cover_path = "%s",listen_url = "%s",audio_length = %u,
			is_disable = %u,lrc_url = "%s",audio_type = %u,audio_size = %u,
			audio_start_pos = %u,md5 = "%s",local_path = "%s",is_fromnet = %u,
			gmt_last_play = %u,gmt_last_modify = %u,play_count = %u where song_local_id = %u;]]
		local nErrCode = self:ExecSql(
			sSql,
			szBuf,
			spSongInfo.m_nSongID,
			spSongInfo.m_sSongName,
			spSongInfo.m_nArtistID,
			spSongInfo.m_sArtistName,
			spSongInfo.m_nAlbumID,
			spSongInfo.m_sAlbumName,
			spSongInfo.m_sCoverUrl,
			spSongInfo.m_sCoverLocalPath,
			spSongInfo.m_sListenFileUrl,
			spSongInfo.m_nAudioLength,
			spSongInfo.m_bAble and 1 or 0,
			spSongInfo.m_sLrcUrl,
			spSongInfo.m_nFileType,
			spSongInfo.m_nFileSize,
			spSongInfo.m_nStartPos,
			spSongInfo.m_sMD5,
			spSongInfo.m_sLocalPath,
			spSongInfo.m_bFromNet and 1 or 0,
			spSongInfo.m_lLastPlayTimeGMT,
			os.time(),			
			spSongInfo.m_nPlayCount,
			spSongInfo.m_nLocalID
			)
		printf("[End]更新歌曲<table:%s>----数据库",szBuf)
		return -1 ~= nErrCode
	end
	local SongInfoT = require "data.SongInfo"
	if spSongInfoOrSongArray:is(SongInfoT) then
		return UpdateOneSong(spSongInfoOrSongArray)
	else
		if not self:BeginTransaction() then
			assert(false)
			return false
		end
		for _, songInfo in ipairs(spSongInfoOrSongArray) do
			UpdateOneSong(songInfo)
		end
		if not self:Commit() then
			assert(false)
			return false 
		end
		return true
	end
end
--nPlayListID default is 0
function CUserDBase:UpdatePlayListSongPath(spSongInfo, nPlayListID)
	assert(spSongInfo)
	if not spSongInfo then return false end
	local sSql = [[update %s set local_path = "%s" where song_local_id = %u;]]
	local szBuf = string.format("%s_%d",self.SongListTableName,nPlayListID)
	return -1 ~= self:ExecSql(sSql,
		szBuf,
		spSongInfo.m_sLocalPath,
		spSongInfo.m_nLocalID)
end
--删除指定列表Id的歌曲
function CUserDBase:RemovePlayListSongs(spSongIdOrSongIdArray, nPlayListID)
	nPlayListID = nPlayListID or 0
	local function RemoveOne(songId)
		local sSql = "delete from %s where song_local_id = %u;"
		local szBuf = string.format("%s_%d",self.SongListTableName,nPlayListID)
		printf("[Begin]移除歌曲<table:%s>----数据库",szBuf)

		local nErrCode = self:ExecSql(
			sSql,
			szBuf,
			songId)

		printf("[End]移除歌曲<table:%s>----数据库",szBuf)

		return -1 ~= nErrCode
	end

	if type(spSongIdOrSongIdArray) == "number" then
		return RemoveOne(spSongIdOrSongIdArray)
	else
		if not self:BeginTransaction() then
			assert(false)
			return false
		end
		for _, songId in ipairs(spSongInfoOrSongArray) do
			RemoveOne(songId)
		end
		if not self:Commit() then
			assert(false)
			return false 
		end
		return true
	end
end

--清空指定列表Id的歌曲
function CUserDBase:RemoveAllPlayListSongs(nPlayListID)
	local szBuf = string.format("%s_%d",self.SongListTableName,nPlayListID)
	local sSql = "delete from %s;"
	return -1 ~= self:ExecSql(sSql,szBuf)
end
--删除指定列表Id的歌曲列表
function CUserDBase:DeletePlayList(nPlayListID)
	local szBuf = string.format("%s_%d",self.SongListTableName,nPlayListID)
	printf("[Begin]删除播放列表<table:%s>----数据库",szBuf)

	local sSql = "drop table %s;"

	local nErrCode = self:ExecSql(sSql,szBuf)

	printf("[End]删除播放列表<table:%s>----数据库",szBuf)

	return -1 ~= nErrCode
end

--我的收藏
function CUserDBase:CreateFavTable()
	printf("[Begin]创建收藏列表<table:%s>----数据库",self.FavoritesTableName)
	local sSql = [[create table %s (favorites_id Integer primary key autoincrement,
		favorites_name text,
		type int,
		gmt_create int,
		playlist_id int,
		song_id int);]]
	
	if not self:IsExistTable(self.FavoritesTableName) then
		self:ExecSql(sSql, self.FavoritesTableName)
	end

	if not self:IsExistTable(self.FavoritesTableName) then
		errorf("[End]创建收藏列表<table:%s>----数据库",self.FavoritesTableName)
		return false
	end

	printf("[End]创建收藏列表<table:%s>----数据库", self.FavoritesTableName)
	return true
end
function CUserDBase:AddFavorites(favInfoOrfavList)
end
function CUserDBase:RemoveFavoritesByIdAndType(nId,nType)
end
function CUserDBase:RemoveFavorites(favList)
end
function CUserDBase:RemoveAllFavorites()
end
function CUserDBase:GetAllFavorites()
end

--我的播放记录
function CUserDBase:AddPlayDetailRecord(nSongId, nSecond, bSkip)
end
function CUserDBase:GetRecentFavRecord()
end
function CUserDBase:GetPlayRecord(bSkipRecord)
end

--本地音乐包接口
function CUserDBase:GetLocalMusicPacket(nPlayListLocalId, nStartPos, nLimit, bSong)
end
function CUserDBase:AddMusicPacket(nPlayListLocalId, PacketOrPacketArray)
end
function CUserDBase:RemoveMusicPacket(nPlayListLocalId,Packet)
end
function CUserDBase:GetMusicPacketCount(nPlayListLocalId,bSong)
end

--Operation表操作;
function CUserDBase:CreateOperationTable()
end
function CUserDBase:GetAllOperation()
end
function CUserDBase:AddOperation(spSynOperation)
end
function CUserDBase:DelOperation(spSynOperation)
end

--protected:
--创建默认播放列表[试听列表](系统属性)
function CUserDBase:AddDefaultPlayList()
	printf("[Begin]新建试听列表----数据库");
	if self:IsPlayListExists("试听列表") then
		return true
	end
	local PlayListInfoT = require "data.PlayListInfo"
	local PlayListInfo = PlayListInfoT()
	PlayListInfo.m_sName = "试听列表"
	PlayListInfo.m_sComment = "试听列表"
	PlayListInfo.m_eType = _G.PlayListType.plt_sys
	PlayListInfo.m_bShow = true

	local bRet = self:AddPlayList(PlayListInfo)
	if not bRet then
		errorf("[End]新建试听列表----数据库")
		return false
	end

	printf("[End]新建试听列表----数据库")
	return bRet
end
--创建我的收藏列表[我的收藏](系统属性)
function CUserDBase:AddDefaultFav()
	printf("[Begin]新建我的收藏列表<table:%s>----数据库",self.FavoritesTableName);
	if self:IsPlayListExists("我的收藏") then
		return true
	end
	local PlayListInfoT = require "data.PlayListInfo"
	local spListInfo = PlayListInfoT()
	spListInfo.m_sName = "我的收藏"
	spListInfo.m_sComment = "我的收藏"
	spListInfo.m_eType = _G.PlayListType.plt_sys
	spListInfo.m_bShow = true

	local bRet = self:AddPlayList(spListInfo)
	if not bRet then
		errorf("[End]新建我的收藏列表<table:%s>----数据库", self.FavoritesTableName)
		return false
	end

	printf("[End]新建我的收藏列表<table:%s>----数据库",self.FavoritesTableName)
	return bRet
end
function CUserDBase:CreateTablePlayLists()
	printf("[Begin]创建播放列表<table:%s>----数据库",self.PlayListTableName)
	local sSql = [[create table %s (
		local_id Integer primary key autoincrement,
		web_id int,
		list_name text unique,
		type int,
		is_show int,
		gmt_create int,
		gmt_last_modify int,
		flag int);]]
	if not self:IsExistTable(self.PlayListTableName) then
		self:ExecSql(sSql, self.PlayListTableName)
	end

	if not self:IsExistTable(self.PlayListTableName) then
		errorf("[End]创建播放列表<table:%s>----数据库",self.PlayListTableName)
		return false
	end

	printf("[End]创建播放列表<table:%s>----数据库",self.PlayListTableName)

	self:AddDefaultPlayList()
	self:AddDefaultFav()

	return true
end
function CUserDBase:CreateTableSongLists(sPlayTableName)
	assert(sPlayTableName)
	printf("[Begin]创建歌曲列表<table:%s>----数据库",sPlayTableName)
	--song_local_id,song_id,song_name,artist_id,artist_name,album_id,"
	--album_name,cover_url,cover_path,listen_url,audio_length,"
	--is_disable,lrc_url,audio_type,audio_size,audio_start_pos,"
	--md5,local_path,is_fromnet,gmt_last_play,gmt_last_modify,gmt_add_time,play_count
	local sSql = [[create table %s (
		song_local_id Integer primary key autoincrement,
		song_id int,
		song_name text,
		artist_id int,
		artist_name text,
		album_id int,
		album_name text,
		cover_url text,
		cover_path text,
		listen_url text,
		audio_length int,
		is_disable int,
		lrc_url text,
		audio_type int,
		audio_size int,
		audio_start_pos int,
		md5 text,
		local_path text,
		is_fromnet int,
		gmt_last_play int,
		gmt_last_modify int,
		gmt_add_time int,
		play_count int default 0);]]
	if not self:IsExistTable(sPlayTableName) then
		self:ExecSql(sSql, sPlayTableName)
	end
	if not self:IsExistTable(sPlayTableName) then
		errorf("[End]创建歌曲列表<table:%s>----数据库",sPlayTableName)
		return false
	end

	printf("[End]创建歌曲列表<table:%s>----数据库",sPlayTableName)
	return true
end

function CUserDBase:OnCreateTables()
	local bRet1 = self:CreateTablePlayLists()--创建播放列表
	assert(bRet1)
	local bRet2 = self:CreateFavTable()--创建收藏列表
	assert(bRet2)
	local bRet3 = true--CreateOperationTable();//创建操作列表
	assert(bRet3)
	return bRet1 and bRet2 and bRet3
end

function CUserDBase:OnUpdateTables(oldVer, newVer)
	return true
end

return CUserDBase