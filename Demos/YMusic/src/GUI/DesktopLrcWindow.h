#pragma once
#include "LrcToolBar.h"
#include "LrcMask.h"
#include "Mgr/LrcMgr/lrcFileReader.h"

class CDesktopLrcWindow 
	: public YWindow 
	, public DuiLib::Singleton<CDesktopLrcWindow>
{

public:
	typedef std::shared_ptr<CLrcFileReader>  spLrcReaderT;
	typedef std::shared_ptr<CLrcToolBar>     spLrcToolBarT;
	typedef std::shared_ptr<CLrcMask>        spLrcMaskT;
public:
	CDesktopLrcWindow();
	~CDesktopLrcWindow();

	static LPCTSTR GetWindowClsss() ;
	LPCTSTR GetWindowClassName() const;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT ProcessWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	
	virtual LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnNCMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnNCMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMoving(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void LockLrc(bool bLock);
	bool IsLock()const;
	bool GetMouseLeaveTag();
	void SetBkTag(bool bDrawBK);//toolbar���ڵ���;
	spLrcToolBarT GetLrcToolBar() const;
	spLrcMaskT    GetLrcMask() const;
	void LrcChange();
	void SetStopText(const DuiLib::CDuiString& strText){m_strOnStop = strText;}
private:
	void DrawSpecialText(HDC hdc,const DuiLib::CDuiString& strText,LPRECT lpRect);

	void DrawDesktopUI();
	void OnLrcChanged(/*CNotifyCenter::NotifyT& notify*/);

private:
	DuiLib::CDuiString  m_strOnStop;
	spLrcMaskT    m_spLrcMask;
	spLrcToolBarT m_spLrcToolBar;
	spLrcReaderT  m_splrcFileReader;
	bool m_bLock;
	bool m_bDrawBk;
	bool m_bMouseOn;
	bool m_bTrace;
	int m_nTimerId;

};
