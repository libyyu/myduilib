#ifndef __PATH_H__
#define __PATH_H__

#pragma once

namespace DuiLib
{
	namespace Path
	{
		DUILIB_API CDuiString GetAppPath();
		DUILIB_API CDuiString GetCurAppName(BOOL bNoExt = TRUE);
		DUILIB_API CDuiString GetCurDir();
		DUILIB_API CDuiString GetTempPath();
		DUILIB_API CDuiString GetTempFileName(LPCTSTR lpszFileName);
		DUILIB_API CDuiString GetRandomFileName(LPCTSTR lpszPath, LPCTSTR lpszFileName);

		DUILIB_API BOOL IsDirectory(LPCTSTR lpszPath);
		DUILIB_API BOOL IsFileExist(LPCTSTR lpszFileName);
		DUILIB_API BOOL IsDirectoryExist(LPCTSTR lpszPath);
		DUILIB_API BOOL CreateDirectory(LPCTSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
		DUILIB_API BOOL IsPathValid(LPCTSTR lpszPath);

		DUILIB_API CDuiString CombinePath(LPCTSTR lpszPath1, LPCTSTR lpszPath2);

		DUILIB_API CDuiString GetPathRoot(LPCTSTR lpszPath);
		DUILIB_API CDuiString GetDirectoryName(LPCTSTR lpszPath);
		DUILIB_API CDuiString GetFileName(LPCTSTR lpszPath);
		DUILIB_API CDuiString GetFileNameWithoutExtension(LPCTSTR lpszPath);
		DUILIB_API CDuiString GetExtension(LPCTSTR lpszPath);
		DUILIB_API CDuiString GetFullPath(LPCTSTR lpszPath);
	}
}

#endif//__PATH_H__