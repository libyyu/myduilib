
/***********************************************************
 *	FileName��LrcView.h
 *  Author  ��DFn Li
 *  Timer   : 2013/3/20 18:10
 *  E-Mail  : libyyu@qq.com
 *  Comment : show lrc to program panel
 *  Version : 1.0.0.1
 ***********************************************************/
#ifndef LRCVIEW_H
#define LRCVIEW_H
#pragma once
#include "Mgr/LrcMgr/lrcFileReader.h"
#include "GUI/LrcSelectWnd.h"


const static int DELAY_TIME = 500;
const static int TOTAL_ALPHA_VALUE = 220;

class CMyLrcViewUI : public DuiLib::CControlUI
{

public:
	typedef std::shared_ptr<CLrcFileReader>  spLrcReaderT;
public:
	CMyLrcViewUI();
	virtual ~CMyLrcViewUI();

	static LPCTSTR GetClassName();
	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	CLrcSelectWnd* GetLrcSelectWnd();

	void LrcChanged(spSongInfoT song);
	spSongInfoT GetSong(){return m_pSelectLrcSong;}
protected:
	void DoInit();
	void DoEvent(DuiLib::TEventUI& event);
	void PaintText(HDC hDC);

private:
	spLrcReaderT    m_spLrcReader;
	CLrcSelectWnd*  m_LrcSelectWnd;

	spSongInfoT     m_pSelectLrcSong;
	int            m_nAlphaPercent;

	bool           m_bIsDrag;
	DuiLib::CDuiString        m_sOnStopShow;
	
};


#endif//LRCVIEW_H