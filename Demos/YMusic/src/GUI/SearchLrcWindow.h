#ifndef __SearchLrcWindow_h
#define __SearchLrcWindow_h

#pragma once
#include "BaseWindow.h"

class YSearchLrcWindow
	: public YWindow
	, public DuiLib::INotifyUI
{
public:
	YSearchLrcWindow();
	virtual ~YSearchLrcWindow(){};
	static YSearchLrcWindow* Instance(HWND hWndParent);
public:
	static LPCTSTR GetWindowClsss() ;
	LPCTSTR GetWindowClassName() const;
	void OnFinalMessage( HWND hWnd );
	void Notify(DuiLib::TNotifyUI& msg);
protected:
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
private:
	static YSearchLrcWindow* m_Singleton;
};



#endif//__SearchLrcWindow_h