#ifndef __LOGIN_NOTICE_H__
#define __LOGIN_NOTICE_H__

#pragma once

enum EMT_NOTICE_TYPE
{
	EMT_NORMAL = 0,
	EMT_GROUP_MSG = 1,//Ⱥ����Ϣ
	EMT_SYS_MSG ,//ϵͳ��Ϣ
	EMT_OFFLINE_MSG ,//������Ϣ
	EMT_FRIEND_MSG,//˽����Ϣ
	EMT_FRIEND_LOAD,
	EMT_FRIEND_SHAKE
};

class CNotice
	: public CWin
	, public INotifyUI
	, public IDialogBuilderCallback
	, public IMessageFilterUI
{
protected:
	virtual CDuiString GetSkinPath();
	virtual CDuiString GetSkinFolder();
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual CDuiString GetZIPFileName() const;
public:
	static const LPCTSTR CLASSNAME;
	CNotice(HWND pParent,LPCTSTR xml,LPCTSTR text,EMT_NOTICE_TYPE type);
	~CNotice();
	LPCTSTR GetWindowClassName() const;
	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM , bool& bHandled);
	CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void Notify(TNotifyUI& msg);
	void TypeInit(TNotifyUI& msg);
	void TypeClick(TNotifyUI& msg);
	void HideNoNotice(bool bHide = true);
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam,BOOL& bHandled);
	LRESULT OnNcLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT ProcessWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	CDuiString m_sWho;
private:
	CDuiString m_strText;
	CDuiString m_xmlFile;
	EMT_NOTICE_TYPE m_NoticeType;
	
};
extern bool bGroupMsgTip;
extern bool bFriednMsgTip;
extern bool bFriednLoadTip;
extern bool bFriednShakeTip;
#endif//__LOGIN_NOTICE_H__