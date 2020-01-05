#ifndef __ADDINGSONG_H
#define __ADDINGSONG_H
#pragma once
#include "BaseWindow.h"
#include "Utility/win_io/FileMgr.h"

class YAddingSong
	:public YWindow
	,public DuiLib::INotifyUI
{
public:
	YAddingSong(bool bOpenFile, DuiLib::CListUI* pSongList);
	virtual ~YAddingSong();

	static LPCTSTR GetWindowClsss() ;
	LPCTSTR GetWindowClassName() const;
	void OnFinalMessage( HWND hWnd );
public:
	void Notify(DuiLib::TNotifyUI& msg);
	void OnClick(DuiLib::TNotifyUI& msg);
	void OnInit(DuiLib::TNotifyUI& msg);
	int ShowDetail(CFileMgr::filesArrayT files);
public:
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
private:
	DuiLib::CListUI* _pSongList;
	DuiLib::CTextUI* _pText;
	bool _bCancel;
	bool _bOpenFile;
	CFileMgr::filesArrayT _files;
};







#endif//__ADDINGSONG_H