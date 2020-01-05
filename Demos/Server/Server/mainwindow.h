#ifndef __MAINWND_H_
#define __MAINWND_H_

#pragma once
#include "UIUserList.h"
#include "trayIcon.h"

class CClientSocket;
class CListeningSocket;
class CMsg;
#include "lock.h"

class CMainWindow 
	: public CWin
	, public INotifyUI
	, public IDialogBuilderCallback
{
public:
	CMainWindow();
	~CMainWindow();
	static const LPCTSTR CLASSNAME;
protected:
	virtual CDuiString GetSkinPath();
	virtual CDuiString GetSkinFolder();
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual CDuiString GetZIPFileName() const;
public:
	LPCTSTR GetClass() const;
	bool IsClass(LPCTSTR pstrClass);
	LPCTSTR GetWindowClassName() const;
	CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void Notify( TNotifyUI &msg );
	void TypeClick(TNotifyUI& msg);
	void TypeInitWindow(TNotifyUI& msg);
	void TypeSelectedChange(TNotifyUI& msg);
	void OnExit(TNotifyUI& msg);
	void OnMenu(TNotifyUI& msg);
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMenuCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnTray(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT ProcessWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void OnSend(TNotifyUI& msg);
	void SendMsg(CDuiString& strText);
	CDuiString BuildGroupMsg(CDuiString msg);
	void FormatMsgToEditView(CDuiString sName,CDuiString sTime,CDuiString sMsg,DWORD dwColor = RGB(0,0,255));
	bool BuildEnvironment();
	void InitAllControls();
	void LoadAllUserFromDB();
	void OnUpdateUserList();
	void EmptyUserList();
	void DeleteUser(User user);
	CClientSocket* GetClient(User user);
public:
	bool InitService();
	bool StopService();
public:
	void OnAccept();
	
	void OnReceive(CClientSocket* pSocket);
	void SendMsg(CDuiString& strText,CClientSocket* pSocket);
	void SendMsgToAll(CDuiString& strText);
	CDuiString AddTimeMsg(CDuiString& text);
	bool GetUserItem(CDuiString& name,CDuiString& password,CDuiString& sf);
	CDuiString FindCallName(CDuiString& loadName);
	CDuiString BuildStopService();
	CDuiString BuildForceOfflineUser();
	CDuiString BuildDelUser();
	CDuiString BuildUserListMsg();
	CDuiString BuildChatMsg(CMsg* pMsg);
	CDuiString BuildSysMsg(CMsg* pMsg,CDuiString sWhat,CDuiString sTate);
	CDuiString BuildShakeMsg(CDuiString sWho);
	void DeleteSocket(CClientSocket* pSocket);
	void RemoveUser(CDuiString name);
	CDuiString IsUserRegisted(CDuiString& name,CDuiString& password);
	bool IsUserLoged(CDuiString& name,CDuiString& sf);
	void ChangeUserState(CClientSocket* pSocket,CDuiString& name,CDuiString state=_T("ONLINE"));
	void ShakeWindow(HWND hWnd);
public:
	struct IControls
	{
		CButtonUI* _pMinBtn;
		CButtonUI* _pMaxBtn;
		CButtonUI* _pResBtn;
		CButtonUI* _pCloseBtn;

		CRichEditUI *_pSearchRichEdit;
		CButtonUI   *_pSearchBtn;

		COptionUI   *_pOnlineOpt;
		COptionUI   *_pOfflineOpt;
		COptionUI   *_pAllUerOpt;

		CUserListUI *_pOnlineUserList;
		CUserListUI *_pOfflineUserList;
		CUserListUI *_pAllUserList;
		CTabLayoutUI *_pUserTab;
	};
public:
private:
	CDuiString  m_strAddress;
	UINT     m_uPort;
	SOCKADDR m_addr;
	CListeningSocket* m_pSocketListen;//监听套接字
	CLock    m_lockConnectionList;
	CPtrList m_connectionList;//链接状态客户端链表
	CLock    m_lockUserinfoList;
	CPtrList m_userInforList;//用户链表
	CMsg*    m_pMsg;
	CLock    m_lockMsg;
private:
	CTrayIconController m_tray;
	IControls  _ui;

	CLock   m_lockEditView;
	CRichEditUI* m_pMsgView;
	CRichEditUI* m_pInput;

	bool emotion_timer_start_;

	bool bold_;
	bool italic_;
	bool underline_;
	DWORD text_color_;
	DWORD font_size_;
	CDuiString font_face_name_;
};

extern CMainWindow* theApp;
#endif//__MAINWND_H_