#ifndef __TAGMODIFYWIN_H
#define __TAGMODIFYWIN_H

#pragma once

#include "BaseWindow.h"
#include "Mgr/DB/ShareData.h"

class TagModifyWin 
	: public YWindow
	, public DuiLib::INotifyUI
	, public SkinChangedReceiver
	, public DuiLib::Singleton<TagModifyWin>
{
public:
	TagModifyWin();
	virtual ~TagModifyWin();
	static LPCTSTR GetWindowClass() ;
	LPCTSTR GetWindowClassName() const;

	bool   InitWnd(spSongInfoT spSong,int nListId);
protected:
	virtual void Notify(DuiLib::TNotifyUI& msg);
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual BOOL Receive(SkinChangedParam param);
protected:
	virtual void OnClick(DuiLib::TNotifyUI& msg);
	virtual void OnModify();
private:
	DECLARE_SINGLETON_CLASS(TagModifyWin)
private:
	DuiLib::CEditUI * m_pSongName;
	DuiLib::CEditUI * m_pArtistName;
	DuiLib::CEditUI * m_pAlbumName;
	spSongInfoT m_pSong;
	int       m_nListId;
};



#endif//__TAGMODIFYWIN_H