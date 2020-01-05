#include "StdAfx.h"
#include "Path.h"

namespace DuiLib
{
	namespace Path
	{
		CDuiString GetAppPath()
		{
			TCHAR szPath[MAX_PATH] = { 0 };
			::GetModuleFileName(NULL, szPath, MAX_PATH);
			CDuiString strPath = GetDirectoryName(szPath);
			_tcsncpy(szPath, strPath, MAX_PATH);
			return szPath;
		}
		CDuiString GetCurAppName(BOOL bNoExt)
		{
			TCHAR szPath[MAX_PATH] = { 0 };
			::GetModuleFileName(NULL, szPath, MAX_PATH);
			if (bNoExt)
				return GetFileNameWithoutExtension(szPath);
			else
				return GetFileName(szPath);
		}

		CDuiString GetCurDir()
		{
			TCHAR szCurDir[MAX_PATH] = { 0 };
			::GetCurrentDirectory(MAX_PATH, szCurDir);
			return szCurDir;
		}
		CDuiString GetTempPath()
		{
			TCHAR szTempPath[MAX_PATH] = { 0 };
			::GetTempPath(MAX_PATH, szTempPath);
			return szTempPath;
		}
		CDuiString GetTempFileName(LPCTSTR lpszFileName)
		{
			return GetRandomFileName(GetTempPath(), lpszFileName);
		}
		CDuiString GetRandomFileName(LPCTSTR lpszPath, LPCTSTR lpszFileName)
		{
			CDuiString strPath, strFileName, strExtFileName, strFullPath;
			TCHAR szBuf[MAX_PATH] = { 0 };

			if (!IsDirectoryExist(lpszPath))
				strPath = GetCurDir();
			else
				strPath = lpszPath;

			strFileName = GetFileNameWithoutExtension(lpszFileName);
			strExtFileName = GetExtension(lpszFileName);

			for (int i = 2; i < 10000; i++)
			{
				if (strExtFileName.IsEmpty())
				{
					strFullPath = strPath;
					strFullPath += strFileName;
					_stprintf(szBuf, _T("%d"), i);

					strFullPath += szBuf;
				}
				else
				{
					strFullPath = strPath;
					strFullPath += strFileName;
					_stprintf(szBuf, _T("%d."), i);
					strFullPath += szBuf;
					strFullPath += strExtFileName;
				}

				if (!IsFileExist(strFullPath))
					return strFullPath;
			}

			return _T("");
		}

