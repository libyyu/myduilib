#include "stdafx.h"
#include "TagModifyWin.h"
#include "YApp.h"
#include "Mgr/PlayMgr/audioinfo.h"
#include "Controls/YPlayListItem.h"
#include "Controls/YSongListItem.h"
#include "Var.h"
TagModifyWin::TagModifyWin()
	: m_pSongName(NULL)
	, m_pArtistName(NULL)
	, m_pAlbumName(NULL)
	, m_nListId(-1)
{
	theMainWin->AddReceive(this);
}

TagModifyWin::~TagModifyWin()
{
	RemoveObserver();
}

LPCTSTR TagModifyWin::GetWindowClass()
{
	return _T("TagModifyWin");
}

LPCTSTR TagModifyWin::GetWindowClassName() const
{
	return TagModifyWin::GetWindowClass();
}

LRESULT TagModifyWin::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	sLog(_T("TagModifyWin::OnCreate"));

	ModifyStyle(WS_CAPTION,WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	if(!RegisterSkin(_T("xml/tagmodify.xml"), 0, this))
		return 0;
	GetPaintMgr()->AddNotifier(this);

	theMainWin->BroadCast();

	FindControl(m_pSongName,_T("song_name"));
	FindControl(m_pArtistName,_T("artist_name"));
	FindControl(m_pAlbumName,_T("album_name"));

	GetPaintMgr()->SetNextTabControl();

	return 0;
}

BOOL TagModifyWin::Receive(SkinChangedParam param)
{
	try
	{
		if(!IsWindow()) return FALSE;
		sLog(_T("TagModifyWin::Receive"));
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
		sLogError(_T("TagModifyWin::Receive"));
		throw _T("TagModifyWin::Receive");
		return FALSE;
	}	
}

bool TagModifyWin::InitWnd(spSongInfoT spSong,int nListId)
{
	if(!spSong)
		return false;
	assert(spSong);
	m_pSong = spSong;
	m_nListId = nListId;
	if (TagModifyWin::Instance()->IsWindow())
	{
	}
	else
	{
		TagModifyWin::Instance()->Create(theMainWin->GetHWND(),_T("Tag�޸�"),
			UI_WNDSTYLE_DIALOG,WS_EX_WINDOWEDGE | WS_EX_OVERLAPPEDWINDOW);
	}

	CenterWindow();

	
	
	m_pSongName->SetText(spSong->GetSongName());
	m_pArtistName->SetText(spSong->GetArtistName());
	m_pAlbumName->SetText(spSong->GetAlbumName());

	ShowModal();

	return true;
}

