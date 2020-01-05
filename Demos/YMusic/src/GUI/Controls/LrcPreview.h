/***********************************************************
 *	FileName£ºLrcPreview.h
 *  Author  £ºDFn Li
 *  Timer   : 2013/3/20 18:10
 *  E-Mail  : libyyu@qq.com
 *  Comment : show lrc to program panel
 *  Version : 1.0.0.1
 ***********************************************************/
#ifndef LRCPREVIEW_H
#define LRCPREVIEW_H

#include "UIlib.h"


class CLrcPreViewUI : public DuiLib::CControlUI
{

public:

public:
	CLrcPreViewUI();
	virtual ~CLrcPreViewUI();

	static LPCTSTR GetClassName();
	LPCTSTR GetClass() const;
protected:
	void DoInit();
	void DoEvent(DuiLib::TEventUI& event);
	void PaintText(HDC hDC);

public:
	DWORD m_dwFontColor;
	DuiLib::CDuiString m_sFontName;
	int    m_iFontSize;
	int    m_iFontStyle;
private:

	DuiLib::CDuiString        m_sLrcPreView;
	
};

class CLrcPanelPreViewUI : public DuiLib::CControlUI
{

public:

public:
	CLrcPanelPreViewUI();
	virtual ~CLrcPanelPreViewUI();

	static LPCTSTR GetClassName();
	LPCTSTR GetClass() const;
protected:
	void DoInit();
	void DoEvent(DuiLib::TEventUI& event);
	void PaintText(HDC hDC);

public:
	DWORD m_dwFontPlayedColor;
	DWORD m_dwFontPlayingColor;
	DWORD m_dwFontNotPlayColor;
	DuiLib::CDuiString m_sFontName;
	int    m_iFontSize;
	int    m_iFontStyle;
private:

	DuiLib::CDuiString        m_sLrcPreView;

};
#endif//LRCPREVIEW_H