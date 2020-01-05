#include "stdafx.h"
#include "CacheMgr.h"
#include "Utility/Convert.h"


CCacheMgr::CCacheMgr(void)
{
	m_nTotalCacheSize = 0;
}

CCacheMgr::~CCacheMgr(void)
{
}

void CCacheMgr::Init(const DuiLib::CDuiString& sCachePath)
{
	DuiLib::CDuiString sTemp = sCachePath;
	sTemp.Replace('\\', '/');
	if(sTemp.GetAt(sTemp.GetLength()-1) != _T('/'))
	{
		sTemp += _T("/");
	}
	m_sCachePath = sTemp;
	m_sLrcPath = m_sCachePath + _T("lrc/");
	em_utility::mci::confirm_dir(m_sLrcPath.GetData(),_T('/'));
	m_sSongPath = m_sCachePath + _T("song/");
	em_utility::mci::confirm_dir(m_sSongPath.GetData(),_T('/'));
	m_sImagePath = m_sCachePath + _T("image/");
	em_utility::mci::confirm_dir(m_sImagePath.GetData(),_T('/'));
	m_sWebPath = m_sCachePath + _T("web/");
	em_utility::mci::confirm_dir(m_sWebPath.GetData(),_T('/'));
	m_nTotalCacheSize = GetFolderSize(m_sLrcPath) + GetFolderSize(m_sSongPath) + GetFolderSize(m_sImagePath) + GetFolderSize(m_sWebPath);
	StatFolderSize(0);
}

void CCacheMgr::ClearAllCatch()
{	
// 	CleanFolder(m_sLrcPath);
// 	CleanFolder(m_sSongPath);
// 	CleanFolder(m_sImagePath);
// 	CleanFolder(m_sWebPath);
	m_nTotalCacheSize = 0;
}

void CCacheMgr::CleanFolder(const DuiLib::CDuiString& sPath)
{
	WIN32_FIND_DATA FIND_DATA;
	int nTotalSize = 0;
	HANDLE h_find = INVALID_HANDLE_VALUE;

	DuiLib::CDuiString sFilePath = sPath + _T("/*.*");
	h_find = ::FindFirstFile(sFilePath,&FIND_DATA);
	if (h_find == INVALID_HANDLE_VALUE) 
		return ;

	do 
	{
		DuiLib::CDuiString s_file_name = sPath;
		s_file_name += FIND_DATA.cFileName;
		em_utility::mci::delete_file(s_file_name.GetData());
	} while (::FindNextFile(h_find, &FIND_DATA));
	FindClose(h_find);
}

size_t CCacheMgr::GetFolderSize(const DuiLib::CDuiString& sPath)
{
	WIN32_FIND_DATA FIND_DATA;
	size_t nTotalSize = 0;
	HANDLE h_find = INVALID_HANDLE_VALUE;

	DuiLib::CDuiString sFilePath = sPath + _T("*.*");
	h_find = ::FindFirstFile(sFilePath,&FIND_DATA);
	if (h_find == INVALID_HANDLE_VALUE) 
		return 0;

	do 
	{
		DuiLib::CDuiString s_file_name = sPath;
		s_file_name += FIND_DATA.cFileName;
		nTotalSize += em_utility::mci::get_file_size(s_file_name.GetData());
	} while (::FindNextFile(h_find, &FIND_DATA));

	FindClose(h_find);
	return nTotalSize;
}

size_t CCacheMgr::GetAllCatchSize()
{
	return m_nTotalCacheSize;
}

void CCacheMgr::StatFolderSize(size_t nNewFileSize)
{

		
}


DuiLib::CDuiString CCacheMgr::GetLrcPath()
{
	return m_sLrcPath;
}

DuiLib::CDuiString CCacheMgr::GetSongPath()
{
	return m_sSongPath;
}

DuiLib::CDuiString CCacheMgr::GetImagePath()
{
	return m_sImagePath;
}

DuiLib::CDuiString CCacheMgr::GetWebPath()
{	
	return m_sWebPath;
}
