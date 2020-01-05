#pragma once


class CRichEditWnd
	: public CLuaWindow
{
public:
	CRichEditWnd();
	~CRichEditWnd();
public:
	UINT GetClassStyle() const
	{
		return UI_CLASSSTYLE_FRAME | CS_DBLCLKS;
	}
	LPCTSTR GetWindowClassName() const;
	CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void OnFinalMessage(HWND hWnd);
	void Notify(TNotifyUI& msg);
	void OnInitWindow(TNotifyUI& msg);
	void OnClick(TNotifyUI& msg);
	void OnItemClick(TNotifyUI& msg);
	void OnItemActive(TNotifyUI& msg);
public:
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
public:
	void UpdateFriendsList();
protected:
	virtual CDuiString GetSkinPath();
	virtual CDuiString GetSkinFolder();
	virtual UILIB_RESOURCETYPE GetResourceType() const;
private:
private:
};