		BOOL IsDirectory(LPCTSTR lpszPath)
		{
			if (NULL == lpszPath || NULL == *lpszPath)
				return FALSE;
			DWORD dwAttr = ::GetFileAttributes(lpszPath);
			if ((dwAttr != 0xFFFFFFFF) && (dwAttr & FILE_ATTRIBUTE_DIRECTORY))
				return TRUE;
			else
				return FALSE;
		}
		BOOL IsFileExist(LPCTSTR lpszFileName)
		{
			if (NULL == lpszFileName || NULL == *lpszFileName)
				return FALSE;
			return ::GetFileAttributes(lpszFileName) != 0xFFFFFFFF;
		}
		BOOL IsDirectoryExist(LPCTSTR lpszPath)
		{
			if (NULL == lpszPath || NULL == *lpszPath)
				return FALSE;
			DWORD dwAttr = ::GetFileAttributes(lpszPath);
			if ((dwAttr != 0xFFFFFFFF) && (dwAttr & FILE_ATTRIBUTE_DIRECTORY))
				return TRUE;
			else
				return FALSE;
		}
		BOOL CreateDirectory(LPCTSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
		{
			TCHAR cPathUtil[MAX_PATH] = { 0 };
			TCHAR cTmpPath[MAX_PATH] = { 0 };
			TCHAR * lpPos = NULL;
			TCHAR cTmp = _T('\0');

			if (NULL == lpPathName || NULL == *lpPathName)
				return FALSE;

			_tcsncpy(cPathUtil, lpPathName, MAX_PATH);

			for (int i = 0; i < (int)_tcslen(cPathUtil); i++)
			{
				if (_T('\\') == cPathUtil[i])
					cPathUtil[i] = _T('/');
			}

			lpPos = _tcschr(cPathUtil, _T('/'));
			while (lpPos != NULL)
			{
				if (lpPos == cPathUtil)
				{
					lpPos++;
				}
				else
				{
					cTmp = *lpPos;
					*lpPos = _T('\0');
					_tcsncpy(cTmpPath, cPathUtil, MAX_PATH);
					if (!IsDirectoryExist(cTmpPath) && !::CreateDirectory(cTmpPath, lpSecurityAttributes))
						return FALSE;
					*lpPos = cTmp;
					lpPos++;
				}
				lpPos = _tcschr(lpPos, _T('/'));
			}

			return TRUE;
		}
		BOOL IsPathValid(LPCTSTR lpszPath)
		{
			return TRUE;
		}

		CDuiString CombinePath(LPCTSTR lpszPath1, LPCTSTR lpszPath2)
		{
			CDuiString strPath1(lpszPath1);
			CDuiString strPath2(lpszPath2);

			strPath1.Replace(_T("\\"), _T("/"));
			strPath2.Replace(_T("\\"), _T("/"));

			int nLen1 = strPath1.GetLength();
			int nLen2 = strPath2.GetLength();
			if (strPath1[nLen1 - 1] == _T('/'))
				strPath1 = strPath1.Mid(0, nLen1 - 1);
			if (strPath2[0] == _T('/'))
				strPath2 = strPath2.Mid(1, nLen2 - 1);

			if (nLen1 > 0)
			{
				strPath1 += _T("/");
				return strPath1 + strPath2;
			}
			else
				return strPath2;
		}

		CDuiString GetPathRoot(LPCTSTR lpszPath)
		{
			if (NULL == lpszPath || NULL == *lpszPath)
				return _T("");

			CDuiString strPath(lpszPath);
			strPath.Replace(_T("\\"), _T("/"));
			int nPos = strPath.Find(_T('/'));
			if (nPos != -1)
				strPath = strPath.Mid(0, nPos + 1);

			return strPath;
		}
		CDuiString GetDirectoryName(LPCTSTR lpszPath)
		{
			if (NULL == lpszPath || NULL == *lpszPath)
				return _T("");

			CDuiString strPath(lpszPath);
			strPath.Replace(_T("\\"), _T("/"));
			int nPos = strPath.ReverseFind(_T('/'));
			if (nPos != -1)
				strPath = strPath.Mid(0, nPos + 1);

			return strPath;
		}
		CDuiString GetFileName(LPCTSTR lpszPath)
		{
			if (NULL == lpszPath || NULL == *lpszPath)
				return _T("");

			CDuiString strPath(lpszPath);
			strPath.Replace(_T("\\"), _T("/"));
			int nPos = strPath.ReverseFind(_T('/'));
			if (nPos != -1)
				strPath = strPath.Mid(nPos + 1);

			return strPath;
		}
		CDuiString GetFileNameWithoutExtension(LPCTSTR lpszPath)
		{
			if (NULL == lpszPath || NULL == *lpszPath)
				return _T("");

			CDuiString strPath(lpszPath);
			strPath.Replace(_T("\\"), _T("/"));
			int nPos = strPath.ReverseFind(_T('/'));
			if (nPos != -1)
				strPath = strPath.Mid(nPos + 1);

			nPos = strPath.ReverseFind(_T('.'));
			if (nPos != -1)
				strPath = strPath.Mid(0, nPos);

			return strPath;
		}
		CDuiString GetExtension(LPCTSTR lpszPath)
		{
			if (NULL == lpszPath || NULL == *lpszPath)
				return _T("");

			CDuiString strPath(lpszPath);
			int nPos = strPath.ReverseFind(_T('.'));
			if (nPos != -1)
				strPath = strPath.Mid(nPos + 1);

			return strPath;
		}
		CDuiString GetFullPath(LPCTSTR lpszPath)
		{
			if (NULL == lpszPath || NULL == *lpszPath)
				return _T("");

			CDuiString strPath(lpszPath);
			strPath.Replace(_T("\\"), _T("/"));
			TCHAR cFirstChar = strPath.GetAt(0);
			TCHAR cSecondChar = strPath.GetAt(1);

			if (cFirstChar == _T('/'))
			{
				CDuiString strCurDir = GetAppPath();
				CDuiString strRootPath = GetPathRoot(strCurDir);
				return strRootPath + strPath;
			}
			else if (::IsCharAlpha(cFirstChar) && cSecondChar == _T(':'))
			{
				return strPath;
			}
			else
			{
				CDuiString strCurDir = GetAppPath();
				return strCurDir + strPath;
			}
		}
	}
}