#include "stdafx.h"

#include "MainMenu.h"

YMainMenuWindow::YMainMenuWindow()
{

}

LPCTSTR YMainMenuWindow::GetWindowClsss()  
{
	return _T("YMainMenu");
}

LPCTSTR YMainMenuWindow::GetWindowClassName() const
{
	return YMainMenuWindow::GetWindowClsss();
}

void YMainMenuWindow::OnFinalMessage( HWND hWnd )
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

void YMainMenuWindow::Notify(DuiLib::TNotifyUI& msg)
{

}

LRESULT YMainMenuWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ModifyStyle(WS_CAPTION,WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	if(!RegisterSkin(_T("xml/menu_main.xml"),0,this))
		return 0;

	GetPaintMgr()->AddNotifier(this);
	RECT rcClient;
	::GetClientRect(*this, &rcClient);
	::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
		rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);
	return 0;
}

