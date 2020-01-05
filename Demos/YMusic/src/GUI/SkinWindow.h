#ifndef __SkinWindow_h
#define __SkinWindow_h

#pragma once
#include "BaseWindow.h"

class CSimpleJsHandler;

class YSkinWindow
	: public YWindow
	, public DuiLib::INotifyUI
	, public SkinChangedReceiver
	, public DuiLib::Singleton<YSkinWindow>
{
public:
	YSkinWindow();
	virtual ~YSkinWindow();
public:
	static LPCTSTR GetWindowClsss() ;
	LPCTSTR GetWindowClassName() const;
	void OnFinalMessage( HWND hWnd );
	DuiLib::CControlUI* CreateControl(LPCTSTR pstrClass);
	void Notify(DuiLib::TNotifyUI& msg);
	void OnClick(DuiLib::TNotifyUI& msg);

	void LoadSysSkin();
	void LoadMySkin();
public:
	void InitJsHandler(CSimpleJsHandler* h);
protected:
	virtual BOOL Receive(SkinChangedParam param);
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	static void OnUseSkin(CSimpleJsHandler* h,LPCTSTR param, DuiLib::CDuiString& sResult);
	virtual LRESULT OnHttpfileDownload(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT ProcessWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
private:
	DECLARE_SINGLETON_CLASS(YSkinWindow);

private:
	DuiLib::CListUI* _pSysSkinLayout;
	DuiLib::CListUI* _pMySkinLayout;
	DuiLib::CTabLayoutUI* _pSkinLayout;

	DuiLib::CWebBrowserUI* _pWebSkin;
	CSimpleJsHandler* _pSkinJsHandler;
};


#endif//__SkinWindow_h