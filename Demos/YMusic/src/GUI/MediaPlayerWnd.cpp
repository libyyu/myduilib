#include "stdafx.h"
#include "Marco.hpp"
#include "MediaPlayerWnd.h"
#include "YApp.h"
#include "YMusic.h"
#include "Utility/Convert.h"

namespace UIMediaWin
{
	static const TCHAR kTitle[] = _T("title");
	static const TCHAR kMin[] = _T("minbtn");
	static const TCHAR kClose[] = _T("closebtn");
	static const TCHAR kMax[] = _T("maxbtn");
	static const TCHAR kRestore[] = _T("restorebtn");
	static const TCHAR kPlayer[] = _T("mediaPlayer");
	static const TCHAR kSliLayout[] = _T("ctnSlider");
	static const TCHAR kProctlSli[] = _T("sliderPlay");
	static const TCHAR kBackward[] = _T("btnFastBackward");
	static const TCHAR kForward[] = _T("btnFastForward");
	static const TCHAR kTimeUse[] = _T("labelPlayTime");
//	static const TCHAR kTimeAll[] = _T("");
	static const TCHAR kPlayBtn[] = _T("btnPlay");
	static const TCHAR kPauseBtn[] = _T("btnPause");
	static const TCHAR kPrevBtn[] = _T("btnPrevious");
	static const TCHAR kNextBtn[] = _T("btnNext");
	static const TCHAR kStopBtn[] = _T("btnStop");
	static const TCHAR kVolOffBtn[] = _T("btnVolume");
	static const TCHAR kVolOnBtn[] = _T("btnVolumeZero");
	static const TCHAR kVolSli[] = _T("sliderVol");
	static const TCHAR kFullScreenBtn[] = _T("btnScreenFull");
	static const TCHAR kNormalBtn[] = _T("btnScreenNormal");
}

CMediaWin::CMediaWin()
{
	m_strCurPlay = "";
	m_nVol = 50;
	m_bVolOn = true;
	ZeroMemory(&_ui,sizeof(_ui));
	m_bFullScreenMode = false;
	theMainWin->AddReceive(this);
}
CMediaWin::~CMediaWin()
{
	RemoveObserver();
}
LPCTSTR CMediaWin::GetWindowClass()
{
	return _T("MediaWin");
}
LPCTSTR CMediaWin::GetWindowClassName() const
{
	return CMediaWin::GetWindowClass();
}

void CMediaWin::OnFinalMessage( HWND hWnd )
{
	thePlayer->SetHWND(theMainWin->GetHWND());

	GetPaintMgr()->RemovePreMessageFilter(this);
	__super::OnFinalMessage(hWnd);
}

CMediaWin* CMediaWin::GetMediaPlayer()
{
	CMediaWin* pMedia = CMediaWin::Instance();
	if(!pMedia->IsWindow())
		pMedia->Create(theMainWin->GetHWND(),_T("MediaWid"),UI_WNDSTYLE_FRAME,0);
	pMedia->CenterWindow();
	pMedia->ShowWindow(true);
	ASSERT(pMedia->IsWindow());
	return pMedia;
}

