#include "StdAfx.h"
#include "login.h"
#include "Msg.h"
#include "mainwindow.h"
#include "config.h"
namespace caption
{
	const TCHAR* kClose = _T("close");
	const TCHAR* kSwitch = _T("switch");
};
namespace logonView
{
	const TCHAR* kEmail = _T("email");
	const TCHAR* kPasswd = _T("password");
	const TCHAR* kLogin = _T("login");
	const TCHAR* kKeepLogin = _T("KeepLogin");
	const TCHAR* kForgetCode = _T("ForgetCode");
	const TCHAR* kRegister = _T("register");
	const TCHAR* kSina = _T("sina");
	const TCHAR* kQQ = _T("QQ");
	const TCHAR* kTip = _T("login_tip");
};
namespace registerView
{
	const TCHAR* kAccount = _T("account");
	const TCHAR* kPasswd = _T("passwd");
	const TCHAR* kPasswd2 = _T("passwd2");
	const TCHAR* kOK = _T("ok");
	const TCHAR* kTip = _T("reg_tip");
	const TCHAR* kLogin = _T("return_login");
};

const LPCTSTR CLoginWnd::CLASSNAME = _T("LoginWnd");
CLoginWnd::CLoginWnd()
	: m_pEditEmail(NULL)
	, m_pEditPwd(NULL)
	, m_pEditAcount(NULL)
	, m_pEditPassword(NULL)
	, m_pTabSwitch(NULL)
{

}
CLoginWnd::~CLoginWnd()
{

}
LPCTSTR CLoginWnd::GetWindowClassName() const
{
	return CLoginWnd::CLASSNAME;
}


CDuiString CLoginWnd::GetSkinPath()
{
	return CPaintManagerUI::GetInstancePath();
}
CDuiString CLoginWnd::GetSkinFolder()
{
	return _T("skin");
}
UILIB_RESOURCETYPE CLoginWnd::GetResourceType() const
{
	return UILIB_ZIP;
}
CDuiString CLoginWnd::GetZIPFileName() const
{
	return _T("Cli.dat");
}


