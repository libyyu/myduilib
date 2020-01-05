#include "stdafx.h"
#include "FilePathMgr.h"
#include <Shlobj.h>
#include "Utility/Convert.h"
#include "YApp.h"

CFilePathMgr::CFilePathMgr()
{
	m_sTemplateDatabasePath = GetAppPath();
	em_utility::mci::confirm_dir(m_sTemplateDatabasePath,_T('/'));
	m_sDatabasePath = GetDataPath()+ _T("db/");
	em_utility::mci::confirm_dir(m_sDatabasePath,_T('/'));	
	//m_sExpressionPath = GetAppPath()+ _T("gif/");
	//em_utility::misc::confirm_dir(m_sExpressionPath,_T('/'));
	m_sDefaultCachePath = GetDataPath()+_T("cache/");
	em_utility::mci::confirm_dir(m_sDefaultCachePath,_T('/'));
	m_sLogPath = GetDataPath() + _T("log/");
	em_utility::mci::confirm_dir(m_sLogPath,_T('/'));
}

CFilePathMgr::~CFilePathMgr()
{

}

DuiLib::CDuiString CFilePathMgr::GetAppPath()
{
	DuiLib::CDuiString sAppPath = em_utility::mci::get_module_path().c_str();
	sAppPath.Replace(_T('\\'),_T('/'));
	if(sAppPath.GetAt(sAppPath.GetLength()-1) != _T('/'))
	{
		sAppPath += _T("/");
	}
	return sAppPath;
}

DuiLib::CDuiString CFilePathMgr::GetDataPath()
{
	TCHAR Path[MAX_PATH] = {0};
	SHGetFolderPath(NULL, CSIDL_APPDATA,NULL,SHGFP_TYPE_CURRENT,Path);
	DuiLib::CDuiString sDataPath(Path);
	sDataPath = sDataPath +_T("\\YTing");
	sDataPath.Replace(_T('\\'),_T('/'));
	if(sDataPath.GetAt(sDataPath.GetLength()-1) != _T('/'))
	{
		sDataPath += _T("/");
	}
	return sDataPath;
}


DuiLib::CDuiString CFilePathMgr::LinkAvatarPath(unsigned int nUserID)
{
	DuiLib::CDuiString sImgPath;
	sImgPath.Format(_T("avater_%u.jpg"),nUserID);
	sImgPath = GetImageCachePath() + sImgPath;
	return sImgPath;
}

DuiLib::CDuiString CFilePathMgr::LinkAlbumCoverPath(unsigned int nSongID)
{
	DuiLib::CDuiString sImgPath;
	sImgPath.Format(_T("AlbumCover_%u.jpg"),nSongID);
	sImgPath = GetImageCachePath() + sImgPath;
	return sImgPath;
}

DuiLib::CDuiString CFilePathMgr::LinkListenPath(unsigned int nResID, const DuiLib::CDuiString& sFileName)
{
	DuiLib::CDuiString sFilePath;
	sFilePath.Format(_T("%u"),nResID);
	sFilePath = GetOnlineResourcePath() + sFilePath;
	return sFilePath;
}

/*void CFilePathMgr::SetDatabasePath(DuiLib::CDuiString sPath)
{
m_sDatabasePath = sPath;
}*/

/*void CFilePathMgr::SetOnlineResourcePath(DuiLib::CDuiString sPath)
{
m_sOnlineResourcePath = sPath;
}*/

/*void CFilePathMgr::SetImageCachePath(DuiLib::CDuiString sPath)
{
m_sImageCachePath = sPath;
}*/

/*void CFilePathMgr::SetExpressionPath(DuiLib::CDuiString sPath)
{
m_sExpressionPath = sPath;
}*/

DuiLib::CDuiString CFilePathMgr::GetDatabasePath()
{
	return m_sDatabasePath;
}

DuiLib::CDuiString CFilePathMgr::GetTemplateDataBasePath()
{
	return m_sTemplateDatabasePath;
}

DuiLib::CDuiString CFilePathMgr::GetOnlineResourcePath()
{
	return _T("");
//	return  theCacheMgr->GetSongPath();
}

DuiLib::CDuiString CFilePathMgr::GetImageCachePath()
{
	return theCacheMgr->GetImagePath();
}

DuiLib::CDuiString CFilePathMgr::GetDefaultCachePath()
{
	return m_sDefaultCachePath;
}

DuiLib::CDuiString CFilePathMgr::GetCurrentCachePath()
{
	return GetDefaultCachePath();
}

//DuiLib::CDuiString CFilePathMgr::GetExpressionPath()
//{
//	return m_sExpressionPath;
//}

DuiLib::CDuiString CFilePathMgr::GetLrcFilePath()
{

	return theCacheMgr->GetLrcPath();
}

DuiLib::CDuiString CFilePathMgr::GetWebServiceCatchPath()
{
	
	return theCacheMgr->GetWebPath();
}

DuiLib::CDuiString CFilePathMgr::GetLogPath()
{
	return m_sLogPath;
}

