#include "stdafx.h"

#include "DownLoadWin.h"


YDownLoadWin::YDownLoadWin()
{}

YDownLoadWin::~YDownLoadWin()
{}

LPCTSTR YDownLoadWin::GetWindowClass()
{
	return _T("YDownLoadWin");
}

LPCTSTR YDownLoadWin::GetWindowClassName() const
{
	return YDownLoadWin::GetWindowClass();
}

void YDownLoadWin::OnFinalMessage( HWND hWnd )
{
	__super::OnFinalMessage( hWnd );
}

LRESULT YDownLoadWin::OnCreate(UINT uMsg,LPARAM lparam,WPARAM wparam,BOOL& bHandled)
{
	return 0;
}

void YDownLoadWin::InitWindow()
{

}

BOOL YDownLoadWin::Receive(SkinChangedParam param)
{
	return TRUE;
}

void YDownLoadWin::Notify(DuiLib::TNotifyUI& msg)
{
	return DuiLib::CNotifyPump::NotifyPump( msg );
}



