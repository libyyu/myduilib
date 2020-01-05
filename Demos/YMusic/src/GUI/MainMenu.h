#ifndef __MainMenu_h
#define __MainMenu_h

#pragma once
#include "BaseWindow.h"

class YMainMenuWindow
	: public YWindow
	, public DuiLib::INotifyUI
	, public DuiLib::Singleton<YMainMenuWindow>
{
public:
	YMainMenuWindow();
	virtual ~YMainMenuWindow(){};

public:
	static LPCTSTR GetWindowClsss() ;
	LPCTSTR GetWindowClassName() const;
	void OnFinalMessage( HWND hWnd );
	void Notify(DuiLib::TNotifyUI& msg);
protected:
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
private:
	DECLARE_SINGLETON_CLASS(YMainMenuWindow);
};




#endif//__MainMenu_h