LRESULT CMediaWin::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ModifyStyle(WS_CAPTION,WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	if(!RegisterSkin(_T("xml/Media.xml"),0,this))
		return 0;

	GetPaintMgr()->AddPreMessageFilter(this);
	GetPaintMgr()->AddNotifier(this);
	
	FindControl(_ui.m_pTitle,UIMediaWin::kTitle);
	FindControl(_ui.m_pMaxBtn,UIMediaWin::kMax);
	FindControl(_ui.m_pResBtn,UIMediaWin::kRestore);
	FindControl(_ui.m_pMediaPlayer,UIMediaWin::kPlayer);
	FindControl(_ui.m_pSliLayout,UIMediaWin::kSliLayout);
	FindControl(_ui.m_pProgressSli,UIMediaWin::kProctlSli);
	FindControl(_ui.m_pBackWard,UIMediaWin::kBackward);
	FindControl(_ui.m_pForWard,UIMediaWin::kForward);
	FindControl(_ui.m_pTimeUse,UIMediaWin::kTimeUse);
	FindControl(_ui.m_pPlayBtn,UIMediaWin::kPlayBtn);
	FindControl(_ui.m_pPauseBtn,UIMediaWin::kPauseBtn);
	FindControl(_ui.m_pPrevBtn,UIMediaWin::kPrevBtn);
	FindControl(_ui.m_pNextBtn,UIMediaWin::kNextBtn);
	FindControl(_ui.m_pStopBtn,UIMediaWin::kStopBtn);
	FindControl(_ui.m_pVolumeOffBtn,UIMediaWin::kVolOffBtn);
	FindControl(_ui.m_pVolumeOnBtn,UIMediaWin::kVolOnBtn);
	FindControl(_ui.m_pVolumeSli,UIMediaWin::kVolSli);
	FindControl(_ui.m_pFullScreenBtn,UIMediaWin::kFullScreenBtn);
	FindControl(_ui.m_pNormalBtn,UIMediaWin::kNormalBtn);
	assert( _ui.m_pTitle
		&& _ui.m_pMaxBtn 
		&& _ui.m_pResBtn 
		&& _ui.m_pMediaPlayer
		&& _ui.m_pSliLayout
		&& _ui.m_pProgressSli
		&& _ui.m_pBackWard
		&& _ui.m_pForWard
		&& _ui.m_pTimeUse
		&& _ui.m_pPlayBtn
		&& _ui.m_pPauseBtn
		&& _ui.m_pPrevBtn
		&& _ui.m_pNextBtn
		&& _ui.m_pStopBtn
		&& _ui.m_pVolumeOffBtn
		&& _ui.m_pVolumeOnBtn
		&& _ui.m_pVolumeSli
		&& _ui.m_pFullScreenBtn
		&& _ui.m_pNormalBtn);

	_ui.m_pVolumeSli->OnNotify += MakeDelegate(this, &CMediaWin::OnVolumeChanged);
	_ui.m_pProgressSli->OnNotify += MakeDelegate(this, &CMediaWin::OnPosChanged);

	m_bVolOn = theConfigMgr->GetNormalConfig()->IsVolumeOff();
	m_nVol = theConfigMgr->GetNormalConfig()->GetVolume();

	theMainWin->BroadCast();

	return 0;
}
DuiLib::CControlUI* CMediaWin::CreateControl(LPCTSTR pstrClassName)
{
	if (_tcsicmp(pstrClassName, _T("Media")) == 0)
	{
		_ui.m_pMediaPlayer = new CMediaUI;   
		assert(_ui.m_pMediaPlayer);
		HWND   hWnd = CreateWindow(_T("#32770"), _T("WndMediaDisplay"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, GetPaintMgr()->GetPaintWindow(), (HMENU)0, NULL, NULL);
		_ui.m_pMediaPlayer->Attach(hWnd);  
		return _ui.m_pMediaPlayer;
	}
	else 
		return __super::CreateControl(pstrClassName);
}
void CMediaWin::Notify(DuiLib::TNotifyUI& msg)
{
	YTrace_(_T("class:%s,id:%s,msg:%s"),msg.pSender->GetClass(),msg.pSender->GetName().GetData(),msg.sType.GetData());
	if(0 == msg.sType.Compare(DUI_MSGTYPE_CLICK))
		return OnClick(msg);
	else if( 0 == msg.sType.Compare(DUI_MSGTYPE_DBCLICK))
		FullScreen(! m_bFullScreenMode);
}
void CMediaWin::OnClick(const DuiLib::TNotifyUI& msg)
{
	if(msg.pSender->IsName(UIMediaWin::kClose))
		SendMessage(WM_SYSCOMMAND,SC_CLOSE, 0);
	else if(msg.pSender->IsName(UIMediaWin::kMin))
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	else if(msg.pSender->IsName(UIMediaWin::kMax))
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	else if(msg.pSender->IsName(UIMediaWin::kRestore))
		SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
	else if(msg.pSender->IsName(UIMediaWin::kPlayBtn))
		Play();
	else if(msg.pSender->IsName(UIMediaWin::kPauseBtn))
		Pause();
	else if(msg.pSender->IsName(UIMediaWin::kStopBtn))
		Stop();
	else if(msg.pSender->IsName(UIMediaWin::kVolOffBtn))
		SwitchVol(false);
	else if(msg.pSender->IsName(UIMediaWin::kVolOnBtn))
		SwitchVol(true);
	else if(msg.pSender->IsName(UIMediaWin::kFullScreenBtn))
		FullScreen(true);
	else if(msg.pSender->IsName(UIMediaWin::kNormalBtn))
		FullScreen(false);
}
LRESULT CMediaWin::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// ��ʱ�����յ�WM_NCDESTROY���յ�wParamΪSC_CLOSE��WM_SYSCOMMAND
	if( wParam == SC_CLOSE ) {
		thePlayer->Stop(); 
		thePlayer->SetHWND(theMainWin->GetHWND());
		Close(0L);	
//		Sleep(1000);
		bHandled = TRUE;
		return 0;
	}
	BOOL bZoomed = ::IsZoomed(*this);
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	if( ::IsZoomed(*this) != bZoomed ) {
		if( !bZoomed ) {
			_ui.m_pMaxBtn->SetVisible(false);
			_ui.m_pResBtn->SetVisible(true);
		}
		else {
			_ui.m_pMaxBtn->SetVisible(true);
			_ui.m_pResBtn->SetVisible(false);
		}
	}
	return lRes;
}