CControlUI* CLoginWnd::CreateControl(LPCTSTR pstrClass)
{
	try
	{
		return NULL;
	}
	catch (...)
	{

	}
	
}
void CLoginWnd::Notify(TNotifyUI& msg)
{
	try
	{
		if( 0 == msg.sType.Compare(_T("windowinit"))){
			TypeInit(msg);
		}
		else if( 0 == msg.sType.Compare(_T("click")) ){
			TypeClick(msg);
		}		
	}
	catch (...)
	{
		throw _T("CLoginWnd::Notify");
	}
}
void CLoginWnd::TypeInit(TNotifyUI& msg)
{
	CPaintManagerUI* pPaintMgrUI = GetPaintMgr();
	ASSERT(pPaintMgrUI);
	m_pEditEmail = static_cast<CEditUI*>(pPaintMgrUI->FindControl(logonView::kEmail));
	m_pEditPwd = static_cast<CEditUI*>(pPaintMgrUI->FindControl(logonView::kPasswd));
	m_pEditAcount = static_cast<CEditUI*>(pPaintMgrUI->FindControl(registerView::kAccount));
	m_pEditPassword = static_cast<CEditUI*>(pPaintMgrUI->FindControl(registerView::kPasswd));
	m_pTabSwitch = static_cast<CTabLayoutUI*>(pPaintMgrUI->FindControl(caption::kSwitch));
	ASSERT(m_pEditEmail);
	ASSERT(m_pEditPwd);
	theApp->BringWindowToTop();
	theApp->ShowWindow(false);
	/* ���ó�ʼƤ�� */
}
void CLoginWnd::TypeClick(TNotifyUI& msg)
{
	if(msg.pSender->IsName(caption::kClose)){
		CDuiString sPath = CPaintManagerUI::GetInstancePath();
		sPath += theApp->GetWindowClassName();
		sPath += _T("\\config\\config.ini");
		CDuiString sIP = Config::ReadIP(sPath);
		CDuiString sPort = Config::ReadPort(sPath);
		Config::WriteIP(sIP,sPath);
		Config::WritePort(sPort,sPath);
//		PostQuitMessage(0L);
		theApp->OnExit(msg);
	}
	else if (msg.pSender->IsName(logonView::kLogin))
	{
		if(LogIn())
		{
			if(!theConning)
			{
				if(!theApp->ConnectToSrv())
					SetLoginTip(_T("��¼ʧ��,ԭ��: �޷����ӵ�������"));
				else
				{
					CDuiString sLoad = theApp->BulidLoadMsg();
					theApp->SendMsg(sLoad);
				}
			}
			else
			{
				CDuiString sLoad = theApp->BulidLoadMsg();
				theApp->SendMsg(sLoad);
			}
		}
		else
		{
			SetLoginTip(_T("����ȷ�����˺ź�����"));
		}
	}
	else if (msg.pSender->IsName(_T("login_now")))
	{
		if(!theConning)
		{
			if(!theApp->ConnectToSrv())
				static_cast<CTextUI*>(GetPaintMgr()->FindControl(_T("hello_friend")))->SetText(_T("��¼ʧ��,ԭ��: �޷����ӵ�������"));
			else
			{
				CDuiString sLoad = theApp->BulidLoadMsg();
				theApp->SendMsg(sLoad);
			}
		}
		else
		{
			CDuiString sLoad = theApp->BulidLoadMsg();
			theApp->SendMsg(sLoad);
		}
	}
	else if (msg.pSender->IsName(logonView::kRegister))
	{
		SetPage(1);
	}
	else if (msg.pSender->IsName(registerView::kOK))
	{
		if (RegisterIn())
		{
			if(!theConning)
			{
				if(!theApp->ConnectToSrv())
					SetRegTip(_T("��¼ʧ��,ԭ��: �޷����ӵ�������"));
				else
				{
					CDuiString sReg = theApp->BuildRegMsg();
					theApp->SendMsg(sReg);
				}
			}
			else
			{
				CDuiString sReg = theApp->BuildRegMsg();
				theApp->SendMsg(sReg);
			}
		}
		else
		{
			SetRegTip(_T("����ȷ�����˺ź�����"));
		}
	}
	else if (msg.pSender->IsName(logonView::kQQ))
	{
	}
	else if (msg.pSender->IsName(registerView::kLogin))
	{
		SetPage(0);
	}
	else if (msg.pSender->IsName(logonView::kSina))
	{
	}
	else if (msg.pSender->IsName(logonView::kQQ))
	{
	}
}
int CLoginWnd::GetPage()
{
	if(m_pTabSwitch)
		return m_pTabSwitch->GetCurSel();
	return -1;
}
void CLoginWnd::SetPage(int iIndex)
{
	switch (iIndex)
	{
	case 0:
		{
			if(m_pTabSwitch)
				m_pTabSwitch->SelectItem(0);
			SetTitle(_T("�û���¼"));
		}
		break;
	case 1:
		{
			if(m_pTabSwitch)
				m_pTabSwitch->SelectItem(1);
			SetTitle(_T("�û�ע��"));
		}
		break;
	case 2:
		{
			if(m_pTabSwitch)
				m_pTabSwitch->SelectItem(2);
			SetTitle(_T("�û���¼"));
		}
		break;
	default:ASSERT(false);
		break;
	}
}
bool CLoginWnd::LogIn()
{
	CDuiString sEmail = m_pEditEmail->GetText();
	CDuiString sPasswd = m_pEditPwd->GetText();
	bool bLogSuccess = true;
	if(sEmail.IsEmpty())
	{
		bLogSuccess = false;
	}
	if(sPasswd.IsEmpty())
	{
		bLogSuccess = false;
	}
	theUser.m_loadName = sEmail;
	theUser.m_password = sPasswd;
	return bLogSuccess;
}
bool CLoginWnd::LoginNow()
{
	return true;
}
bool CLoginWnd::RegisterIn()
{
	CDuiString sEmail = m_pEditAcount->GetText();
	CDuiString sPasswd = m_pEditPassword->GetText();
	CDuiString sPasswd2 = static_cast<CEditUI*>(GetPaintMgr()->FindControl(registerView::kPasswd2))->GetText();
	bool bLogSuccess = true;
	if(sEmail.IsEmpty())
	{
		bLogSuccess = false;
	}
	if(sPasswd.IsEmpty())
	{
		bLogSuccess = false;
	}
	if(sPasswd2.IsEmpty())
	{
		bLogSuccess = false;
	}
	if(sPasswd!=sPasswd2)
	{
		bLogSuccess = false;
	}
	theUser.m_loadName = sEmail;
	theUser.m_password = sPasswd;
	return bLogSuccess;
}
void CLoginWnd::SetLoginTip(CDuiString sTip)
{
	static_cast<CTextUI*>(GetPaintMgr()->FindControl(logonView::kTip))->SetText(sTip);
}
void CLoginWnd::SetRegTip(CDuiString sTip)
{
	static_cast<CTextUI*>(GetPaintMgr()->FindControl(registerView::kTip))->SetText(sTip);
}
void CLoginWnd::SetTitle(CDuiString sTitle)
{
	CLabelUI* pTitle = static_cast<CLabelUI*>(GetPaintMgr()->FindControl(_T("caption")));
	if(pTitle)pTitle->SetText(sTitle);
}
LRESULT CLoginWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam,BOOL& bHandled)
{
	try
	{
		ModifyStyle(WS_CAPTION,WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
		if(!RegisterSkin(_T("login_register.xml"),NULL,this))
			return 0;

		GetPaintMgr()->AddNotifier(this);
 		RECT rcClient;
 		::GetClientRect(*this, &rcClient);
 		::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
 			rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);
	}
	catch (...)
	{
		throw _T("CLoginWnd::OnCreate");
	}

	return 0;
}

LRESULT CLoginWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if (uMsg == WM_KEYDOWN)
	{
		switch (wParam)
		{
		case VK_RETURN:
		case VK_ESCAPE:
			{
				if (wParam == VK_RETURN)
				{
					return FALSE;
				}
				else if (wParam == VK_ESCAPE)
				{
					Close();
					return TRUE;
				}
			}
		default:
			break;
		}
	}
	return FALSE;
}
