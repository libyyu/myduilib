#pragma once

/************************************************************************
* 管理缓存文件
*   歌词文件
*   歌曲文件
*   专辑图片文件
*   Web文件
***********************************************************************/
#include "UIlib.h"

class CCacheMgr
{
public:
	CCacheMgr(void);
	~CCacheMgr(void);
public:
	void Init(const DuiLib::CDuiString& sCachePath);
	size_t GetAllCatchSize();
	void ClearAllCatch();
	void StatSize(size_t nNewFileSize) {}

	DuiLib::CDuiString GetLrcPath();
	DuiLib::CDuiString GetSongPath();
	DuiLib::CDuiString GetImagePath();
	DuiLib::CDuiString GetWebPath();

private:
	size_t GetFolderSize(const DuiLib::CDuiString& sPath);
	void CleanFolder(const DuiLib::CDuiString& sPath);
	void StatFolderSize(size_t nNewFileSize);

	size_t m_nTotalCacheSize;
	DuiLib::CDuiString m_sCachePath;
//	spOperationQueueT m_pQueue;

	DuiLib::CDuiString m_sLrcPath;
	DuiLib::CDuiString m_sSongPath;
	DuiLib::CDuiString m_sImagePath;
	DuiLib::CDuiString m_sWebPath;
};


