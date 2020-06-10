#include "stdafx.h"

#include "YPlayListItem.h"
#include "YApp.h"
#include "Var.h"

const int ListItemHeight = 30;
const int ListItemExpHeight = 50;


YPlayListItemUI::YPlayListItemUI():_spList(NULL)
{
	
}
YPlayListItemUI::~YPlayListItemUI()
{

}

LPCTSTR YPlayListItemUI::GetClassName()
{
	return _T("PlayListItem");
}

LPCTSTR YPlayListItemUI::GetClass() const
{
	return YPlayListItemUI::GetClassName();
}
LPVOID YPlayListItemUI::GetInterface(LPCTSTR pstrName)
{
	if (_tcscmp(pstrName, YPlayListItemUI::GetClassName()) == 0)
		return static_cast<YPlayListItemUI*>(this);
	return __super::GetInterface(pstrName);
}

void YPlayListItemUI::OnCreate()
{
	this->SetFixedHeight(ListItemHeight);

	InitVar();
}
void YPlayListItemUI::InitVar()
{
	_bReName = false;
	_spListName = static_cast<DuiLib::CLabelUI*>(FindSubControl(_T("list-name")));
	_spRenameEdit = static_cast<DuiLib::CEditUI*>(FindSubControl(_T("rename")));
	_spPlayIco = FindSubControl(_T("list_ico"));
	ASSERT(_spListName);
	ASSERT(_spRenameEdit);
	ASSERT(_spPlayIco);
	_spListName->SetTag((UINT_PTR)this);
	_spRenameEdit->SetTag((UINT_PTR)this);
	_spPlayIco->SetTag((UINT_PTR)this);
}

void YPlayListItemUI::SetPlayList(spPlayListInfoT spList)
{
	ASSERT(spList);
	_spList = spList;

	Update();
}
spPlayListInfoT YPlayListItemUI::GetPlayList() const
{
	return _spList;
}
void YPlayListItemUI::ReName(bool bReName)
{
	if(bReName)
	{
		_bReName = true;
		_spRenameEdit->SetText(_spList->GetName());
		_spRenameEdit->SetToolTip(_T(""));
		_spRenameEdit->SetVisible(true);
		_spListName->SetVisible(false);
		_spPlayIco->SetVisible(false);
		_spRenameEdit->SetFocus();
	}
	else
	{
		_bReName = false;
		_spListName->SetText(_spRenameEdit->GetText());
		_spListName->SetToolTip(_spRenameEdit->GetText());
		_spRenameEdit->SetVisible(false);
		_spListName->SetVisible(true);
		_spPlayIco->SetVisible((_spPlayIco->GetBkImage()!=NULL));
		_spList->SetName(_spRenameEdit->GetText().GetData());
	}
}
void YPlayListItemUI::Update()
{
	if(_spList)
	{
		_spListName->SetText(_spList->GetName());
		_spListName->SetToolTip(_spList->GetName());
		_spRenameEdit->SetText(_spList->GetName());
	}
	else
	{
		_spListName->SetText(UnKnown);
		_spListName->SetToolTip(UnKnown);
		_spRenameEdit->SetText(UnKnown);
	}
}
void YPlayListItemUI::ChangeIcon(int bPlaying /* = em_play */)
{
	static DuiLib::CDuiString sPlayImg = _spPlayIco->GetBkImage();
	static DuiLib::CDuiString sPauseImg = _spPlayIco->GetUserData();
	if(bPlaying==em_stop)
		_spPlayIco->SetVisible(false);
	else if (em_play==bPlaying)
	{
		_spPlayIco->SetVisible(true);
		_spPlayIco->SetBkImage(sPlayImg);
		_spPlayIco->SetUserData(sPauseImg);
	}
	else if(em_pause==bPlaying)
	{
		_spPlayIco->SetVisible(true);
		_spPlayIco->SetBkImage(sPauseImg);
		_spPlayIco->SetUserData(sPlayImg);
	}
}

bool YPlayListItemUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl)
{
	if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return false;
	if (!GetOwner()) return false;
	ASSERT(GetOwner());
	if (_spList)
	{
// 		int nCurPlayListLocalId = thePlayCenter->GetListIndex();
// 		if (_spList->GetLocalId()==nCurPlayListLocalId)
		spPlayListInfoT spList = thePlayListMgr->GetCurList();
		if(spList&&spList->GetLocalId()==_spList->GetLocalId())
		{
			DuiLib::CDuiString sText = _spList->GetName();
			TCHAR szBuf[MAX_PATH] = {0};
#if defined(UNDER_WINCE)
			_stprintf(szBuf, _T("<f 1><c #F27D30>%s</c></f>"), sText.GetData());
#else
			_stprintf_s(szBuf, MAX_PATH - 1, _T("<f 1><c #F27D30>%s</c></f>"), sText.GetData());
#endif
			_spListName->SetText(szBuf);

			static DuiLib::CDuiString sPlayImg = _spPlayIco->GetBkImage();
			static DuiLib::CDuiString sPauseImg = _spPlayIco->GetUserData();
			int nStatus = thePlayer->GetStatus();
			ChangeIcon(nStatus);
			
// 			RECT rt = {15,0,0,0};
// #define		BK_IMG_PLAYING  _T("file='res/Menu/play.png' source='13,0,26,13' dest='2,8,15,21' mask='0xFF000000'")
// 			_spListName->SetFont(1);
// 			_spListName->SetTextPadding(rt);
// 			_spListName->SetText(sText);
// 			_spListName->SetBkImage(BK_IMG_PLAYING);
//			_spListName->ApplyAttributeList(_T("font=\"3\" bkimage=\"file='res/Menu/play.png' source='13,0,26,13' dest='2,8,15,21' mask='0xFF000000'\" textpadding=\"15,0,0,0\""));
		}
		else
		{
			DuiLib::CDuiString sText = _spList->GetName();
			TCHAR szBuf[MAX_PATH] = {0};
#if defined(UNDER_WINCE)
			_stprintf(szBuf, _T("<f 0>%s</f>"), sText.GetData());
#else
			_stprintf_s(szBuf, MAX_PATH - 1, _T("<f 0>%s</f>"), sText.GetData());
#endif
			_spListName->SetText(szBuf);
	        ChangeIcon(em_stop);
// 			RECT rt = {0,0,0,0};
// #define		BK_IMG_NORMAL  _T("")
// 			_spListName->SetFont(1);
// 			_spListName->SetTextPadding(rt);
// 			_spListName->SetText(sText);
// 			_spListName->SetBkImage(BK_IMG_NORMAL);
		}
	}
	return __super::DoPaint(hDC,rcPaint, pStopControl);
}