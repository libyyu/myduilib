#include "StdAfx.h"


#include "mainwindow.h"

#include "Database.h"
#include "ClientSocket.h"
#include "ListeningSocket.h"
#include "Msg.h"
//#include "DuiLib/Extend/Menu/UIMenu.h"
#include "config.h"
#include "log.h"
//#include "ColorPicker.h"

namespace sysbtn
{
	const TCHAR* const kMinBtn = _T("minbtn");
	const TCHAR* const kMaxBtn = _T("maxbtn");
	const TCHAR* const kCloseBtn = _T("closebtn");
	const TCHAR* const kRestoreBtn = _T("restorebtn");
};
namespace search
{
	const TCHAR* const kSearchEdit = _T("search_edit");
	const TCHAR* const kSearchBtn = _T("searchbtn");
};
namespace option
{
	const TCHAR* const kOnlineOpt = _T("onlineopt");
	const TCHAR* const kOfflineOpt = _T("offlineopt");
	const TCHAR* const kAllUserOpt = _T("alluseropt");
	const TCHAR* const kGroupChatOpt = _T("groupchatopt");
};
namespace tab
{
	const TCHAR* const kOnlineList = _T("online");
	const TCHAR* const kOfflineLIst = _T("offline");
	const TCHAR* const kAllUserList = _T("alluser");
	const TCHAR* const kTabLayout = _T("tabs");
};
namespace chatview
{
	const TCHAR* kLogoBtn = _T("logo");
	const TCHAR* kNameTxt = _T("nickname");
	const TCHAR* kDescEdt = _T("description");
	const TCHAR* kMinBtn = _T("minbtn");
	const TCHAR* kMaxBtn = _T("maxbtn");
	const TCHAR* kResBtn = _T("restorebtn");
	const TCHAR* kCloseBtn = _T("closebtn");

	const TCHAR* kHomeBtn = _T("home");
	const TCHAR* kHideBtn = _T("hide");
	const TCHAR* kViewEdt = _T("view_richedit");

	const TCHAR* kFontHor = _T("fontbar");
	const TCHAR* kFontTypeCom = _T("font_type");
	const TCHAR* kFontSizeCom = _T("font_size");
	const TCHAR* kBoldOpt = _T("boldbtn");
	const TCHAR* kItalicOpt = _T("italicbtn");
	const TCHAR* kUnderlineOpt = _T("underlinebtn");
	const TCHAR* kColorOpt = _T("colorbtn");

	const TCHAR* kFontOpt = _T("fontbtn");
	const TCHAR* kEmotionOpt = _T("emotionbtn");
	const TCHAR* kImageOpt = _T("imagebtn");
	const TCHAR* kShakeOpt = _T("shockbtn");
	const TCHAR* kScreenShortOpt = _T("screenshotsbtn");
	const TCHAR* kEmptyMsgViewOpt = _T("empty_richedit");

	const TCHAR* kInputEdt = _T("input_richedit");
	const TCHAR* kSendBtn = _T("sendbtn");
};
namespace userlistmenu
{
	const TCHAR* const kShowInfo = _T("info");
	const TCHAR* const kOfflineUser = _T("offline");
	const TCHAR* const kDelUser = _T("del");
	const TCHAR* const kRemoveAllUser = _T("clean");
};
enum EMT
{
	EMT_USERLIST = 1
};


#define DBFILE   _T("\\users.db")
#define WM_SERVICE_TRAY  (WM_USER + 1)
CMainWindow*  theApp = NULL;
const LPCTSTR CMainWindow::CLASSNAME = _T("Server");

CMainWindow::CMainWindow()
	: m_pSocketListen(NULL)
	, m_pMsg(NULL)
	, emotion_timer_start_(false)
	, text_color_(0xFF000000)
	, bold_(false)
	, italic_(false)
	, underline_(false)
	, font_size_(12)
	, font_face_name_(_T("΢���ź�"))
	, m_pInput(NULL)
	, m_pMsgView(NULL)
{
	bool b = BuildEnvironment();
	ASSERT(b);
	ZeroMemory(&_ui,sizeof(IControls));
}
CMainWindow::~CMainWindow()
{
	if(NULL!=theDB)
	{
		delete theDB;
		theDB = NULL;
	}
}
LPCTSTR CMainWindow::GetClass() const
{
	return _T("MainWindow");
}
bool CMainWindow::IsClass(LPCTSTR pstrClass)
{
	return (_tcscmp(pstrClass, _T("MainWindow")) == 0);
}
LPCTSTR CMainWindow::GetWindowClassName() const
{
	return CLASSNAME;
}
CControlUI* CMainWindow::CreateControl(LPCTSTR pstrClass)
{
	try
	{
		if(0 == _tcscmp(pstrClass,_T("UserList")))
			return new CUserListUI(*(GetPaintMgr()));
		return NULL;
	}
	catch (...)
	{
		throw _T("CMainWindow::CreateControl");
	}
}

CDuiString CMainWindow::GetSkinPath()
{
	return CPaintManagerUI::GetInstancePath();
}
CDuiString CMainWindow::GetSkinFolder()
{
	return _T("skin");
}
UILIB_RESOURCETYPE CMainWindow::GetResourceType() const
{
	return UILIB_ZIP;
}
CDuiString CMainWindow::GetZIPFileName() const
{
	return _T("Srv.dat");
}