LRESULT CMediaWin::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}
LRESULT CMediaWin::OnWebService(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	sLog("CMediaWin::OnWebService");
	return __super::OnWebService(uMsg,wParam,lParam,bHandled);
}

void CMediaWin::OnWebService(void *pRequest ,void *pProto)
{

}
LRESULT CMediaWin::OnNcHitTest( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if (m_bFullScreenMode)
	{
		return HTCLIENT;
	}

	return __super::OnNcHitTest(uMsg, wParam, lParam, bHandled);
}
LRESULT CMediaWin::ProcessWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(uMsg==WM_TIMER)
		return OnTimer(uMsg,wParam,lParam,bHandled);
	if(uMsg==WM_USER_PLAYING)
		OnPlayerPlaying(uMsg,wParam,lParam,bHandled);
	if(uMsg==WM_USER_POS_CHANGED)
		OnPlayerPosChanged(uMsg,wParam,lParam,bHandled);
	if(uMsg==WM_USER_END_REACHED)
		OnPlayerEndReached(uMsg,wParam,lParam,bHandled);
	
	return __super::ProcessWindowMessage(uMsg,wParam,lParam,bHandled);
}
LRESULT CMediaWin::ResponseDefaultKeyEvent(WPARAM wParam)
{
	if (wParam == VK_RETURN)
	{
		return FALSE;
	}
	else if (wParam == VK_ESCAPE)
	{
		FullScreen(false);
		return TRUE;
	}

	return FALSE;
}
LRESULT CMediaWin::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM , bool& bHandled)
{
	if (uMsg == WM_KEYDOWN)
	{
		switch (wParam)
		{
		case VK_RETURN:
		case VK_ESCAPE:
			return ResponseDefaultKeyEvent(wParam);
		default:
			break;
		}
	}
	return FALSE;
}

BOOL CMediaWin::Receive(SkinChangedParam param)
{
	try
	{
		if(!IsWindow()) return FALSE;
		sLog("CMediaWin::Receive");
		DuiLib::CDuiString bgimage_ = param.bgimage;
		DWORD bkcolor_ = param.bkcolor;
		DuiLib::CControlUI* background = GetPaintMgr()->GetRoot();
// 		if (background != NULL&&0!=bgimage_.Compare(background->GetBkImage()))
// 		{
// 			if (!param.bgimage.IsEmpty())
// 			{
// 				CDuiString sBkImage = bgimage_;
// 
// 				background->SetBkImage(sBkImage);
// 			}
// 			else
// 				background->SetBkImage(_T(""));
// 
// 			background->SetBkColor(param.bkcolor);
// 		}

		return TRUE;
	}
	catch (...)
	{
		sLogError("CMediaWin::Receive");
		throw _T("CMediaWin::Receive");
		return FALSE;
	}	
}

void CMediaWin::ReadyToPlay(LPCTSTR pstrName)
{
	ASSERT(pstrName);
	if(!pstrName)
		return ;
	DuiLib::CDuiString sName(pstrName);
	m_strCurPlay = TransToStdString(sName);
}
bool CMediaWin::Play()
{
	if (thePlayer->GetHWND()!=_ui.m_pMediaPlayer->GetHWND())
	{
		thePlayer->SetHWND(_ui.m_pMediaPlayer->GetHWND());
		theMainWin->Stop();
	}
	if (thePlayer->GetStatus()==em_pause)
	{
		Resume();
		return true;
	}
	Stop();
	// ���ò������Ĵ��ھ���ͻص�����
	if(!m_strCurPlay.empty())
	{
		thePlayer->PlayFile(m_strCurPlay);

		ChangeBtnStatus(false);

		_ui.m_pFullScreenBtn->SetEnabled(true);

		_ui.m_pTitle->SetText(TransToDuiString(m_strCurPlay.c_str()));
	}
	return true;
}

//-------------------------------------------------------------------
LRESULT CMediaWin::OnPlayerPlaying(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	sLog("Playing");
	return TRUE;
}
//-------------------------------------------------------------------
LRESULT CMediaWin::OnPlayerPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//	sLog("PlayPosChanged");
	UpdatePlayingProgress();
	return TRUE;
}
//-------------------------------------------------------------------
LRESULT CMediaWin::OnPlayerEndReached(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	sLog(_T("EndReached"));
	Stop();
	return TRUE;
}

