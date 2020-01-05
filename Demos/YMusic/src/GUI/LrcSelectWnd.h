#ifndef __LrcSelectWnd_h
#define __LrcSelectWnd_h

#pragma once
#include "BaseWindow.h"
#include "Mgr/LrcMgr/SearchLrcFromWeb.h"

class CLrcSelectWnd:
	public YWindow,
	public DuiLib::INotifyUI,
	public SkinChangedReceiver,
	public DuiLib::Singleton<CLrcSelectWnd>
{
public:
	CLrcSelectWnd();
	~CLrcSelectWnd();
	static LPCTSTR GetWindowClass();
	LPCTSTR GetWindowClassName() const;

	virtual void Notify(DuiLib::TNotifyUI& msg);
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
;
	void AddItem(int nIndex,const DuiLib::CDuiString& sArtist, const DuiLib::CDuiString& sSongName);
	void ClearItem();
	void InitSearchEdit(spSongInfoT spSong);
	DuiLib::CDuiString GetInputSongName();
	DuiLib::CDuiString GetInputArtistName();
	void  ReleatLocalLrc();
protected:
	virtual BOOL Receive(SkinChangedParam param);
private:
	void OnOK();
	void OnClose();
	void OnSearchLrc();
	void OnReleatLocalLrc();
private:
	int m_nTimerID;
	int m_nSeconds;
	DuiLib::CDuiString m_InPutArtist;
	DuiLib::CDuiString m_InPutSong;
	spSongInfoT m_spSongForSearch;
	CSearchLrcFromWeb m_Search;
private:
	DECLARE_SINGLETON_CLASS(CLrcSelectWnd);
};

#endif//__LrcSelectWnd_h