#ifndef __PROPERTYWIN_H_
#define __PROPERTYWIN_H_

#pragma once

#include "BaseWindow.h"
#include "Mgr/DB/ShareData.h"

class PropertyWin 
	: public YWindow
	, public DuiLib::INotifyUI
{
public:
	PropertyWin();
	virtual ~PropertyWin();
	static LPCTSTR GetWindowClsss() ;
	LPCTSTR GetWindowClassName() const;
	void OnFinalMessage( HWND hWnd );

	void InitWnd(spSongInfoT spSong);
protected:
	virtual void Notify(DuiLib::TNotifyUI& msg);
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
protected:
	virtual void OnClick(DuiLib::TNotifyUI& msg);
private:
	DuiLib::CLabelUI* m_pFileName;
	DuiLib::CLabelUI* m_pSongName;
	DuiLib::CLabelUI* m_pArtistName;
	DuiLib::CLabelUI* m_pAlbumName;
	DuiLib::CControlUI* m_pFilePos;
};






#endif//__PROPERTYWIN_H_