void CMediaWin::UpdatePlayingProgress()
{
	DuiLib::CDuiString  strTime;
	struct tm   tmCurrent,tmTotal;
	time_t      timeCurrent = thePlayer->GetTime() / 1000;
	time_t      timeTotal   = thePlayer->GetTotalTime() / 1000;
	TCHAR       szCurrent[MAX_PATH],szTotal[MAX_PATH];

	gmtime_s(&tmCurrent, &timeCurrent);
	gmtime_s(&tmTotal, &timeTotal);

	_tcsftime(szCurrent, MAX_PATH, _T("%X"), &tmCurrent);
	_tcsftime(szTotal,   MAX_PATH, _T("%X"), &tmTotal);
	strTime.Format(_T("%s/%s"), szCurrent,szTotal);

	SetTimeText(strTime);
	SetProgress(thePlayer->GetPos(),true);
}

void CMediaWin::Pause()
{
	thePlayer->Pause();
	ChangeBtnStatus(true);
}

void CMediaWin::Resume()
{
	thePlayer->Play();
	ChangeBtnStatus(false);
}

void CMediaWin::Stop()
{
	ChangeBtnStatus(true);
	thePlayer->Stop();
	SetTimeText(_T(""));
	SetProgress(0,false);
	_ui.m_pTitle->SetText(_T("������Ƶ����"));
}

void CMediaWin::ChangeBtnStatus(bool bPlaying)
{
	_ui.m_pPlayBtn->SetVisible(bPlaying);
	_ui.m_pPauseBtn->SetVisible(!bPlaying);
	_ui.m_pStopBtn->SetEnabled(!bPlaying);
	
	_ui.m_pFullScreenBtn->SetVisible(!bPlaying);
	_ui.m_pNormalBtn->SetVisible(bPlaying);
}

void CMediaWin::FullScreen(bool bFull /* = true */)
{
	DuiLib::CControlUI* pUICaption = GetPaintMgr()->FindControl(_T("caption"));
	int iBorderX = GetSystemMetrics(SM_CXFIXEDFRAME) + GetSystemMetrics(SM_CXBORDER);
	int iBorderY = GetSystemMetrics(SM_CYFIXEDFRAME) + GetSystemMetrics(SM_CYBORDER);

	if (_ui.m_pFullScreenBtn && _ui.m_pNormalBtn && pUICaption)
	{
		m_bFullScreenMode = bFull;

		if (bFull)
		{
			::GetWindowPlacement(*this, &m_OldWndPlacement);

			if (::IsZoomed(*this))
			{
				::ShowWindow(*this, SW_SHOWDEFAULT);
			}

			::SetWindowPos(*this, HWND_TOPMOST, -iBorderX, -iBorderY, GetSystemMetrics(SM_CXSCREEN) + 2 * iBorderX, GetSystemMetrics(SM_CYSCREEN) + 2 * iBorderY, 0);
		} 
		else
		{
			::SetWindowPlacement(*this, &m_OldWndPlacement);
			::SetWindowPos(*this, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		}

		_ui.m_pNormalBtn->SetVisible(bFull);
		pUICaption->SetVisible(! bFull);
		_ui.m_pFullScreenBtn->SetVisible(! bFull);
	}
}

void CMediaWin::SetVolume(int nVal)
{
	m_nVol = nVal;
	thePlayer->Volume(nVal);
}

void CMediaWin::SwitchVol(bool bOn /* = true */)
{
	m_bVolOn = bOn;
	if (m_bVolOn)
	{
		thePlayer->Volume(m_nVol);
	}
	else
	{
		thePlayer->Volume(0);
	}
	_ui.m_pVolumeOnBtn->SetVisible(!m_bVolOn);
	_ui.m_pVolumeOffBtn->SetVisible(m_bVolOn);
}

void CMediaWin::SetTimeText(LPCTSTR szText)
{
	_ui.m_pTimeUse->SetText(szText);
}

void CMediaWin::SetProgress(int nVal,bool bShowLayout)
{
	if(_ui.m_pSliLayout->IsVisible() != bShowLayout)
		_ui.m_pSliLayout->SetVisible(bShowLayout);

	_ui.m_pProgressSli->SetValue(nVal);
}

bool CMediaWin::OnPosChanged(void* param)
{
	DuiLib::TNotifyUI* pMsg = (DuiLib::TNotifyUI*)param;

	if( pMsg->sType == _T("valuechanged") )
	{
		thePlayer->SeekTo((static_cast<DuiLib::CSliderUI*>(pMsg->pSender))->GetValue() + 1); // ��ȡ��ֵ����1���������õ�ֵҲ����1����������+1
	}

	return true;
}

bool CMediaWin::OnVolumeChanged(void* param)
{
	DuiLib::TNotifyUI* pMsg = (DuiLib::TNotifyUI*)param;

	if( pMsg->sType == _T("valuechanged") )
	{
		SetVolume((static_cast<DuiLib::CSliderUI*>(pMsg->pSender))->GetValue());
	}

	return true;
}