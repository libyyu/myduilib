#ifndef __YLogin_h
#define __YLogin_h

#pragma once
#include "BaseWindow.h"

class YLoginWindow
	: public YWindow
	, public DuiLib::INotifyUI
{
	YLoginWindow();
public:
	virtual ~YLoginWindow(){};

	static YLoginWindow* Instance(HWND hWndParent);
public:
	static LPCTSTR GetWindowClsss() ;
	LPCTSTR GetWindowClassName() const;
	void OnFinalMessage( HWND hWnd );
	void Notify(DuiLib::TNotifyUI& msg);
	void OnClick(DuiLib::TNotifyUI& msg);
protected:
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
private:
	static YLoginWindow* m_hSingleton;
};


#endif//__YLogin_h