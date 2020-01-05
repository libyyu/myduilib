#ifndef __MAINWND_H_
#define __MAINWND_H_

#pragma once
#include "UIUserList.h"
#include "UIFriends.h"
#include "Login.h"

class CChatSocket;
class CListenSocket;
class CMsg;
#include "lock.h"
#include "trayIcon.h"
#include "chat_dialog.h"

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
	LPCTSTR GetWindowClassName() const;
	CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void Notify( TNotifyUI &msg );
	void TypeClick(TNotifyUI& msg);
	void TypeInitWindow(TNotifyUI& msg);
	void TypeSelectedChange(TNotifyUI& msg);
	void TypeItemActive(TNotifyUI& msg);
	void TypeItemClick(TNotifyUI& msg);
	void OnExit(TNotifyUI& msg);
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnTray(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer();
	virtual LRESULT ProcessWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	template<typename T>
	void FindControl(T& pctrl,LPCTSTR pstrName) const;
	bool BuildEnvironment();
	void InitAllControls();
	void OnSendMsg(TNotifyUI& msg);
	void FormatMsgToEditView(CDuiString sName,CDuiString sTime,CDuiString sMsg);
	void UpdateUserInfo(); 
	void EmptyUserList();
	void AddRootToFriendList();
	void DisAllConnect();
	void ShowMsg(CMsg* pMsg);
	void SetTitle(CDuiString sName);
	void MsgComming(CMsg* pMsg);
	void SetPage(int iIndex);
public:
	bool ConnectToSrv(LPCTSTR lpIP = _T("127.0.0.1"),UINT nPort = SERVERPORT);
public:
	void SendMsg(CDuiString& strText);
	void OnReceive(CChatSocket* pSocket);
	CDuiString BuildNomalMsg(CDuiString& preString,CDuiString& toWho);//聊天消息
	CDuiString BuildRegMsg();//注册
	CDuiString BulidLoadMsg();//登录
	CDuiString BulidChangeMsg();//改名
	CDuiString BulidCancelMsg();//取消登录
	CDuiString BuildShakeMsg();
//	CDuiString BuildChatMsg(CDuiString& preString);
	bool IsUserLoad(CDuiString& textStr);
	bool IsUserRegit(CDuiString& textStr);
	bool IsSystemMsg(CDuiString& textStr);
	CDuiString AddTimeMsg(CDuiString& text);
	
	void LogOut();
	bool IsToMe(CDuiString& text);
	bool IsMeSnd(CDuiString& text);
	bool CheckId(CDuiString& preIp);
private:
	bool IsNumberOfIp(CDuiString ipSeg);
public:
	bool CreateUDPSocket(CDuiString sIP,CDuiString sPort);
	void RecvFrom(CListenSocket* pSocket);
	void SendTo(CDuiString peerIP,CDuiString peerPort,CDuiString msg);
	void SendTo(User user,CDuiString msg);
	bool bIsUserOnLine(CDuiString name);
	bool FindUserFromList(CDuiString name,User& user);
	CDuiString BuildChatMsg(CDuiString sMsg,CDuiString sToWho);
	CDuiString BuildShakeMsg(CDuiString sToWho);
	void ShowChatBox(CDuiString sWho,CMsg* pMsg);
	bool IsChatBoxOpen(CDuiString sName);
	ChatDialog* FindChatBox(CDuiString sName);
	bool HasMsgForUserInQueue(CDuiString sName);
	void ClearAllVar();
	void ShakeWindow(HWND hWnd);
	void ShakeToWho(User user);
public:
	struct IControls
	{
		CButtonUI* _pMinBtn;
		CButtonUI* _pMaxBtn;
		CButtonUI* _pResBtn;
		CButtonUI* _pCloseBtn;

		CRichEditUI *_pSearchRichEdit;
		CButtonUI   *_pSearchBtn;

		COptionUI   *_pFriendOpt;
		COptionUI   *_pGroupOpt;
		COptionUI   *_pMicroBlogOpt;
		COptionUI   *_pGroupChatOpt;

		CTabLayoutUI *_pUserTab;

		CFriendsUI* _pFriendTree;
		CUserListUI* _pGroupList;
		CUserListUI* _pBlogList;

	};
public:
	CLock    m_LockuserList;
	CPtrList m_userList;
private:
	CListenSocket* m_pChatSocket;//UDP通信
	CMsg*   m_pChatMsg;//UDP通信消息处理
	CChatSocket* m_pSocket;//与服务端通信
	CDuiString m_nameCopy;
	CDuiString m_IpAddress;
	CMsg*   m_pMsgRcv;
	CLoginWnd* m_pLogin;
private:
	Node* _parent_node_online_friends;
	Node* _parent_node_offline_friends;
	Node* _parent_node_all_firends;

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
	IControls  _ui;
	CTrayIconController m_tray;
};

extern CMainWindow* theApp;
extern bool theConning;
#endif//__MAINWND_H_