#include "stdafx.h"
#include "ShareWin.h"
#include "YApp.h"
#include "YMusic.h"

CShareWin::CShareWin()
{
	theMainWin->AddReceive(this);
}
CShareWin::~CShareWin()
{
	RemoveObserver();
}
LPCTSTR CShareWin::GetWindowClass()
{
	return _T("ShareWin");
}
LPCTSTR CShareWin::GetWindowClassName() const
{
	return CShareWin::GetWindowClass();
}

LRESULT CShareWin::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ModifyStyle(WS_CAPTION,WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	if(!RegisterSkin(_T("xml/share-window.xml"), 0, this))
		return 0;
	GetPaintMgr()->AddNotifier(this);

	FindControl(ui._title,_T("title"));
	FindControl(ui._edit,_T("input"));
	FindControl(ui._textlen,_T("textlen"));
	FindControl(ui._sharebtn,_T("sharebtn"));
	theMainWin->BroadCast();

	_nTimerId = SetTimer(1,1000);
	return 0;
}

void CShareWin::Share(spSongInfoT song)
{
	ASSERT(song);
	DuiLib::CDuiString stitle;
	stitle.Format(_T("%s-%s"),song->GetSongName().GetData(),song->GetArtistName().GetData());
	ui._title->SetText(stitle);
	ui._title->SetToolTip(stitle);

	Share(emShareObjT::SOT_SONG,song->GetSongId(),stitle,NULL);
}
void CShareWin::Share(emShareObjT type,std::size_t id,LPCTSTR title,LPCTSTR content)
{
// 	spUserContextT pContext = theRuntimeState->GetCurrentContext(NULL);
// 	spLogonUserInfoT theUser = pContext->GetUser();
// 	if(!theUser)
// 	{
// 		theMainWin->OnShowLoginWindow();
// 		return;
// 	}

	ASSERT(title);
//	if(!(type == _type && id == _id))
	{
//		_type = type;
		_id   = id;

		ui._title->SetText(title);
		ui._title->SetToolTip(title);
		ui._edit->SetText(content);
		ui._sharebtn->SetText(_T("����"));
	}

	if(!IsWindowVisible())
	{
		CenterWindow();
		ShowWindow();
	}
}

void CShareWin::Notify(DuiLib::TNotifyUI& msg)
{
	YTrace_(_T("class:%s,id:%s,msg:%s"),msg.pSender->GetClass(),msg.pSender->GetName().GetData(),msg.sType.GetData());
	if(0 == msg.sType.Compare(DUI_MSGTYPE_CLICK))
		return OnClick(msg);
}
void CShareWin::OnClick(const DuiLib::TNotifyUI& msg)
{
	if(msg.pSender->IsName(_T("closebtn")))
		Close(0L);
	else if(msg.pSender->IsName(_T("sharebtn")))
	{
		spUserContextT pContext = theRuntimeState->GetCurrentContext(NULL);
		DuiLib::CDuiString str(ui._edit->GetText());
	//	theWebServiceMgr->AsynPostRequest(url::Requester::MakeShare(pContext->GetUserId(),(str.IsEmpty() ? _T(" ") : str),_id,_type));
		Close(1L);
	}
	else if(msg.pSender->IsName(_T("sharesetting")))
	{
		DuiLib::CSystem::Instance()->OpenUrl(_T("http://www.xiami.com/share"));
	}
}
LRESULT CShareWin::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
//	if(nIDEvent == _nTimerId)
	{
		DuiLib::CDuiString str;
		str.Format(_T("%d"),ui._edit->GetLimitText() - ui._edit->GetTextLength());
		ui._textlen->SetText(str);
		return 0;
	}
	
//	SetMsgHandled(FALSE);
}
LRESULT CShareWin::OnWebService(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	sLog(_T("CShareWin::OnWebService"));
	return __super::OnWebService(uMsg,wParam,lParam,bHandled);
}

void CShareWin::OnWebService(void *pRequest ,void *pProto)
{

}

LRESULT CShareWin::ProcessWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(uMsg==WM_TIMER)
		return OnTimer(uMsg,wParam,lParam,bHandled);
	return __super::ProcessWindowMessage(uMsg,wParam,lParam,bHandled);
}
BOOL CShareWin::Receive(SkinChangedParam param)
{
	try
	{
		if(!IsWindow()) return FALSE;
		sLog(_T("CShareWin::Receive"));
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
		sLogError(_T("CShareWin::Receive"));
		throw _T("CShareWin::Receive");
		return FALSE;
	}	
}
