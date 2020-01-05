#ifndef __DOWNLOADWIN_H__
#define __DOWNLOADWIN_H__

#pragma once

#include "BaseWindow.h"


/************************************************************************/
/* ���ع���������ģ��Ϊ����ģʽ                                         */
/************************************************************************/
class YDownLoadWin 
	: public YWindow
	, public DuiLib::INotifyUI
	, public SkinChangedReceiver
	, public DuiLib::CNotifyPump
	, public DuiLib::Singleton<YDownLoadWin>
{
	DECLARE_SINGLETON_CLASS(YDownLoadWin)
public:
	YDownLoadWin();
	virtual ~YDownLoadWin();

	static LPCTSTR GetWindowClass() ;
	LPCTSTR GetWindowClassName() const;
	void OnFinalMessage( HWND hWnd );
protected:
	virtual LRESULT OnCreate(UINT uMsg,LPARAM lparam,WPARAM wparam,BOOL& bHandled);

	void         InitWindow();
	virtual BOOL Receive(SkinChangedParam param);
	virtual void Notify(DuiLib::TNotifyUI& msg);
private:
};




#endif//__DOWNLOADWIN_H__