void CMainWindow::Notify( TNotifyUI &msg )
{
	try
	{
		if ( 0 == _tcscmp(msg.sType,_T("windowinit")) )
			TypeInitWindow(msg);
		else if( 0 == _tcscmp(msg.sType,_T("click")))
			TypeClick(msg);
		else if( 0 == _tcscmp(msg.sType,_T("selectchanged")))
			TypeSelectedChange(msg);
		else if( 0 == _tcscmp(msg.sType,_T("menu")))
			OnMenu(msg);
		else if( _tcsicmp(msg.sType, _T("return")) == 0 ) 
		{
			if (_tcsicmp(msg.pSender->GetName(), chatview::kInputEdt) == 0)
			{
				OnSend(msg);
			}
		}
	}
	catch (...)
	{
		throw _T("CMainWindow::Notify");
	}
}
void CMainWindow::TypeClick(TNotifyUI& msg)
{
	if(msg.pSender->IsName(sysbtn::kCloseBtn))
	{
		if(IDOK==MessageBox(m_hWnd,_T("���ȷ����С������̨,����ֱ���˳�"),_T("��ʾ"),MB_OKCANCEL))
		{
			SendMessage(WM_SYSCOMMAND,SC_MINIMIZE);
			ShowWindow(SW_HIDE);//���������� 
		}
		else{
			FormatMsgToEditView(_T("(ϵͳ��Ϣ)"),mci::format_currenttime(),_T("�������˳�..."));
			OnExit(msg);
		}
	}
	else if (msg.pSender->IsName(sysbtn::kMaxBtn))
	{
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); return;
	}
	else if (msg.pSender->IsName(sysbtn::kRestoreBtn))
	{
		SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); return; 
	}
	else if (msg.pSender->IsName(sysbtn::kMinBtn))
	{
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); return;
	}
	else if(msg.pSender->IsName(chatview::kSendBtn))
	{
		OnSend(msg);
	}
	else if (msg.pSender->IsName(chatview::kEmptyMsgViewOpt))
	{
		m_lockEditView.Lock();
		int lSelBegin,lSelEnd;
		lSelEnd = lSelBegin = m_pMsgView->GetTextLength();
		m_pMsgView->SetSel(0, lSelEnd);//ѡ��ȫ��	
		m_pMsgView->ReplaceSel(_T(""), false);
		m_lockEditView.UnLock();
	}
	else if (msg.pSender->IsName(chatview::kShakeOpt))
	{
		FormatMsgToEditView(_T("(ϵͳ��Ϣ)"),mci::format_currenttime(),_T("�������û����Ͷ�����Ϣ"));
		CDuiString shake = BuildShakeMsg(_T(""));
		SendMsgToAll(shake);
		ShakeWindow(m_hWnd);		
	}
}
void CMainWindow::OnExit(TNotifyUI& msg)
{
	StopService();
	m_tray.Remove();
	if(theLog)
	{
		delete theLog;
		theLog = NULL;
	}
	if (m_pSocketListen)
	{
		delete m_pSocketListen;
		m_pSocketListen = NULL;
	}
	Sleep(10);
//	FadeInOut(2000,0x00010000|0x00080000);
	AnimateWindow(1000,0x00000010|0x00010000);
	::SendMessage(m_hWnd,WM_SYSCOMMAND,SC_CLOSE,0);
}
void CMainWindow::OnMenu(TNotifyUI& msg)
{
	if (msg.pSender!= _ui._pOfflineUserList &&
		msg.pSender != _ui._pOnlineUserList &&
		msg.pSender != _ui._pAllUserList)
	{
		return ;
	}
	POINT point = msg.ptMouse;
	
	CControlUI* pControl = NULL;
	CUserListUI* pUserList = NULL;
	if(_ui._pUserTab->GetCurSel()==0)
	{
		pUserList = _ui._pOnlineUserList;
	}
	else if(_ui._pUserTab->GetCurSel()==1)
	{
		pUserList = _ui._pOfflineUserList;
	}
	else if(_ui._pUserTab->GetCurSel()==2)
	{
		pUserList = _ui._pAllUserList;
	}
	CMenuWnd* pMenu = new CMenuWnd(m_hWnd);
	ClientToScreen(&point);
	pMenu->Init(NULL,_T("userlist_menu.xml"),0,point);
	pMenu->SetTag((UINT)EMT_USERLIST);
	const int nCurSelCnt = 0;// pUserList->GetSelCount();
	if (0 >= nCurSelCnt)
	{//û��ѡ���κ���
		LPCTSTR kItems[] =
		{
			userlistmenu::kShowInfo,
			userlistmenu::kOfflineUser,
			userlistmenu::kDelUser,
			userlistmenu::kRemoveAllUser
		};
		for(int i=0;i<sizeof(kItems)/sizeof(kItems[0]);++i)
			pMenu->EnableItem(kItems[i],FALSE);	
	}
// 	else if(pUserList->IsName(tab::kOnlineList))
// 	{
// 
// 	}
	else if (pUserList->IsName(tab::kOfflineLIst))
	{
		LPCTSTR kItems[] =
		{
			userlistmenu::kOfflineUser
		};
		for(int i=0;i<sizeof(kItems)/sizeof(kItems[0]);++i)
			pMenu->EnableItem(kItems[i],FALSE);	
	}
	else if (pUserList->IsName(tab::kAllUserList))
	{
		LPCTSTR kItems[] =
		{
			userlistmenu::kOfflineUser
		};
		for(int i=0;i<sizeof(kItems)/sizeof(kItems[0]);++i)
			pMenu->EnableItem(kItems[i],FALSE);	
	}
}
void CMainWindow::TypeInitWindow(TNotifyUI& msg)
{
	SetIcon(LoadIcon(IDI_MAIN_ICON));
	m_tray.Remove();
	m_tray.Setup(m_hWnd,IDI_MAIN_ICON,WM_SERVICE_TRAY,LoadIcon(IDI_MAIN_ICON),_T("������"));

	m_pMsg = new CMsg();
	InitAllControls();
	if (!InitService())
	{
		MessageBox(m_hWnd,_T("�޷������׽���"),_T("Error"),MB_OK);
		PostQuitMessage(0L);
	}
	CDuiString sIP;
	UINT uPort;
	m_pSocketListen->MyGetSockName(sIP,uPort);
	CDuiString sMsg = _T("State : �������Ѿ��ɹ�����\r\n");
	sMsg += _T("IP : ");
	sMsg += sIP;
	sMsg += _T("\r\n");
	CDuiString sPort;
	sPort.Format(_T("Port : %d"),uPort);
	sMsg += sPort;
	

	m_tray.ShowToolTip(_T("�������ɹ�����"),_T("Ϻ��--������"));

	TCHAR sTip[MAX_PATH]={0};
	_stprintf_s(sTip, MAX_PATH - 1, _T("%s"), sMsg.GetData());
	m_tray.SetTip(sTip);
	AnimateWindow(2000);
}
void CMainWindow::TypeSelectedChange(TNotifyUI& msg)
{
	if (msg.pSender->IsName(option::kGroupChatOpt))
	{
		_ui._pUserTab->SelectItem(3);		
	}
	else if (msg.pSender->IsName(option::kAllUserOpt))
	{
		_ui._pUserTab->SelectItem(2);		
	}
	else if (msg.pSender->IsName(option::kOnlineOpt))
	{
		_ui._pUserTab->SelectItem(0);
	}
	else if (msg.pSender->IsName(option::kOfflineOpt))
	{
		_ui._pUserTab->SelectItem(1);
	}
	else if (msg.pSender->IsName(chatview::kFontOpt))
	{
		CControlUI* pUI = NULL;
		pUI = GetPaintMgr()->FindControl(chatview::kFontHor);
		if(pUI) pUI->SetVisible(static_cast<COptionUI*>(msg.pSender)->IsSelected());
	}
	else if (msg.pSender->IsName(chatview::kColorOpt))
	{
		static CHOOSECOLOR cc;
		static COLORREF crCustCoLors[16];
		cc.lStructSize=sizeof(CHOOSECOLOR);
		cc.hwndOwner= this->m_hWnd;
		cc.hInstance=NULL;
		cc.rgbResult = RGB(0,0,0);
		cc.lpCustColors=crCustCoLors;
		cc.Flags=CC_RGBINIT|CC_FULLOPEN | CC_ANYCOLOR; 
		cc.lCustData=0;
		cc.lpfnHook=NULL;
		cc.lpTemplateName=NULL;
		if(TRUE == ChooseColor(&cc))
		{ 
			TCHAR szBuf[MAX_PATH] = {0};
#if defined(UNDER_CE)
			_stprintf(szBuf, _T("FF%02X%02X%02X"), GetRValue(cc.rgbResult), GetGValue(cc.rgbResult), GetBValue(cc.rgbResult));
#else
			_stprintf_s(szBuf, MAX_PATH - 1, _T("FF%02X%02X%02X"), GetRValue(cc.rgbResult), GetGValue(cc.rgbResult), GetBValue(cc.rgbResult));
#endif
			LPTSTR pstr = NULL;
			text_color_ = _tcstoul(szBuf, &pstr, 16);
		}
	}
}

