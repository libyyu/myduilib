#include "stdafx.h"
#include "PropertyWin.h"
#include "YApp.h"

PropertyWin::PropertyWin()
	: m_pFileName(NULL)
	, m_pSongName(NULL)
	, m_pArtistName(NULL)
	, m_pAlbumName(NULL)
	, m_pFilePos(NULL)
{

}
PropertyWin::~PropertyWin()
{

}

LPCTSTR PropertyWin::GetWindowClsss()
{
	return _T("PropertyWin");
}
LPCTSTR PropertyWin::GetWindowClassName() const
{
	return PropertyWin::GetWindowClsss();
}
void PropertyWin::OnFinalMessage( HWND hWnd )
{
	__super::OnFinalMessage( hWnd );
	delete this;
}

LRESULT PropertyWin::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	sLog(_T("PropertyWin::OnCreate"));

	ModifyStyle(WS_CAPTION,WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	if(!RegisterSkin(_T("xml/property.xml"), 0, this))
		return 0;

	GetPaintMgr()->AddNotifier(this);

	GetPaintMgr()->SetLayeredOpacity(0);

	FindControl(m_pFileName,_T("file_name"));
	FindControl(m_pSongName,_T("song_name"));
	FindControl(m_pArtistName,_T("artist_name"));
	FindControl(m_pAlbumName,_T("album_name"));
	FindControl(m_pFilePos,_T("file_pos"));

	return 0;
}

void PropertyWin::Notify(DuiLib::TNotifyUI& msg)
{
	if (msg.sType.Compare(DUI_MSGTYPE_CLICK)==0)
	{
		OnClick(msg);
	}
}

void PropertyWin::OnClick(DuiLib::TNotifyUI& msg)
{
	DuiLib::CControlUI* pUI = msg.pSender;
	if(!pUI) 
		return ;
	if (pUI->IsName(_T("close")))
	{
		Close(IDCANCEL);
	}
	else if (pUI->IsName(_T("ok")))
	{
		Close(IDOK);
	}
	else if (pUI->IsName(_T("file_pos")))
	{
		theMainWin->OnOpenFolderPath(pUI->GetUserData());
	}
}

void PropertyWin::InitWnd(spSongInfoT spSong)
{
	if (spSong->IsLocal())
	{
		m_pFilePos->SetEnabled(true);
		m_pFilePos->SetMouseEnabled(true);
		m_pFilePos->SetText(_T("������Դ"));
		m_pFilePos->SetUserData(spSong->GetLocalPath());
		m_pFileName->SetText(em_utility::mci::rip_file_name(spSong->GetLocalPath().GetData(),true).c_str());
	}
	else
	{
		m_pFilePos->SetEnabled(false);
		m_pFilePos->SetMouseEnabled(false);
		m_pFilePos->SetText(_T("������Դ"));
		m_pFileName->SetText(spSong->GetSongName());
	}
	m_pSongName->SetText(spSong->GetSongName());
	m_pArtistName->SetText(spSong->GetArtistName());
	m_pAlbumName->SetText(spSong->GetAlbumName());
}