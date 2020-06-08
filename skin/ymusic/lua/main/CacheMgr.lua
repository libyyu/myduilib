local CCacheMgr = FLua.Class("CCacheMgr")

function CCacheMgr:__ctor()
	self.m_sCachePath = ""
	self.m_nTotalCacheSize = 0
	self.m_sLrcPath = ""
	self.m_sSongPath = ""
	self.m_sImagePath = ""
	self.m_sWebPath = ""
end
function CCacheMgr:Init(path)
	assert(Application.CreateDirectory(path))
	self.m_sCachePath = path:gsub("\\", "/")
	if self.m_sCachePath:sub(-1,-1) ~= "/" then
		self.m_sCachePath = self.m_sCachePath .. "/"
	end

	self.m_sLrcPath = self.m_sCachePath .. "lrc/"
	self.m_sSongPath = self.m_sCachePath .. "song/"
	self.m_sImagePath = self.m_sCachePath .. "image/"
	self.m_sWebPath = self.m_sCachePath .. "web/"

	self:StatFolderSize(0)
end

function CCacheMgr:GetCacheDir()
	return self.m_sCachePath
end

function CCacheMgr:GetAllCatchSize()
	return self.m_nTotalCacheSize
end

function CCacheMgr:ClearAllCatch()
	--self:CleanFolder(self.m_sLrcPath)
	--self:CleanFolder(self.m_sSongPath)
	--self:CleanFolder(self.m_sImagePath)
	--self:CleanFolder(self.m_sWebPath)
	self.m_nTotalCacheSize = 0
end

function CCacheMgr:StatSize(nNewFileSize)
end

function CCacheMgr:GetLrcPath()
	return self.m_sLrcPath
end

function CCacheMgr:GetSongPath()
	return self.m_sSongPath
end

function CCacheMgr:GetImagePath()
	return self.m_sImagePath
end

function CCacheMgr:GetWebPath()
	return self.m_sWebPath
end

--private:
function CCacheMgr:GetFolderSize(sPath)
	local strPath = sPath:gsub("\\", "/")
	if strPath:sub(-1,-1) ~= "/" then
		strPath = strPath .. "/"
	end
	local nTotalSize = 0
	local data = Application.FindFirstFile(strPath .. "*.*")
	local bStop = false
	while true do 
		if not data or bStop then
			break
		end

		local info = Application.GetFindData(data, strPath)
		local s_file_name = strPath .. info.cFileName
		nTotalSize = nTotalSize + info.st_size
		if not Application.FindNextFile(data) then
			bStop = true
		end
	end
	Application.FindClose(data)
	return nTotalSize
end
function CCacheMgr:CleanFolder(sPath)
	local strPath = sPath:gsub("\\", "/")
	if strPath:sub(-1,-1) ~= "/" then
		strPath = strPath .. "/"
	end

	local data = Application.FindFirstFile(strPath .. "*.*")
	local bStop = false
	while true do 
		if not data or bStop then
			break
		end

		local info = Application.GetFindData(data, strPath)
		local s_file_name = strPath .. info.cFileName
		os.remove(s_file_name)
		if not Application.FindNextFile(data) then
			bStop = true
		end
	end
	Application.FindClose(data)
end
function CCacheMgr:StatFolderSize(nNewFileSize)
end

return CCacheMgr