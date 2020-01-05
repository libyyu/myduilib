#include "stdafx.h"
#include "RichEditTest.h"

CRichEditWnd::CRichEditWnd() : CLuaWindow(NULL)
{

}
CRichEditWnd::~CRichEditWnd()
{

}
LPCTSTR CRichEditWnd::GetWindowClassName() const
{
	return _T("RichEditWnd");
}

CDuiString CRichEditWnd::GetSkinPath()
{
	return CPaintManagerUI::GetInstancePath();
}
CDuiString CRichEditWnd::GetSkinFolder()
{
	return _T("richedit_test/skin");
}
UILIB_RESOURCETYPE CRichEditWnd::GetResourceType() const
{
	return UILIB_FILE;
}

CControlUI* CRichEditWnd::CreateControl(LPCTSTR pstrClass)
{
	return NULL;
}
void CRichEditWnd::Notify(TNotifyUI &msg)
{
	if (msg.sType == _T("windowinit"))
	{
		OnInitWindow(msg);
	}
	else if (msg.sType == _T("click"))
	{
		OnClick(msg);
	}
	else if (msg.sType == _T("itemclick"))
	{
		OnItemClick(msg);
	}
	else if (msg.sType == _T("itemactivate"))
	{
		OnItemActive(msg);
	}
	else if (msg.sType == (_T("selectchanged")))
	{

	}

}
void CRichEditWnd::OnInitWindow(TNotifyUI& msg)
{
	UpdateFriendsList();
}
void CRichEditWnd::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
};
void CRichEditWnd::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->IsName(_T("BTN_CLOSE")))
	{
		PostQuitMessage(0L);
		//SendMessage(WM_CLOSE, 0, 0);
	}
}
void CRichEditWnd::OnItemClick(TNotifyUI& msg)
{
	
}
void CRichEditWnd::OnItemActive(TNotifyUI& msg)
{
	
}
LRESULT CRichEditWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	try
	{
		ModifyStyle(WS_CAPTION, WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
		if (!RegisterSkin(_T("main.xml"), 0, this))
			return 0;

		GetPaintMgr()->AddNotifier(this);

		RECT rcClient;
		::GetClientRect(*this, &rcClient);
		::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
			rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);
	}
	catch (...)
	{
		throw _T("CRichEditWnd::OnCreate");
	}
	return 0;
}
void CRichEditWnd::UpdateFriendsList()
{
}