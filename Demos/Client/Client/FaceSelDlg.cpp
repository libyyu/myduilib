#include "StdAfx.h"
#include "FaceSelDlg.h"

CFaceSelDlg::CFaceSelDlg(void)
{

}

CFaceSelDlg::~CFaceSelDlg(void)
{
}

LPCTSTR CFaceSelDlg::GetWindowClassName() const
{
	return _T("DUI_WINDOW");
}

void CFaceSelDlg::InitWindow()
{
	m_pFaceCtrl = static_cast<CFaceCtrl*>(GetPaintMgr()->FindControl(_T("FaceCtrl1")));

	//m_pFaceCtrl->SetBkColor(RGB(255, 255, 255));
	m_pFaceCtrl->SetLineColor(RGB(223, 230, 246));
	m_pFaceCtrl->SetFocusBorderColor(RGB(0, 0, 255));
	m_pFaceCtrl->SetZoomBorderColor(RGB(0, 138, 255));
	m_pFaceCtrl->SetRowAndCol(8, 15);
	m_pFaceCtrl->SetItemSize(28, 28);
	m_pFaceCtrl->SetZoomSize(84, 84);
	m_pFaceCtrl->SetFaceList(m_lpFaceList);
	m_pFaceCtrl->SetCurPage(0);

}

CDuiString CFaceSelDlg::GetSkinFile()
{
	return _T("FaceSelDlg.xml");
}

CDuiString CFaceSelDlg::GetSkinFolder()
{
	return _T("richedit_test\\skin\\");
}
CDuiString CFaceSelDlg::GetSkinPath()
{
	return CPaintManagerUI::GetInstancePath();
}

CControlUI* CFaceSelDlg::CreateControl(LPCTSTR pstrClass)
{
	if (_tcsicmp(pstrClass, _T("FaceCtrl")) == 0)
		return new CFaceCtrl;
	return NULL;
}
void CFaceSelDlg::SetFaceList(CFaceList * lpFaceList)
{
	m_lpFaceList = lpFaceList;
}

LRESULT CFaceSelDlg::ResponseDefaultKeyEvent(WPARAM wParam)
{
	if (wParam == VK_RETURN)
	{
		return FALSE;
	}
	else if (wParam == VK_ESCAPE)
	{
		return TRUE;
	}
	return FALSE;
}

LRESULT CFaceSelDlg::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM, bool& bHandled)
{
	if (uMsg == WM_KEYDOWN)
	{
		return ResponseDefaultKeyEvent(wParam);
	}
	return FALSE;

}

void CFaceSelDlg::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

LRESULT CFaceSelDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_ACTIVATE)
	{
		if (WA_INACTIVE == (UINT)LOWORD(wParam))
			::PostMessage(m_hWnd, WM_CLOSE, NULL, NULL);
	}
	else if (uMsg == WM_CLOSE)
	{
		::DestroyWindow(m_hWnd);
		return 0;
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CFaceSelDlg::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}


LRESULT CFaceSelDlg::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	try
	{
		ModifyStyle(WS_CAPTION, WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
		if (!RegisterSkin(_T("FaceSelDlg.xml"), NULL, this))
			return 0;

		GetPaintMgr()->AddNotifier(this);
		RECT rcClient;
		::GetClientRect(*this, &rcClient);
		::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
			rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);
		//GetPaintMgr()->SetNextTabControl(true);
	}
	catch (...)
	{
		throw _T("ChatDialog::OnCreate");
	}

	return 0;
}




void CFaceSelDlg::Notify(TNotifyUI& msg)
{
	if (msg.sType == _T("click"))
	{
		if (msg.pSender == m_pFaceCtrl)
		{
			int nSelIndex = (int)msg.lParam;
			CFaceInfo * lpFaceInfo = m_pFaceCtrl->GetFaceInfo(nSelIndex);
			if (lpFaceInfo != NULL)
			{
				::PostMessage(::GetParent(m_hWnd), FACE_CTRL_SEL, (WPARAM)lpFaceInfo, NULL);
			}
			::PostMessage(m_hWnd, WM_CLOSE, NULL, NULL);
		}
	}
	else if (0 == _tcscmp(msg.sType, _T("windowinit")))
	{
		InitWindow();
	}
}