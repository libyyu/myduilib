#ifndef FILE_DIALOG_H
#define FILE_DIALOG_H
#pragma once

#include <commdlg.h>
#include <string>
namespace DuiLib
{
	class DUILIB_API CFileDialog
	{
	public:
		struct __POSITION {};
		typedef __POSITION* POSITION;

		CFileDialog(const TCHAR* title = _T("Select File"));
		~CFileDialog(void);

	public:
		CDuiString ToString() const;
		void SetDefExt(LPCTSTR lpszDefExt);
		void SetFileName(LPCTSTR lpszFileName);
		void SetFlags(DWORD dwFlags);
		void SetFilter(CDuiString& lpszFilter);
		void SetMultiSel(bool bMultiSel = true);
		void SetParentWnd(HWND hParentWnd);
		void SetTitle(LPCTSTR lpszTitle);

		bool ShowOpenFileDlg();
		bool ShowSaveFileDlg();

		CDuiString GetPathName();
		CDuiString GetFileName();
		CDuiString GetFileExt();
		CDuiString GetFileTitle();
		CDuiString GetFolderPath();

		POSITION GetStartPosition();
		CDuiString GetNextPathName(POSITION& pos);

	public:
		OPENFILENAME m_stOFN;

	private:
		LPTSTR szFile;
		TCHAR m_szTitleName[MAX_PATH];
		TCHAR m_szFilter[MAX_PATH];
	};
}
#endif//FILE_DIALOG_H