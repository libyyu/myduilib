local CSongInfo = FLua.Class("CSongInfo")

function CSongInfo:__constructor()
	self.m_nLocalID 			= 0 	--本地ID
	self.m_bFromNet 			= false --if bLocal is true then nSongID is 0;
	self.m_nSongID 				= 0		--歌曲ID
	self.m_nResourceID 			= 0		--资源ID
	self.m_nAlbumID				= 0		--专辑ID
	self.m_nArtistID			= 0		--作者Id
	self.m_nAudioLength			= 0		--时长
	self.m_nPlayCount			= 0		--播放次数
	self.m_lAddTimeGMT			= 0		--添加时间
	self.m_lLastModifyTimeGMT 	= 0		--上次修改时间
	self.m_lLastPlayTimeGMT		= 0		--上次播放时间
	self.m_nFileType			= 0		--文件类型
	self.m_nStartPos			= 0		--文件起始位置
	self.m_nFileSize			= 0		--文件大小
	self.m_sSongName			= ""	--歌曲名称
	self.m_sAlbumName			= ""	--专辑名称
	self.m_sArtistName			= ""	--艺术家名称
	self.m_sRealArtistName		= ""	--实际艺术家名称
	self.m_sCoverLocalPath		= ""
	self.m_sLrcLocalPath		= ""
	self.m_sLocalPath			= ""
	self.m_sCoverUrl			= ""
	self.m_sLrcUrl				= ""
	self.m_sListenFileUrl		= ""
	self.m_sFileExt				= ""
	self.m_sMD5					= ""
	self.m_bAble				= false
	self.m_nFlag				= 0
	self.m_nTag					= 0
end

function CSongInfo:copy(ths)
	self.m_nLocalID = ths.m_nLocalID
	self.m_bFromNet = ths.m_bFromNet
	self.m_nSongID = ths.m_nSongID
	self.m_nResourceID = ths.m_nResourceID
	self.m_nAlbumID = ths.m_nAlbumID
	self.m_nArtistID = ths.m_nArtistID
	self.m_nAudioLength = ths.m_nAudioLength
	self.m_nPlayCount = ths.m_nPlayCount
	self.m_lAddTimeGMT = ths.m_lAddTimeGMT
	self.m_lLastModifyTimeGMT = ths.m_lLastModifyTimeGMT
	self.m_lLastPlayTimeGMT = ths.m_lLastPlayTimeGMT
	self.m_nFileType = ths.m_nFileType
	self.m_nStartPos = ths.m_nStartPos
	self.m_nFileSize = ths.m_nFileSize
	self.m_sSongName = ths.m_sSongName
	self.m_sAlbumName = ths.m_sAlbumName
	self.m_sArtistName = ths.m_sArtistName
	self.m_sRealArtistName = ths.m_sRealArtistName
	self.m_sCoverLocalPath = ths.m_sCoverLocalPath
	self.m_sLrcLocalPath = ths.m_sLrcLocalPath
	self.m_sLocalPath = ths.m_sLocalPath
	self.m_sCoverUrl = ths.m_sCoverUrl
	self.m_sLrcUrl = ths.m_sLrcUrl
	self.m_sListenFileUrl = ths.m_sListenFileUrl
	self.m_sFileExt = ths.m_sFileExt
	self.m_sMD5 = ths.m_sMD5
	self.m_bAble = ths.m_bAble
	self.m_nFlag = ths.m_nFlag
	self.m_nTag = ths.m_nTag
end

function CSongInfo:Equal(ths)
	if self.m_bFromNet and ths.m_bFromNet then
		return self.m_sListenFileUrl == ths.m_sListenFileUrl
	elseif not self.m_bFromNet and not ths.m_bFromNet then
		return self.m_sLocalPath == ths.m_sLocalPath
	else
		return false
	end
end

function CSongInfo:Less(ths)
	return self.m_nLocalID < ths.m_nLocalID
end

return CSongInfo