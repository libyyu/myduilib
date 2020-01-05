#pragma once
#include "BaseWindow.h"

class CLoginWindow :
	public YWindow,
	public DuiLib::INotifyUI,
	public SkinChangedReceiver,
	public DuiLib::Singleton<CLoginWindow>
{
public:
	CLoginWindow(void);
	~CLoginWindow(void);
	static LPCTSTR GetWindowClsss() ;
	LPCTSTR GetWindowClassName() const;

	virtual void Notify(DuiLib::TNotifyUI& msg);
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//	virtual void OnWebService(url::CRequest *pRequest ,proto::base *pProto);
	virtual LRESULT OnWebService(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual void OnWebService(void *pRequest ,void *pProto);

	void Login(const DuiLib::CDuiString& email,const DuiLib::CDuiString& pwd,bool isMD5 = false,bool isKeepLogin = true);
	void LoginWithToken(const DuiLib::CDuiString& token);
	
protected:
	void OnSinaLogin();
	void OnQQLogin();
	void OnLogin();
	void OnReg();
	void OnForgetPassword();
	
	void OpenWebWindow(const DuiLib::CDuiString& sUrl);
	virtual BOOL Receive(SkinChangedParam param);
private:
	DuiLib::CDuiString m_sUserName;
	DuiLib::CDuiString m_sPasswordMd5;
	//BOOL    m_isKeepLogin;

	DuiLib::CDuiString m_forgetURL;
	DuiLib::CDuiString m_regURL;
	DuiLib::CDuiString m_sinURL;
	DuiLib::CDuiString m_qqURL;
	DuiLib::CDuiString m_webLoginSucceedURL;
	
private:
	DECLARE_SINGLETON_CLASS(CLoginWindow);
};
