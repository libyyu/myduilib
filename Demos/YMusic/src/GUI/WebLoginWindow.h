#pragma once
#include "BaseWindow.h"

class CWebLoginWindow : 
	public YWindow,
	public DuiLib::CWebBrowserEventHandler,
	public DuiLib::INotifyUI,
	public SkinChangedReceiver
{
public:
	CWebLoginWindow();
	~CWebLoginWindow();
	static LPCTSTR GetWindowClsss() ;
	LPCTSTR GetWindowClassName() const;

	void GoUrl(const DuiLib::CDuiString& sUrl);
	void SetLoginSucceedURL(const DuiLib::CDuiString& str){_loginSucceedURL = str;}
	DuiLib::CDuiString GetToken();
	virtual void Notify(DuiLib::TNotifyUI& msg);
protected:
	virtual BOOL Receive(SkinChangedParam param);
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void CheckLoginSucceed(const DuiLib::CDuiString& strUrl);
	virtual void NavigateComplete2(IDispatch *pDisp,VARIANT *&url);

	DuiLib::CDuiString        _loginSucceedURL;
	DuiLib::CDuiString        _sToken;
};
