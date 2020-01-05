#pragma once

/**
 * 管理各种文件路径
 * 数据库文件
 * 缓存路径
 * 日志文件路径
 */

#include "UIlib.h"
class CFilePathMgr
{
public:
	CFilePathMgr();
	~CFilePathMgr();


	//void SetDatabasePath(DuiLib::CDuiString sPath);
	//void SetOnlineResourcePath(DuiLib::CDuiString sPath);
	//void SetImageCachePath(DuiLib::CDuiString sPath);
	//void SetCachePath(DuiLib::CDuiString sPath);
	//void SetExpressionPath(DuiLib::CDuiString sPath);
	DuiLib::CDuiString LinkAvatarPath(unsigned int nUserID);
	DuiLib::CDuiString LinkListenPath(unsigned int nResID, const DuiLib::CDuiString& sFileName);
	DuiLib::CDuiString LinkAlbumCoverPath(unsigned int nSongID);

	DuiLib::CDuiString GetDatabasePath();
	DuiLib::CDuiString GetTemplateDataBasePath();
	DuiLib::CDuiString GetOnlineResourcePath();
	DuiLib::CDuiString GetImageCachePath();
	DuiLib::CDuiString GetDefaultCachePath();
	DuiLib::CDuiString GetCurrentCachePath();
	//DuiLib::CDuiString GetExpressionPath();
	DuiLib::CDuiString GetLrcFilePath();
	DuiLib::CDuiString GetAppPath();
	DuiLib::CDuiString GetDataPath();
	DuiLib::CDuiString GetWebServiceCatchPath();
	DuiLib::CDuiString GetLogPath();
private:
	DuiLib::CDuiString m_sTemplateDatabasePath;
	DuiLib::CDuiString m_sDatabasePath;
	DuiLib::CDuiString m_sOnlineResourcePath;
	DuiLib::CDuiString m_sDefaultCachePath;
	DuiLib::CDuiString m_sImageCachePath;
	DuiLib::CDuiString m_sExpressionPath;
	DuiLib::CDuiString m_sLrcFilePath;
	DuiLib::CDuiString m_sWebServiceCatchPath;
	DuiLib::CDuiString m_sLogPath;
};
