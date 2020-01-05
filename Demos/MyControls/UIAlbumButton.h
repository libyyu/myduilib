#ifndef __UIALBUMBUTTON_H
#define __UIALBUMBUTTON_H

#ifdef _MSC_VER
#pragma once
#endif

namespace DuiLib
{
class MYCONTROLS_API CAlbumButtonUI : 
	public CFadeButtonUI
{
	DECLARE_CLASS(CAlbumButtonUI)
public:
	CAlbumButtonUI();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	void DoEvent(TEventUI& event);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	void PaintBkImage(HDC hDC);

	void SetHotBkImage(LPCTSTR pStrImage);
	CDuiString GetHotBkImage() const;

	void PaintStatusImage(HDC hDC);

	void SetAlbumImage(LPCTSTR pStrImage);
private:
	TDrawInfo m_diHotBkImage;
	TDrawInfo m_diOldNormalImage;
};

}

#endif//__UIALBUMBUTTON_H