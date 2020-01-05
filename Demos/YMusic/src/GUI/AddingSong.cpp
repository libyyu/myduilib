#include "stdafx.h"
#include "AddingSong.h"

YAddingSong::YAddingSong(bool bOpenFile,DuiLib::CListUI* pSongList)
{
	ASSERT(pSongList);
	_bOpenFile = bOpenFile;
	_pSongList = pSongList;
	_bCancel = false;
	_pText = NULL;
}
YAddingSong::~YAddingSong()
{}

LPCTSTR YAddingSong::GetWindowClsss()
{
	return _T("YAddingSong");
}
LPCTSTR YAddingSong::GetWindowClassName() const
{
	return YAddingSong::GetWindowClsss();
}
void YAddingSong::OnFinalMessage( HWND hWnd )
{
	__super::OnFinalMessage(hWnd);
	delete this;
}
LRESULT YAddingSong::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ModifyStyle(WS_CAPTION,WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	if(!RegisterSkin(_T("xml/adding-window.xml"), 0, this))
		return 0;

	GetPaintMgr()->AddNotifier(this);
	RECT rcClient;
	::GetClientRect(*this, &rcClient);
	::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
		rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

	if(_bOpenFile)
		CFileMgr::Instance()->OpenFile(m_hWnd,_files);
	else
		CFileMgr::Instance()->OpenFolder(m_hWnd,_files);

	return 0;
}
void YAddingSong::Notify(DuiLib::TNotifyUI& msg)
{
	if (msg.sType.Compare(DUI_MSGTYPE_CLICK)==0)
		OnClick(msg);
	else if(msg.sType.Compare(DUI_MSGTYPE_WINDOWINIT)==0)
		OnInit(msg);
}
void YAddingSong::OnClick(DuiLib::TNotifyUI& msg)
{
	DuiLib::CControlUI* pSender = msg.pSender;
	if (pSender->IsName(_T("closebtn")))
	{
		Close(0);
	}
}
void YAddingSong::OnInit(DuiLib::TNotifyUI& msg)
{
	FindControl(_pText,_T("info"));
	ASSERT(_pText);
	ShowDetail(_files);
}
int YAddingSong::ShowDetail(CFileMgr::filesArrayT files)
{
	int iSearched = 0;
	/*for (CFileMgr::filesArrayIterT iter=files.begin();iter!=files.end();++iter)
	{
		TStreamFormat tf = thePlayCenter->GetFileFormat(*iter);
		if (tf==TStreamFormat::sfUnknown)
		{
			sLogWarn("����<%s>��ʽ�ݲ�֧��",iter->GetBuffer());
			iter->ReleaseBuffer();
		}
		else
		{
			iSearched++;
			spSongInfoT spSongInfo;
			theDbMgr->GetUserDB()->AddSongToPlayList(*iter,(unsigned int)(_pSongList->GetTag()),spSongInfo);
			thePlayCenter->AddSongToList(*spSongInfo,(unsigned int)(_pSongList->GetTag()));
			YSongListItemUI* songItem = new YSongListItemUI;
			ASSERT(songItem);
			songItem->SetSong(spSongInfo);
			_pSongList->Add(songItem);

			_pText->SetText(_T("���ڵ���:")+spSongInfo->GetLocalPath());
		}
	}*/
	return iSearched;
}