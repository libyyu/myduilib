#include "MyControls.h"
#include "UIAlbumButton.h"

namespace DuiLib{

//REGIST_DUICONTROL(CAlbumButtonUI);

CAlbumButtonUI::CAlbumButtonUI():CFadeButtonUI()
{

}

LPCTSTR CAlbumButtonUI::GetClass() const
{
	return DUI_CTR_ALBUMBUTTON;
}

LPVOID CAlbumButtonUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, DUI_CTR_ALBUMBUTTON) == 0 ) 
		return static_cast<CAlbumButtonUI*>(this);
	return CFadeButtonUI::GetInterface(pstrName);
}

void CAlbumButtonUI::DoEvent(TEventUI& event)
{
	CFadeButtonUI::DoEvent( event );
}

bool CAlbumButtonUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if (_tcscmp(pstrName, _T("bkhotimage")) == 0)
	{
		SetHotBkImage(pstrValue); 
		return true;
	}
	else 
		return __super::SetAttribute(pstrName,pstrValue);
}
void CAlbumButtonUI::PaintBkImage(HDC hDC)
{
	CFadeAnimation::PaintAnimationStatusImage(hDC,m_diBk,m_diHotBkImage,m_uButtonState);
}

void CAlbumButtonUI::SetHotBkImage(LPCTSTR pStrImage)
{
	if( m_diHotBkImage.sDrawString == pStrImage && m_diHotBkImage.pImageInfo != NULL ) return;
	m_diHotBkImage.Clear();
	m_diHotBkImage.sDrawString = pStrImage;
	Invalidate();
}
CDuiString CAlbumButtonUI::GetHotBkImage() const
{
	return m_diHotBkImage.sDrawString;
}

void CAlbumButtonUI::PaintStatusImage(HDC hDC)
{
	return CButtonUI::PaintStatusImage(hDC); //È¥³ýFADE Ð§¹û
}
void CAlbumButtonUI::SetAlbumImage(LPCTSTR pStrImage)
{
	if(pStrImage && lstrlen(pStrImage) != 0)
	{
		if(m_diOldNormalImage.sDrawString.IsEmpty())
		{
			m_diOldNormalImage.Clear();
			m_diOldNormalImage.sDrawString = GetNormalImage();
		}
		SetNormalImage(pStrImage);
	}
	else
	{
		if(!m_diOldNormalImage.sDrawString.IsEmpty())
			SetNormalImage(m_diOldNormalImage.sDrawString);
	}
}

}