void TagModifyWin::Notify(DuiLib::TNotifyUI& msg)
{
	if (msg.sType.Compare(DUI_MSGTYPE_CLICK)==0)
	{
		OnClick(msg);
	}
}
void TagModifyWin::OnClick(DuiLib::TNotifyUI& msg)
{
	DuiLib::CControlUI* pUI = msg.pSender;
	if(!pUI)return ;
	if (pUI->IsName(_T("close")) || pUI->IsName(_T("cancle")))
	{
		Close(IDCANCEL);
	}
	else if (pUI->IsName(_T("modify")))
	{
		OnModify();
	}
}
void TagModifyWin::OnModify()
{
	if (!m_pSong)
	{
		sLogError("song to modify is null");
		return ;
	}
	if (-1==m_nListId)
	{
		sLogError("list id is invalid");
		return ;
	}
	DuiLib::CControlUI* pUI = NULL;
	FindControl(pUI,_T("modify"));
	ASSERT(pUI);

	RECT rtWin;
	::GetWindowRect(theMainWin->GetHWND(),&rtWin);
	POINT rtPos = {rtWin.left+25,rtWin.top+35};
	::ScreenToClient(theMainWin->GetHWND(),&rtPos);

	DuiLib::CDuiString sText[3];
	sText[0] = m_pSongName->GetText();
	sText[1] = m_pArtistName->GetText();
	sText[2] = m_pAlbumName->GetText();

	DuiLib::CDuiString sOld[3];
	sOld[0] = m_pSong->GetSongName();
	sOld[1] = m_pSong->GetArtistName();
	sOld[2] = m_pSong->GetAlbumName();
	if( (0==sText[0].Compare(sOld[0]))&&
		(0==sText[1].Compare(sOld[1]))&&
		(0==sText[2].Compare(sOld[2])) )
	{
		theMainWin->ShowInfoWindow(rtPos,_T("                           ���³ɹ�                           "));
		Close(IDOK);
		return;
	}
	m_pSong->SetSongName(sText[0]);
	m_pSong->SetArtistName(sText[1]);
	m_pSong->SetAblumName(sText[2]);
#define MB_2_UTF8(x) DuiLib::Convert::MBToUTF8(TransToStdString(x).c_str(), -1)
	bool bResult = theDbMgr->GetUserDB()->UpdatePlayListSongInfo(m_pSong,m_nListId);
	if (bResult)
	{
		theMainWin->ShowInfoWindow(rtPos,_T("                           ���³ɹ�                           "));
		//�����б�����
		thePlayListMgr->UpdateSong(m_pSong,m_nListId);
		if (m_pSong->IsLocal())
		{//���ظ������޸��ļ�tag
			AudioTagInfo audioinfo;
			audioinfo.init(MB_2_UTF8(m_pSong->GetLrcLocalPath()));
			audioinfo.title(MB_2_UTF8(m_pSong->GetSongName()));
			audioinfo.artist(MB_2_UTF8(m_pSong->GetArtistName()));
			audioinfo.album(MB_2_UTF8(m_pSong->GetAlbumName()));
			audioinfo.save();
		}
		//����UI
		DuiLib::CListUI* pList = NULL;
		theMainWin->FindControl(pList,varMainWindow::kPlayList);
		ASSERT(pList);
		int nCurSel = pList->GetCurSel();
		if(nCurSel<0) return;
		YPlayListItemUI* pListItem = static_cast<YPlayListItemUI*>(pList->GetItemAt(nCurSel));
		ASSERT(pListItem);
		spPlayListInfoT spListCur = pListItem->GetPlayList();
		if (spListCur&&spListCur->GetLocalId()==m_nListId)
		{
			DuiLib::CListUI* pSongs = NULL;
			theMainWin->FindControl(pSongs,varMainWindow::kSongList);
			ASSERT(pSongs);
			int nSel = pSongs->GetCurSel();
			if(nSel<0) return;
			YSongListItemUI* pSongItem = static_cast<YSongListItemUI*>(pSongs->GetItemAt(nSel));
			ASSERT(pSongItem);
			pSongItem->SetSong(m_pSong);
			pSongItem->Update();
			//���Ϊ���ڲ��Ÿ���
			if (thePlayListMgr->GetCurSong()&&thePlayListMgr->GetCurSong()->GetLocalId()==m_pSong->GetLocalId())
			{
				theMainWin->ShowSongInfo(m_pSong);
// 				CLabelUI* pSongName = NULL;
// 				theMainWin->FindControl(pSongName,varMainWindow::kSongName);
// 				ASSERT(pSongName);
// 				pSongName->SetText(m_pSong->GetSongName());
// 				CLabelUI* pAthourName = NULL;
// 				theMainWin->FindControl(pAthourName,varMainWindow::kAuthorName);
// 				ASSERT(pAthourName);
// 				pAthourName->SetText(m_pSong->GetArtistName());
// 				pSongName->SetToolTip(m_pSong->GetSongName());
// 				pAthourName->SetToolTip(m_pSong->GetArtistName());
			}
		}
		Close(IDOK);
	}
	else
	{
		theMainWin->ShowInfoWindow(rtPos,_T("                           ����ʧ��                           "));
		Close(IDCANCEL);
	}
#undef MB_2_UTF8
}