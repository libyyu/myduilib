#pragma once
#include "BaseWindow.h"
#include "MyControls/MyControls.h"
class CDesktopLrcWindow;

class CLrcToolBar :
	public YWindow,
	public DuiLib::INotifyUI,
	public SkinChangedReceiver
{
public:
	CLrcToolBar(CDesktopLrcWindow* pDeskLrc);
	~CLrcToolBar();
	static LPCTSTR GetWindowClsss() ;
	LPCTSTR GetWindowClassName() const;

	virtual void Notify(DuiLib::TNotifyUI& msg);
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT ProcessWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	bool GetMouseLeaveTag();
	void SetStatePlay(bool bPlay = true);
private:
	void Init();
	void PlayOrPause();
	void OnSongStatusChanged(/*CNotifyCenter::NotifyT& notify*/);
	virtual BOOL Receive(SkinChangedParam param);
private:
	bool m_bMouseOn;
	bool m_bTrace;
	CDesktopLrcWindow * m_pDeskLrc;
	typedef struct
	{
		DuiLib::CFadeButtonUI*       _pPreSongBtn;
		DuiLib::CFadeButtonUI*       _pPlayBtn;
		DuiLib::CFadeButtonUI*       _pPauseBtn;
		DuiLib::CFadeButtonUI*       _pNextSongBtn;

		DuiLib::CFadeButtonUI*		 _pZoomInFont;
		DuiLib::CFadeButtonUI*		 _pZoomOutFont;
		DuiLib::CFadeCheckBoxUI*     _pKaraok;
		DuiLib::CFadeButtonUI*		 _pThemeBtn;
		DuiLib::CFadeButtonUI*		 _pSetting;

		DuiLib::CFadeButtonUI*	     _pSearchBtn;

		DuiLib::CFadeButtonUI*       _pLockBtn;
		DuiLib::CFadeButtonUI*       _pPanelLrcBtn;
		DuiLib::CFadeButtonUI*       _pCloseSysBtn;
	}ui;
	ui _ui;
};
