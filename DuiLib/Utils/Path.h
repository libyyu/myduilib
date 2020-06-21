#ifndef __PATH_H__
#define __PATH_H__

#pragma once

namespace DuiLib
{
	namespace Path
	{
		UILIB_API CDuiString GetAppPath();
		UILIB_API CDuiString GetCurAppName(BOOL bNoExt = TRUE);
		UILIB_API CDuiString GetCurDir();
		UILIB_API CDuiString GetTempPath();
		UILIB_API CDuiString GetTempFileName(LPCTSTR lpszFileName);
		UILIB_API CDuiString GetRandomFileName(LPCTSTR lpszPath, LPCTSTR lpszFileName);

		UILIB_API BOOL IsDirectory(LPCTSTR lpszPath);
		UILIB_API BOOL IsFileExist(LPCTSTR lpszFileName);
		UILIB_API BOOL IsDirectoryExist(LPCTSTR lpszPath);
		UILIB_API BOOL CreateDirectory(LPCTSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
		UILIB_API BOOL IsPathValid(LPCTSTR lpszPath);

		UILIB_API CDuiString CombinePath(LPCTSTR lpszPath1, LPCTSTR lpszPath2);

		UILIB_API CDuiString GetPathRoot(LPCTSTR lpszPath);
		UILIB_API CDuiString GetDirectoryName(LPCTSTR lpszPath);
		UILIB_API CDuiString GetFileName(LPCTSTR lpszPath);
		UILIB_API CDuiString GetFileNameWithoutExtension(LPCTSTR lpszPath);
		UILIB_API CDuiString GetExtension(LPCTSTR lpszPath);
		UILIB_API CDuiString GetFullPath(LPCTSTR lpszPath);
	}
}

#endif//__PATH_H__