LRESULT CMainWindow::ProcessWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(WM_MENUCOMMAND == uMsg) OnMenuCommand(uMsg,wParam,lParam,bHandled);
	if(WM_SERVICE_TRAY==uMsg) OnTray(uMsg,wParam,lParam,bHandled);
	return 0;
}
LRESULT CMainWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	try
	{
		ModifyStyle(WS_CAPTION,WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
		if(!RegisterSkin(_T("mainwindow.xml"),0,this))
			return 0;

		GetPaintMgr()->AddNotifier(this);
		RECT rcClient;
		::GetClientRect(*this, &rcClient);
		::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
			rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);
	}
	catch (...)
	{
		throw _T("CMainWindow::OnCreate");
	}
	return 0;
}
LRESULT CMainWindow::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// ��ʱ�����յ�WM_NCDESTROY���յ�wParamΪSC_CLOSE��WM_SYSCOMMAND
	if( wParam == SC_CLOSE ) {
		Sleep(1000);
		::PostQuitMessage(0L);
		bHandled = TRUE;
		return 0;
	}
	BOOL bZoomed = ::IsZoomed(*this);
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	if( ::IsZoomed(*this) != bZoomed ) {
		if( !bZoomed ) {
			CControlUI* pControl = static_cast<CControlUI*>(GetPaintMgr()->FindControl(sysbtn::kMaxBtn));
			if( pControl ) pControl->SetVisible(false);
			pControl = static_cast<CControlUI*>(GetPaintMgr()->FindControl(sysbtn::kRestoreBtn));
			if( pControl ) pControl->SetVisible(true);
		}
		else {
			CControlUI* pControl = static_cast<CControlUI*>(GetPaintMgr()->FindControl(sysbtn::kMaxBtn));
			if( pControl ) pControl->SetVisible(true);
			pControl = static_cast<CControlUI*>(GetPaintMgr()->FindControl(sysbtn::kRestoreBtn));
			if( pControl ) pControl->SetVisible(false);
		}
	}
	return lRes;
}
LRESULT CMainWindow::OnMenuCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MenuCmdMsg* pMsg = (MenuCmdMsg*)wParam;
	if (pMsg->nMenuTag==EMT_USERLIST)
	{
		CUserListUI* pUserList = NULL;
		if(_ui._pUserTab->GetCurSel()==0)
			pUserList = _ui._pOnlineUserList;
		else if(_ui._pUserTab->GetCurSel()==1)
			pUserList = _ui._pOfflineUserList;
		else if(_ui._pUserTab->GetCurSel()==2)
			pUserList = _ui._pAllUserList;
		if ( 0 == pMsg->sItemName.Compare(userlistmenu::kShowInfo))
		{

		}
		else if ( 0 == pMsg->sItemName.Compare(userlistmenu::kOfflineUser))
		{//ǿ���û�����
			if(pUserList)
			{
				int iIndex = pUserList->GetCurSel();
				User user = pUserList->GetUser(iIndex);
				CClientSocket* pClient = GetClient(user);
				CDuiString a = BuildForceOfflineUser();
				SendMsg(a,pClient);
				ChangeUserState(pClient,user.m_loadName,OFFLINE);
			}
			OnUpdateUserList();
		}
		else if ( 0 == pMsg->sItemName.Compare(userlistmenu::kDelUser))
		{	
			int iCurSel = _ui._pUserTab->GetCurSel();
			int iIndex = pUserList->GetCurSel();
			User user = pUserList->GetUser(iIndex);
			if (user.m_userState.Compare(ONLINE)==0)
			{
				if (iCurSel==0)
				{
					CClientSocket* pClient = GetClient(user);
					if(pClient)
					{
						CDuiString s = BuildDelUser();
						SendMsg(s,pClient);

						DeleteUser(user);
						DeleteSocket(pClient);

						_ui._pOnlineUserList->RemoveAt(iIndex);

						for (int i=0;i<_ui._pAllUserList->GetCount();++i)
						{
							User user2 = _ui._pAllUserList->GetUser(i);
							if (user2.m_loadName==user.m_loadName)
							{
								_ui._pAllUserList->RemoveAt(i);
								break;
							}
						}
					}
				}
				else
				{
					CClientSocket* pClient = GetClient(user);
					if(pClient)
					{
						CDuiString s = BuildDelUser();
						SendMsg(s,pClient);

						DeleteUser(user);
						DeleteSocket(pClient);

						_ui._pAllUserList->RemoveAt(iIndex);

						for (int i=0;i<_ui._pOnlineUserList->GetCount();++i)
						{
							User user2 = _ui._pOnlineUserList->GetUser(i);
							if (user2.m_loadName==user.m_loadName)
							{
								_ui._pOnlineUserList->RemoveAt(i);
								break;
							}
						}
					}
				}
			}
			else
			{
				if (iCurSel==1)
				{
					DeleteUser(user);
					_ui._pOfflineUserList->RemoveAt(iIndex);
					for (int i=0;i<_ui._pAllUserList->GetCount();++i)
					{
						User user2 = _ui._pAllUserList->GetUser(i);
						if (user2.m_loadName==user.m_loadName)
						{
							_ui._pAllUserList->RemoveAt(i);
							break;
						}
					}
				}
				else
				{
					DeleteUser(user);
					_ui._pAllUserList->RemoveAt(iIndex);
					for (int i=0;i<_ui._pOfflineUserList->GetCount();++i)
					{
						User user2 = _ui._pOfflineUserList->GetUser(i);
						if (user2.m_loadName==user.m_loadName)
						{
							_ui._pOfflineUserList->RemoveAt(i);
							break;
						}
					}
				}
			}
			OnUpdateUserList();
		}
		else if ( 0 == pMsg->sItemName.Compare(userlistmenu::kRemoveAllUser))
		{
				
		}
	}
	return 0;
}
LRESULT CMainWindow::OnTray(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(wParam!=IDI_MAIN_ICON)
		return 1;
	switch (lParam)
	{
	case WM_LBUTTONDBLCLK:
		{
			this->ShowWindow(true);// 
			SetForegroundWindow(this->m_hWnd);
		}
		break;
	}
	return 0;
}
CClientSocket* CMainWindow::GetClient(User user)
{
	m_lockConnectionList.Lock();
	POSITION pos;
	CClientSocket* pSocket = NULL;
	for (pos=m_connectionList.GetHeadPosition();pos!=NULL;)
	{
		pSocket = (CClientSocket*)m_connectionList.GetNext(pos);
		if (pSocket)
		{
			SOCKADDR_IN   sa; 
			int  len  = sizeof(sa);
			pSocket->GetPeerName((SOCKADDR*)&sa,&len);
			CDuiString sIP,sPort;
			sIP.Format(_T("%d.%d.%d.%d"),
				sa.sin_addr.S_un.S_un_b.s_b1,
				sa.sin_addr.S_un.S_un_b.s_b2,
				sa.sin_addr.S_un.S_un_b.s_b3,
				sa.sin_addr.S_un.S_un_b.s_b4);
			sPort.Format(_T("%d"),sa.sin_port);
			if (user.m_userIp==sIP&&user.m_userPort==sPort)
			{
				return pSocket;
			}
		}
	}
	m_lockConnectionList.UnLock();
	return pSocket;
}
#define MAX_RCV_BUFLEN 1024*2
#define MAX_SND_BUFLEN 1024
void CMainWindow::OnSend(TNotifyUI& msg)
{
	if( m_pInput == NULL ) return;
	m_pInput->SetFocus();
	CDuiString sText = m_pInput->GetTextRange(0, m_pInput->GetTextLength());
	if( sText.IsEmpty() ) return;
	m_pInput->SetText(_T(""));

	if( m_pMsgView == NULL ) return;

	CDuiString sTime = mci::format_currenttime();

	FormatMsgToEditView(_T("(ϵͳ��Ϣ)"),sTime,sText,RGB(255,0,0));

	SendMsg(sText);
}
void CMainWindow::SendMsg(CDuiString& strText)
{
	//���޸ģ�2013-6-24 18:41
	std::string ss = TO_STDSTRING(strText);
	const char* snd = ss.c_str();
	int iTotal = strlen(snd);
	int iPart = 0;
	while (iTotal>MAX_SND_BUFLEN)
	{
		char buf[MAX_SND_BUFLEN]={0};
		memcpy(buf,snd+iPart,MAX_SND_BUFLEN);
		iPart += MAX_SND_BUFLEN;
		iTotal -= iPart;

		CDuiString sPart = TO_DUISTRING(buf);
		DUI__Trace(sPart);
		CDuiString sMsg = BuildGroupMsg(sPart);
		DUI__Trace(sMsg);
		SendMsgToAll(sMsg);
		Sleep(10);
	}
	if(iTotal>0)
	{
		char buf[MAX_SND_BUFLEN]={0};
		memcpy(buf,snd+iPart,iTotal);
		CDuiString sPart = TO_DUISTRING(buf);
		DUI__Trace(sPart);
		CDuiString sMsg = BuildGroupMsg(sPart);
		DUI__Trace(sMsg);
		SendMsgToAll(sMsg);
	}
}
//<MSGTYPE TYPE="CHATMSG">
//  <MSG who="name" towho="name" text="" time="" />
//</MSGTYPE>
CDuiString CMainWindow::BuildGroupMsg(CDuiString msg)
{
	CDuiString a = _T("<MSGTYPE TYPE=\"CHATMSG\">");
	CDuiString b = _T("<MSG ");
	b += _T("who=\"\" ");

	b += _T("towho=\"\"");

	b += _T("text=\"");
	b += msg + _T("\"  ");

	b += _T("time=\"");
	b += mci::format_currenttime() + _T("\"  ");
	b += _T("/>");

	a += b;
	a += _T("</MSGTYPE>");

	return a;
}
void CMainWindow::FormatMsgToEditView(CDuiString sName,CDuiString sTime,CDuiString sMsg,DWORD dwColor)
{
	long lSelBegin = 0, lSelEnd = 0;
	CHARFORMAT2 cf;
	ZeroMemory(&cf, sizeof(CHARFORMAT2));
	cf.cbSize = sizeof(cf);
	cf.dwReserved = 0;
	cf.dwMask = CFM_COLOR  | CFM_UNDERLINE | CFM_UNDERLINETYPE | CFM_BOLD;
	cf.dwEffects = CFE_LINK ;
	cf.bUnderlineType = CFU_UNDERLINE;
	cf.crTextColor = dwColor;
//	cf.cbSize = 20;

	m_lockEditView.Lock();

	lSelEnd = lSelBegin = m_pMsgView->GetTextLength();
	m_pMsgView->SetSel(lSelEnd, lSelEnd);//�Ƶ�ĩβ
	sName = CDuiString(_T("[ "))+sName;
	sName += _T(" ");
	sName += sTime;
	sName += _T(" ]\n");
	m_pMsgView->ReplaceSel(sName, false);

	lSelEnd = m_pMsgView->GetTextLength();
	m_pMsgView->SetSel(lSelBegin, lSelEnd);
	m_pMsgView->SetSelectionCharFormat(cf);

	cf.dwMask = CFM_COLOR;
	cf.crTextColor = RGB(0, 0, 0);
	cf.dwEffects = 0;

	PARAFORMAT2 pf;
	ZeroMemory(&pf, sizeof(PARAFORMAT2));
	pf.cbSize = sizeof(pf);
	pf.dwMask = PFM_STARTINDENT;
	pf.dxStartIndent = 0;
	m_pMsgView->SetParaFormat(pf);

	lSelEnd = lSelBegin = m_pMsgView->GetTextLength();

	m_pMsgView->SetSel(-1, -1);
	m_pMsgView->ReplaceSel(sMsg, false);

	m_pMsgView->SetSel(-1, -1);
	m_pMsgView->ReplaceSel(_T("\n"), false);

//	cf.cbSize = 16;
	cf.crTextColor = RGB(0, 0, 0);
	lSelEnd = m_pMsgView->GetTextLength();
	m_pMsgView->SetSel(lSelBegin, lSelEnd);
	m_pMsgView->SetSelectionCharFormat(cf);

	ZeroMemory(&pf, sizeof(PARAFORMAT2));
	pf.cbSize = sizeof(pf);
	pf.dwMask = PFM_STARTINDENT;
	pf.dxStartIndent = 220;
	m_pMsgView->SetParaFormat(pf);

	m_pMsgView->NeedUpdate();
	m_pMsgView->EndDown();

	m_lockEditView.UnLock();
}
bool CMainWindow::BuildEnvironment()
{
	CDuiString sPath = CPaintManagerUI::GetInstancePath();
	bool ret = true;
	sPath += +_T("\\");
	sPath += CLASSNAME;
	ret = mci::make_dir(sPath.GetData());
	ASSERT(ret);

	CDuiString sLogPath = sPath + _T("\\log");
	ret = mci::make_dir(sLogPath.GetData());
	ASSERT(ret);
	CDuiString sLogDir = sLogPath + _T("\\");
	TCHAR sDir[MAX_PATH]={0};
	_stprintf_s(sDir, MAX_PATH - 1, _T("%s"), sLogDir.GetData());
	theLog = new CLog(_T("service_log.log"),sDir);
	if(theLog==NULL)
		ASSERT(FALSE&&"�޷�������־����");

	CDuiString sConfig = sPath + _T("\\config");
	ret = mci::make_dir(sConfig.GetData());
	ASSERT(ret);

	CDuiString sDBPath = sPath + _T("\\db");
	ret = mci::make_dir(sDBPath.GetData());
	ASSERT(ret);

	CDuiString sDBFile = sDBPath + DBFILE;
	theDB = new CUserMgr();
	ASSERT(theDB);
	ret = theDB->OpenDB(sDBFile.GetData());
	ASSERT(ret);

	sLog(_T("���û�����Ϣ..."));
	return TRUE;
}
void CMainWindow::InitAllControls()
{
	sLog(_T("�󶨿ؼ�..."));
	FindControl(_ui._pMinBtn,sysbtn::kMinBtn);
	FindControl(_ui._pMaxBtn,sysbtn::kMaxBtn);
	FindControl(_ui._pResBtn,sysbtn::kRestoreBtn);
	FindControl(_ui._pCloseBtn,sysbtn::kCloseBtn);
	ASSERT(_ui._pMinBtn);ASSERT(_ui._pMaxBtn);
	ASSERT(_ui._pResBtn);ASSERT(_ui._pCloseBtn);

	FindControl(_ui._pSearchRichEdit,search::kSearchEdit);
	FindControl(_ui._pSearchBtn,search::kSearchBtn);
	ASSERT(_ui._pSearchRichEdit);ASSERT(_ui._pSearchBtn);

	FindControl(_ui._pOnlineOpt,option::kOnlineOpt);
	FindControl(_ui._pOfflineOpt,option::kOfflineOpt);
	FindControl(_ui._pAllUerOpt,option::kAllUserOpt);
	ASSERT(_ui._pOnlineOpt);ASSERT(_ui._pOfflineOpt);
	ASSERT(_ui._pAllUerOpt);

	FindControl(_ui._pOnlineUserList,tab::kOnlineList);
	FindControl(_ui._pOfflineUserList,tab::kOfflineLIst);
	FindControl(_ui._pAllUserList,tab::kAllUserList);
	FindControl(_ui._pUserTab,tab::kTabLayout);
	ASSERT(_ui._pOnlineUserList);ASSERT(_ui._pOfflineUserList);
	ASSERT(_ui._pAllUserList);ASSERT(_ui._pUserTab);

	FindControl(m_pMsgView,chatview::kViewEdt);
	FindControl(m_pInput,chatview::kInputEdt);
	ASSERT(m_pInput); ASSERT(m_pMsgView);
	
}
bool CMainWindow::InitService()
{
	if(m_pSocketListen)
		delete m_pSocketListen;
	m_pSocketListen = NULL;
	m_pSocketListen = new CListeningSocket(this);
	if(m_pSocketListen==NULL)
	{
		sLogError(_T("�����׽���ʧ��..."));
		MessageBox(m_hWnd,_T("�����׽���ʧ��"),_T("Error"),MB_OK);
		return false;
	}
	ASSERT(m_pSocketListen);
	CDuiString sPath = CPaintManagerUI::GetInstancePath();
	sPath += CLASSNAME;
	sPath += _T("\\config\\config.ini");
	CDuiString sIP = Config::ReadIP(sPath);
	CDuiString sPort = Config::ReadPort(sPath);
	Config::WriteIP(sIP,sPath);
	Config::WritePort(sPort,sPath);
	CDuiString ssIP;
	if(isalpha(sIP.GetAt(0)))
	{//������
		LPHOSTENT     lphostent; 
		lphostent = ::gethostbyname(TO_STDSTRING(sIP).c_str());
		if(lphostent)
		{
			///����
			struct in_addr ip_addr;
			memcpy(&ip_addr,lphostent->h_addr_list[0],4);///h_addr_list[0]��4���ֽ�,ÿ���ֽ�8λ
			ssIP.Format(_T("%d.%d.%d.%d"),
				ip_addr.S_un.S_un_b.s_b1,
				ip_addr.S_un.S_un_b.s_b2,
				ip_addr.S_un.S_un_b.s_b3,
				ip_addr.S_un.S_un_b.s_b4);
		}
		else ssIP = _T("127.0.0.1");
	}
//	if(m_pSocketListen->Create(mci::ToUInt(sPort),1,sIP))
	//���޸� 2013-6-26 8:13
	if(m_pSocketListen->Create(mci::ToUInt(sPort),SOCK_STREAM))
	{
		sLog(_T("�ɹ������׽���..."));
		if(!m_pSocketListen->Listen())
		{
			sLogError(_T("���ü���ʧ��..."));
			MessageBox(m_hWnd,_T("���ü���ʧ��"),_T("Error"),MB_OK);
			return false;
		}
		CDuiString sInfo = _T("�ɹ���IP:");
		sInfo += sIP;
		sInfo += _T(",Port:");
		sInfo += sPort;
		sInfo += _T("���м���");
		sLog(_T("%s"), sInfo.GetData());

		FormatMsgToEditView(_T("(ϵͳ��Ϣ)"),mci::format_currenttime(),CDuiString(_T("�ɹ�����:")) + sIP + _T(":") + sPort);
	}
	else
	{
		sLogError(_T("�����׽���ʧ��..."));
		int iErrCode = WSAGetLastError();
		MessageBox(m_hWnd,_T("�����׽���ʧ��"),_T("Error"),MB_OK);
		return false;
	}
	int len = sizeof(SOCKADDR);
	if(m_pSocketListen->MyGetPeerName(m_strAddress,m_uPort))
	{
		sLogError(_T("��ȡ��������ַ����..."));
		MessageBox(m_hWnd,_T("��ȡ��������ַ����"),_T("Error"),MB_OK);
		return false;
	}
	LoadAllUserFromDB();
	OnUpdateUserList();
	return true;
}
bool CMainWindow::StopService()
{
	CDuiString sStop = BuildStopService();
	SendMsgToAll(sStop);
	return m_pSocketListen->ShutDown();
	sLog(_T("�������ر�..."));
}
void CMainWindow::LoadAllUserFromDB()
{
	std::vector<User> UserList;
	theDB->GetAllUsersToList(UserList);
	m_lockUserinfoList.Lock();
	for (size_t ii=0;ii<UserList.size();++ii)
	{
		User* user = new User;
		user->m_loadName = UserList[ii].m_loadName;
		user->m_password = UserList[ii].m_password;
		m_userInforList.AddTail(user);
	}
	m_lockUserinfoList.UnLock();
	sLog(_T("�����ݿ�����û�..."));
}
void CMainWindow::EmptyUserList()
{
	m_lockUserinfoList.Lock();
	POSITION pos,temp;
	for (pos=m_userInforList.GetHeadPosition();NULL!=pos;)
	{
		temp = pos;
		User* user = (User*)m_userInforList.GetNext(pos);
		m_userInforList.RemoveAt(temp);
		delete user;
		user = NULL;
	}
	m_lockUserinfoList.UnLock();
	sLog(_T("����û��б�..."));
}
void CMainWindow::OnAccept()
{
	CClientSocket* pSocket=new CClientSocket(this);
	if(m_pSocketListen->Accept(*pSocket))
	{
		SOCKADDR_IN   sa; 
		int  len  = sizeof(sa);
		pSocket->GetSockName((SOCKADDR*)&sa,&len);
		CDuiString sInfo;
		sInfo.Format(_T("Accept:(%d.%d.%d.%d:%d)..."),
			sa.sin_addr.S_un.S_un_b.s_b1,
			sa.sin_addr.S_un.S_un_b.s_b2,
			sa.sin_addr.S_un.S_un_b.s_b3,
			sa.sin_addr.S_un.S_un_b.s_b4,
			sa.sin_port);
		sLog(_T("%s"), sInfo);
		pSocket->Initialize();
		m_lockConnectionList.Lock();
		m_connectionList.AddTail(pSocket);
		m_lockConnectionList.UnLock();

//		OnUpdateUserList();
	}
	else
	{
		delete pSocket;
	}
}
//���½����û��б���Ϣ
void CMainWindow::OnUpdateUserList()
{
	m_lockUserinfoList.Lock();
	_ui._pAllUserList->RemoveAll();
	_ui._pOnlineUserList->RemoveAll();
	_ui._pOfflineUserList->RemoveAll();
	POSITION pos;
	int iAllCount = 0,iOnLine = 0;
	for (pos = m_userInforList.GetHeadPosition(); NULL != pos;)
	{
		User* ppUser = (User*)m_userInforList.GetNext(pos);
		iAllCount ++;
		if (ppUser)
		{
			_ui._pAllUserList->AddNode(*ppUser);
			if(ppUser->m_userState.Compare(ONLINE) ==0 ) 
			{ _ui._pOnlineUserList->AddNode(*ppUser);iOnLine ++; } 
			else
			{ _ui._pOfflineUserList->AddNode(*ppUser); }
		}
	}
	m_lockUserinfoList.UnLock();
	sLog(_T("�����û��б�..."));
}
CDuiString CMainWindow::AddTimeMsg(CDuiString& text)
{
	CTime a = CTime::GetCurrentTime();
	CDuiString b = a.Format(_T("(%H:%M:%S)"));
	b = b + text;
	return b;
}
void CMainWindow::OnReceive(CClientSocket* pSocket)
{
	char *pBuf = new char[MAX_RCV_BUFLEN+1];
	int nBufSize = MAX_RCV_BUFLEN;
	int nReceived;
	CDuiString strReceived;
	nReceived=pSocket->Receive(pBuf,nBufSize);
	if(nReceived!=SOCKET_ERROR)
	{
		pBuf[nReceived] = 0;
		strReceived = TO_DUISTRING(pBuf);
		m_lockMsg.Lock();
		m_pMsg->m_strText = strReceived;
//		DUI__Trace(strReceived);
		CDuiString preType = m_pMsg->WhatType();


		if(preType==MSGTYPE::NOMALMSG)
		{
			m_pMsg->BuildNomal();
			CDuiString normalMsg = this->BuildChatMsg(m_pMsg);
			//normalMsg=AddTimeMsg(normalMsg);
			SendMsgToAll(normalMsg);
// 			normalMsg = m_pMsg->m_nomalMsg.who + _T("  ��  ") + m_pMsg->m_nomalMsg.toWho+_T("  ˵ :") + m_pMsg->m_nomalMsg.msgItem;
// 			normalMsg = AddTimeMsg(normalMsg);
//			m_listMsg.AddString(anormalMsg);//����Ϣ������ʾ��Ϣ
			sLog(_T("Receive:�û�[%s]����Ⱥ��Ϣ..."), m_pMsg->m_nomalMsg.who);
		}

		else if(preType==MSGTYPE::SYSTEMMSG)
		{
			m_pMsg->BuileSystem();
			if (m_pMsg->m_systemMsg.type==MSGTYPE::REGI)
			{//ע��
				CDuiString sf = this->IsUserRegisted(m_pMsg->m_systemMsg.name,m_pMsg->m_systemMsg.msgItem);
				SendMsg(sf,pSocket);
				sLog(_T("Receive:�û�[%s]ע����Ϣ..."), m_pMsg->m_systemMsg.name);
				FormatMsgToEditView(_T("(ϵͳ��Ϣ)"),m_pMsg->m_systemMsg.sndTime,CDuiString(_T("�û�:"))+m_pMsg->m_systemMsg.name+_T(" ע��..."));
			}
			else if (m_pMsg->m_systemMsg.type==_T("SHAKE"))
			{//��
				CDuiString sf = BuildShakeMsg(m_pMsg->m_systemMsg.name);
				SendMsgToAll(sf);
				sLog(_T("Receive:�û�[%s]���Ͷ�����Ϣ..."), m_pMsg->m_systemMsg.name);
				FormatMsgToEditView(_T("(ϵͳ��Ϣ)"),m_pMsg->m_systemMsg.sndTime,CDuiString(_T("�û�:"))+m_pMsg->m_systemMsg.name+_T(" ���Ͷ�����Ϣ..."));
			}
			else if(m_pMsg->m_systemMsg.type==MSGTYPE::LOAD)
			{//��¼
				CDuiString sl;
				if(!IsUserLoged(m_pMsg->m_systemMsg.name,sl))
				{
					CDuiString sf;
					bool ret = this->GetUserItem(m_pMsg->m_systemMsg.name,m_pMsg->m_systemMsg.msgItem,sf);
					if(ret)
					{
						SendMsg(sf,pSocket);
						Sleep(500);
						CDuiString callNameStr = FindCallName(m_pMsg->m_systemMsg.name);
						CDuiString a = BuildSysMsg(m_pMsg,_T("LOAD"),ONLINE);

						ChangeUserState(pSocket,m_pMsg->m_systemMsg.name);//�����û�״̬
						OnUpdateUserList();//�����б�
						SendMsgToAll(a);
						Sleep(500);
						CDuiString userList = BuildUserListMsg();//�����û��б�
						SendMsgToAll(userList);//�������û������б�

						sLog(_T("Receive:�û�[%s]��¼�ɹ���Ϣ..."), m_pMsg->m_systemMsg.name);
						FormatMsgToEditView(_T("(ϵͳ��Ϣ)"),m_pMsg->m_systemMsg.sndTime,CDuiString(_T("�û�:"))+m_pMsg->m_systemMsg.name+_T(" ��¼Ϻ��..."));
					}
					else
					{
						SendMsg(sf,pSocket);
	//					DeleteSocket(pSocket);
						sLog(_T("Receive:�û�[%s]��¼ʧ����Ϣ..."), m_pMsg->m_systemMsg.name);
					}
				}
				else
				{
					SendMsg(sl,pSocket);
					sLog(_T("Receive:�û�[%s]�ص�¼��Ϣ..."), m_pMsg->m_systemMsg.name);
				}
			}
			else if(m_pMsg->m_systemMsg.type==MSGTYPE::CLOSE)
			{//�ͻ����˳�
//				CDuiString a = _T("ϵͳ��Ϣ(CL)") + m_pMsg->m_systemMsg.name + _T("  �˳���������!");
				sLog(_T("Receive:�û�[%s]�˳���Ϣ..."), m_pMsg->m_systemMsg.name);
				FormatMsgToEditView(_T("(ϵͳ��Ϣ)"),m_pMsg->m_systemMsg.sndTime,CDuiString(_T("�û�:"))+m_pMsg->m_systemMsg.name+_T(" �˳�Ϻ��..."));

				ChangeUserState(pSocket,m_pMsg->m_systemMsg.name,OFFLINE);

//				DeleteSocket(pSocket);

				OnUpdateUserList();//�����б�

// 				a = AddTimeMsg(a);
// 				SendMsgToAll(a);

				CDuiString userList = BuildUserListMsg();
				SendMsgToAll(userList);//�������¿ͻ����б���Ϣ
				
			}
			else if(m_pMsg->m_systemMsg.type==MSGTYPE::CHANGE)
			{

// 				CDuiString a = _T("ϵͳ��Ϣ(CH)") + m_pMsg->m_systemMsg.name + _T("  ����Ϊ  ") + m_pMsg->m_systemMsg.msgItem;
// 				a = AddTimeMsg(a);
// 				SendMsgToAll(a);
				//�������½�����Ϣ

				OnUpdateUserList();//�����б�

				Sleep(1000);

				CDuiString userList = BuildUserListMsg();
				SendMsgToAll(userList);//�������¿ͻ����б���Ϣ
				sLog(_T("Receive:�û�[%s]������Ϣ..."), m_pMsg->m_systemMsg.name);
				FormatMsgToEditView(_T("(ϵͳ��Ϣ)"),m_pMsg->m_systemMsg.sndTime,CDuiString(_T("�û�:"))+m_pMsg->m_systemMsg.name+_T(" ����Ϊ��")+m_pMsg->m_systemMsg.msgItem);

			}
			else
			{
				MessageBox(NULL,_T("��Ϣ����!"),_T("Error"),MB_ICONERROR|MB_OK);
			}
		}
		m_lockMsg.UnLock();
	}
	else
	{
		MessageBox(NULL,_T("��Ϣ���մ���!"),_T("Error"),MB_OK|MB_ICONERROR);
	}
	delete pBuf;
}
void CMainWindow::SendMsg(CDuiString& strText,CClientSocket* pSocket)
{
	int nLen;
	int nSent;
	nLen = strText.GetLength();
	std::string ss = TO_STDSTRING(strText);
	const char* snd = ss.c_str();
	nSent = pSocket->Send(snd,strlen(snd));
	if(nSent==SOCKET_ERROR)
	{
		sLogError(_T("������Ϣ����..."));
//		MessageBox(m_hWnd,_T("��Ϣ���ʹ���!"),_T("Error"),MB_OK|MB_ICONERROR);
	}
}
void CMainWindow::SendMsgToAll(CDuiString& strText)
{
	m_lockConnectionList.Lock();
	//���������ÿ���ڵ�
	POSITION pos;
	for (pos = m_connectionList.GetHeadPosition(); NULL != pos;)
	{
		CClientSocket *pSocket = (CClientSocket*)m_connectionList.GetNext(pos);
		if (NULL != pSocket)
		{
			SendMsg(strText,pSocket);
		}
	}
	m_lockConnectionList.UnLock();
}
//<MSGTYPE TYPE="SERVERDOWN">
//  <MSG who="" result="" time="" />
//</MSGTYPE>
CDuiString CMainWindow::BuildStopService()
{
	CDuiString a = _T("<MSGTYPE TYPE=\"SERVERDOWN\">");
	a += _T("<MSG who=\"\" result=\"\" time=\"\" />");
	a += _T("</MSGTYPE>");
	return a;
}
//<MSGTYPE TYPE="FORCEOFFLINE">
//  <MSG who="" result="" time="" />
//</MSGTYPE>
CDuiString CMainWindow::BuildForceOfflineUser()
{
	CDuiString a = _T("<MSGTYPE TYPE=\"FORCEOFFLINE\">");
	a += _T("<MSG who=\"\" result=\"\" time=\"\" />");
	a += _T("</MSGTYPE>");
	return a;
}
//<MSGTYPE TYPE="FORCEDELETE">
//  <MSG who="" result="" time="" />
//</MSGTYPE>
CDuiString CMainWindow::BuildDelUser()
{
	CDuiString a = _T("<MSGTYPE TYPE=\"FORCEDELETE\">");
	a += _T("<MSG who=\"\" result=\"\" time=\"\" />");
	a += _T("</MSGTYPE>");
	return a;
}
//<MSGTYPE TYPE="SHAKE">
//  <MSG who="" result="" time="" />
//</MSGTYPE>
CDuiString CMainWindow::BuildShakeMsg(CDuiString sWho)
{
	CDuiString a = _T("<MSGTYPE TYPE=\"SHAKE\">");
	a += _T("<MSG who=\"");
	a += sWho;
	a += _T("\" result=\"\" ");
	a += _T("time=\"");
	a += mci::format_currenttime();
	a += _T("\" />");
	a += _T("</MSGTYPE>");
	return a;
}
//<MSGTYPE TYPE="LOAD">
//  <MSG who="name" result="success" time="" />
//</MSGTYPE>
bool CMainWindow::GetUserItem(CDuiString& name,CDuiString& password,CDuiString& sf)
{
	sf = _T("");
	POSITION pos;	
	bool ret = false;
	m_lockUserinfoList.Lock();

	for (pos = m_userInforList.GetHeadPosition(); NULL != pos;)
	{
		User *pUser = (User*)m_userInforList.GetNext(pos);
		if (pUser!=NULL&&pUser->m_loadName==name&&pUser->m_password==password)
		{
			sf  = _T("<MSGTYPE TYPE=\"LOAD\">");
			CDuiString b = _T("<MSG ");
			b += _T("who=\"");
			b += name + _T("\"  ");
			b += _T("result=\"");
			b += _T("success");
			b += _T("\"  ");
			b += _T("time=\"");
			b += mci::format_currenttime() + _T("\"  ");
			b += _T("/>");

			sf += b + _T("</MSGTYPE>");
			ret = true;
			break;
		}
		else
		{
			sf  = _T("<MSGTYPE TYPE=\"LOAD\">");
			CDuiString b = _T("<MSG ");
			b += _T("who=\"");
			b += name + _T("\"  ");
			b += _T("result=\"");
			b += _T("fail");
			b += _T("\"  ");
			b += _T("time=\"");
			b += mci::format_currenttime() + _T("\"  ");
			b += _T("/>");

			sf += b + _T("</MSGTYPE>");
			ret = false;				
		}
	}

	m_lockUserinfoList.UnLock();

	return ret;
}
CDuiString CMainWindow::FindCallName(CDuiString& loadName)
{
	m_lockUserinfoList.Lock();
	POSITION pos;
	for(int i=0;i<m_userInforList.GetCount();i++)
	{
		pos = m_userInforList.FindIndex(i);
		User* ppUser=(User*)m_userInforList.GetPrev(pos);
		if(ppUser && ppUser->m_loadName==loadName)
		{
			return ppUser->m_callName;
		}

	}
	m_lockUserinfoList.UnLock();
	return _T("");
}
//<MSGTYPE TYPE="USERLIST">
//  <USER loadname="loadname" callname="callname" ip="ip" port="port" state="online" />
//  <USER loadname="loadname" callname="callname" ip="ip" port="port" state="online" />
//</MSGTYPE>
CDuiString CMainWindow::BuildUserListMsg()
{
	CDuiString a = _T("<MSGTYPE TYPE=\"USERLIST\">");

	CDuiString b = _T("");
	POSITION pos;	
	m_lockUserinfoList.Lock();

	for (pos = m_userInforList.GetHeadPosition(); NULL != pos;)
	{
		User *pUser = (User*)m_userInforList.GetNext(pos);
		if (pUser!=NULL)
		{
			b += _T("<USER ");

			b += _T("loadname=\"");
			b += pUser->m_loadName + _T("\"  ");

			b +=_T("callnamename=\"");
			b += pUser->m_callName + _T("\"  ");

			b +=_T("ip=\"");
			b += pUser->m_userIp + _T("\"  ");

			b +=_T("port=\"");
			b += pUser->m_userPort + _T("\" ");

			b +=_T("state=\"");
			b += pUser->m_userState + _T("\"  ");

			b += _T("/>");
		}
	}
	m_lockUserinfoList.UnLock();

	a += b;
	a += _T("</MSGTYPE>");

	return a;
}
//<MSGTYPE TYPE="CHATMSG">
//  <MSG who="name" towho="name" text="" time="" />
//</MSGTYPE>
CDuiString CMainWindow::BuildChatMsg(CMsg* pMsg)
{
	CDuiString a = _T("<MSGTYPE TYPE=\"CHATMSG\">");
	CDuiString b = _T("<MSG  ");
	b += _T("who=\"");
	b += pMsg->m_nomalMsg.who + _T("\"  ");

	b += _T("towho=\"");
	b += pMsg->m_nomalMsg.toWho + _T("\"  ");

	b += _T("text=\"");
	b += pMsg->m_nomalMsg.msgItem + _T("\"  ");

	b += _T("time=\"");
	b += pMsg->m_nomalMsg.sndTime + _T("\"  ");
	b += _T("/>");

	a += b;
	a += _T("</MSGTYPE>");

	return a;
}
//<MSGTYPE TYPE="SYSMSG">
//  <MSG who="name" what="load" msg="������" time="" />
//</MSGTYPE>
CDuiString CMainWindow::BuildSysMsg(CMsg* pMsg,CDuiString sWhat,CDuiString sTate)
{
	CDuiString a = _T("<MSGTYPE TYPE=\"SYSMSG\">");
	CDuiString b = _T("<MSG ");
	b += _T("who=\"");
	b += pMsg->m_systemMsg.name + _T("\"  ");

	b += _T("what=\"");
	b += sWhat ;
	b += _T("\"  ");

	b += _T("msg=\"");
	b += sTate ;
	b += _T("\"  ");

	b += _T("time=\"");
	b += mci::format_currenttime() + _T("\"  ");
	b += _T("/>");

	a = a + b;
	a += _T("</MSGTYPE>");

	return a;
}
void CMainWindow::DeleteSocket(CClientSocket* pSocket)
{
	pSocket->Close();
	POSITION pos,temp;
	m_lockConnectionList.Lock();
	for(pos=m_connectionList.GetHeadPosition();pos!=NULL;)
	{
		temp = pos;//��ʱ�洢��ǰ��
		CClientSocket* ppSock=(CClientSocket*)m_connectionList.GetNext(pos);
		if(ppSock == pSocket)
		{
			m_connectionList.RemoveAt(temp);
			break;
		}
	}
	delete pSocket;
	m_lockConnectionList.UnLock();
	sLog(_T("�Ƴ������û�..."));
}
void CMainWindow::RemoveUser(CDuiString name)
{
	POSITION pos,temp;	
	User* pUser = NULL;
	m_lockUserinfoList.Lock();
	for (pos = m_userInforList.GetHeadPosition(); NULL != pos;)
	{
		temp = pos;
		pUser = (User*)m_userInforList.GetNext(pos);
		if (pUser&&pUser->m_loadName==name)
		{
			m_userInforList.RemoveAt(temp);
		}
	}
//	delete pUser;
	m_lockUserinfoList.UnLock();
}
//<MSGTYPE TYPE="REGI">
//  <MSG who="name" result="success" time="" />
//</MSGTYPE>
CDuiString CMainWindow::IsUserRegisted(CDuiString& name,CDuiString& password)
{
	CDuiString a = _T("<MSGTYPE TYPE=\"REGI\">");
	ASSERT(theDB);
	if(theDB->IsNameUsed(TO_STDSTRING(name).c_str()))
	{
		CDuiString b = _T("<MSG ");
		b += _T("who=\"");
		b += name + _T("\"  ");
		b += _T("result=\"");
		b += _T("fail");
		b += _T("\"  ");
		b += _T("time=\"");
		b += mci::format_currenttime() + _T("\"  ");
		b += _T("/>");

		a += b + _T("</MSGTYPE>");
	}
	else
	{
		CDuiString b = _T("<MSG ");
		b += _T("who=\"");
		b += name + _T("\"  ");
		b += _T("result=\"");
		b += _T("success");
		b += _T("\"  ");
		b += _T("time=\"");
		b += mci::format_currenttime() + _T("\"  ");
		b += _T("/>");

		a += b + _T("</MSGTYPE>");

		User * user = new User;
		user->m_loadName = name;
		user->m_password = password;
		theDB->InsertUser(*user);

		m_lockUserinfoList.Lock();
		m_userInforList.AddTail(user);
		m_lockUserinfoList.UnLock();
		OnUpdateUserList();
	}
	return a;
}
//<MSGTYPE TYPE="LOAD">
//  <MSG who="name" result="success" time="" />
//</MSGTYPE>
bool CMainWindow::IsUserLoged(CDuiString& name,CDuiString& sf)
{
	POSITION pos;	
	m_lockUserinfoList.Lock();
	sf = _T("<MSGTYPE TYPE=\"LOAD\">");
	bool ret = false;
	for (pos = m_userInforList.GetHeadPosition(); NULL != pos;)
	{
		User *pUser = (User*)m_userInforList.GetNext(pos);
		if (pUser!=NULL&&pUser->m_loadName==name)
		{
			if(pUser->m_userState.Compare(ONLINE) == 0)
			{
				CDuiString b = _T("<MSG ");
				b += _T("who=\"");
				b += name + _T("\"  ");
				b += _T("result=\"");
				b += _T("loged");
				b += _T("\"  ");
				b += _T("time=\"");
				b += mci::format_currenttime() + _T("\"  ");
				b += _T("/>");

				sf += b + _T("</MSGTYPE>");
				ret = true;		
				break;
			}
			else if(pUser->m_userState.Compare(OFFLINE) ==0)
			{
				CDuiString b = _T("<MSG ");
				b += _T("who=\"");
				b += name + _T("\"  ");
				b += _T("result=\"");
				b += _T("success");
				b += _T("\"  ");
				b += _T("time=\"");
				b += mci::format_currenttime() + _T("\"  ");
				b += _T("/>");

				sf += b + _T("</MSGTYPE>");
				ret = false;	
				break;
			}
		}
	}
	if(!pos)
	{
		CDuiString b = _T("<MSG ");
		b += _T("who=\"");
		b += name + _T("\"  ");
		b += _T("result=\"");
		b += _T("fail");
		b += _T("\"  ");
		b += _T("time=\"");
		b += mci::format_currenttime() + _T("\"  ");
		b += _T("/>");

		sf += b + _T("</MSGTYPE>");
	}
	m_lockUserinfoList.UnLock();
	return ret;
}
void CMainWindow::ChangeUserState(CClientSocket* pSocket,CDuiString& name,CDuiString state/*=ONLINE*/)
{
	POSITION pos;	
	SOCKADDR_IN   sa; 
	int  len  = sizeof(sa);
	pSocket->GetPeerName((SOCKADDR*)&sa,&len);
	CDuiString sIP,sPort;
	sIP.Format(_T("%d.%d.%d.%d"),
		sa.sin_addr.S_un.S_un_b.s_b1,
		sa.sin_addr.S_un.S_un_b.s_b2,
		sa.sin_addr.S_un.S_un_b.s_b3,
		sa.sin_addr.S_un.S_un_b.s_b4);
	sPort.Format(_T("%d"),sa.sin_port);

	m_lockUserinfoList.Lock();
	for (pos = m_userInforList.GetHeadPosition(); NULL != pos;)
	{
		User *pUser = (User*)m_userInforList.GetNext(pos);
		if (pUser!=NULL&&pUser->m_loadName==name)
		{
			pUser->m_userState = state;
			if(state.Compare(ONLINE) == 0)
			{
				pUser->m_userIp = sIP;
				pUser->m_userPort = sPort;
			}
			else
			{
				pUser->m_userIp = _T("");
				pUser->m_userPort = _T("");
				DeleteSocket(pSocket);
			}
		}
	}
	m_lockUserinfoList.UnLock();
	sLog(_T("�����û�״̬..."));
}
void CMainWindow::DeleteUser(User user)
{
	POSITION pos,temp;
	m_lockUserinfoList.Lock();
	for (pos = m_userInforList.GetHeadPosition(); NULL != pos;)
	{
		temp = pos;
		User *pUser = (User*)m_userInforList.GetNext(pos);
		if (pUser!=NULL&&pUser->m_loadName==user.m_loadName)
		{
			m_userInforList.RemoveAt(temp);

			delete pUser;
			pUser = NULL;
			break;
		}
	}
	theDB->RemoveUser(TO_STDSTRING(user.m_loadName).c_str());
	m_lockUserinfoList.UnLock();
	sLog(_T("ɾ���û�..."));
}
void CMainWindow::ShakeWindow(HWND hWnd)
{
	int ty = 3;
	RECT   m_rect; 
	::ShowWindow(hWnd,SW_SHOW);
	::BringWindowToTop(hWnd);
	::SetForegroundWindow(hWnd );
	::GetWindowRect(hWnd,&m_rect);  
	int recordy = m_rect.left;
	int recordx = m_rect.top;
	CDuiString sPath = CPaintManagerUI::GetInstancePath();
	if(sPath.Right(1)!=_T("\\")) sPath += _T("\\");
	sPath += _T("sound\\shake.wav");
	PlaySound(sPath,GetModuleHandle(NULL),SND_NODEFAULT | SND_NOWAIT | SND_FILENAME | SND_SYNC); 
	for(int i=0;i<5;i++)
	{
		m_rect.left = recordy;
		m_rect.top = recordx;
		m_rect.top  = m_rect.top + ty;  
		m_rect.left = m_rect.left - ty;
		::SetWindowPos(hWnd,NULL,m_rect.left,m_rect.top,0,0,SWP_NOSIZE);
		Sleep(20);
		m_rect.top = m_rect.top -ty;
		::SetWindowPos( hWnd,NULL,m_rect.left,m_rect.top,0,0,SWP_NOSIZE);
		Sleep(20);
		m_rect.top = m_rect.top -ty;
		::SetWindowPos( hWnd,NULL,m_rect.left,m_rect.top,0,0,SWP_NOSIZE);
		Sleep(20);
		m_rect.left=m_rect.left+ty;
		::SetWindowPos( hWnd,NULL,m_rect.left,m_rect.top,0,0,SWP_NOSIZE);
		Sleep(20);
		m_rect.left=m_rect.left+ty;
		::SetWindowPos( hWnd,NULL,m_rect.left,m_rect.top,0,0,SWP_NOSIZE);
		Sleep(20);
		m_rect.top = m_rect.top + ty;  
		::SetWindowPos( hWnd,NULL,m_rect.left,m_rect.top,0,0,SWP_NOSIZE);
		Sleep(20);
		m_rect.top=m_rect.top+ty;
		::SetWindowPos( hWnd,NULL,m_rect.left,m_rect.top,0,0,SWP_NOSIZE);
		Sleep(20);
		m_rect.top=m_rect.top+ty;
		::SetWindowPos( hWnd,NULL,m_rect.left,m_rect.top,0,0,SWP_NOSIZE);
		Sleep(20);
		::SetWindowPos( hWnd,NULL,recordy,recordx,0,0,SWP_NOSIZE);
		Sleep(3);
	}
}