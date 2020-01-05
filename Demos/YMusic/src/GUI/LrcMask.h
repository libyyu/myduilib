#ifndef _LRCMASK_H
#define _LRCMASK_H

#pragma once

#include "BaseWindow.h"
class CDesktopLrcWindow;

class CLrcMask
	: public YWindow
	, public DuiLib::INotifyUI
{
public:
	CLrcMask(CDesktopLrcWindow* pDeskLrc);
	~CLrcMask();

	static LPCTSTR GetWindowClsss() ;
	LPCTSTR GetWindowClassName() const;
public:
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNCMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNCMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMoving(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT ProcessWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	bool GetMouseLeaveTag();
	void Notify(DuiLib::TNotifyUI& msg);
protected:
	void DrawBkMask();
private:
	bool m_bMouseOn;
	bool m_bTrace;
	CDesktopLrcWindow * m_pDeskLrc;
	int m_nTimerId ;
};






#endif//