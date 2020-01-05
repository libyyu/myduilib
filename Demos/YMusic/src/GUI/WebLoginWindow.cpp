#include "stdafx.h"
#include "WebLoginWindow.h"
#include "YApp.h"
#include "YMusic.h"


CWebLoginWindow::CWebLoginWindow()
{
	theMainWin->AddReceive(this);
}
CWebLoginWindow::~CWebLoginWindow()
{
	RemoveObserver();
}
LPCTSTR CWebLoginWindow::GetWindowClsss() 
{
	return _T("WebLoginWindow");
}
LPCTSTR CWebLoginWindow::GetWindowClassName() const
{
	return CWebLoginWindow::GetWindowClsss();
}
LRESULT CWebLoginWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	sLog(_T("CWebLoginWindow::OnCreate"));

	ModifyStyle(WS_CAPTION,WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	if(!RegisterSkin(_T("xml/web-login.xml"), 0, this))
		return 0;
	GetPaintMgr()->AddNotifier(this);

	theMainWin->BroadCast();

	DuiLib::CControlUI *pWebControl = GetPaintMgr()->FindControl(_T("ie"));
	if (pWebControl)
	{
		DuiLib::CWebBrowserUI *pWebBrowser = static_cast<DuiLib::CWebBrowserUI*>(pWebControl);
		pWebBrowser->SetWebBrowserEventHandler(this);
	}
	return 0;
}

void CWebLoginWindow::GoUrl(const DuiLib::CDuiString& sUrl)
{
	sLog(_T("CWebLoginWindow::GoUrl<%s>"), sUrl.GetData());
	assert(this->IsWindow());

	DuiLib::CControlUI *pWebControl = GetPaintMgr()->FindControl(_T("ie"));
	if (pWebControl)
	{
		DuiLib::CWebBrowserUI *pWebBrowser = static_cast<DuiLib::CWebBrowserUI*>(pWebControl);
		pWebBrowser->SetHomePage(sUrl);
		pWebBrowser->Navigate2(sUrl);
	}
	
}
static BSTR tVariant2BSTR(_variant_t vString)
{
	vString.ChangeType(VT_BSTR);
	return vString.bstrVal;
}
void CWebLoginWindow::NavigateComplete2(IDispatch *pDisp,VARIANT *&url)
{
	DuiLib::CDuiString strUrl; strUrl += tVariant2BSTR(*url);
	CheckLoginSucceed(strUrl);
}

void CWebLoginWindow::CheckLoginSucceed(const DuiLib::CDuiString& strUrl)
{
	if(strUrl.IsEmpty() || _loginSucceedURL.IsEmpty()) 
		return;
	int i = strUrl.Find(_loginSucceedURL);
	if(0 != i) 
		return;

	_sToken = strUrl.Right(strUrl.GetLength() - _loginSucceedURL.GetLength());
	_sToken.IsEmpty() ? CWebLoginWindow::Close(0) : CWebLoginWindow::Close(1);
}

DuiLib::CDuiString CWebLoginWindow::GetToken()
{
	return _sToken;
}

void CWebLoginWindow::Notify(DuiLib::TNotifyUI& msg)
{
	YTrace_(_T("CWebLoginWindow::Notify class:%s,id:%s,msg:%s"),msg.pSender->GetClass(),msg.pSender->GetName().GetData(),msg.sType.GetData());
	DuiLib::CControlUI* pSender = msg.pSender;
	if (0 == msg.sType.Compare(DUI_MSGTYPE_CLICK))
	{
		if (pSender->IsName(_T("close")))
			CWebLoginWindow::Close(0);
	}
}
BOOL CWebLoginWindow::Receive(SkinChangedParam param)
{
	try
	{
		if(!IsWindow()) return FALSE;
		sLog(_T("CWebLoginWindow::Receive"));
		DuiLib::CDuiString bgimage_ = param.bgimage;
		DWORD bkcolor_ = param.bkcolor;
		DuiLib::CControlUI* background = GetPaintMgr()->GetRoot();
		if (background != NULL&&0!=bgimage_.Compare(background->GetBkImage()))
		{
			if (!param.bgimage.IsEmpty())
			{
				DuiLib::CDuiString sBkImage = bgimage_;

				background->SetBkImage(sBkImage);
			}
			else
				background->SetBkImage(_T(""));

			background->SetBkColor(param.bkcolor);
		}

		return TRUE;
	}
	catch (...)
	{
		sLogError(_T("CWebLoginWindow::Receive"));
		throw _T("CWebLoginWindow::Receive");
		return FALSE;
	}	
}

