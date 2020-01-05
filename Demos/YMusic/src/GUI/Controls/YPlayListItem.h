#ifndef __YPlayListItem_h
#define __YPlayListItem_h

#pragma once
#include "UIlib.h"
#include "Mgr/DB/ShareData.h"
#include "Marco.hpp"
#include <MyControls/MyControls.h>

class YPlayListItemUI : public DuiLib::CTemplListItemUI
{
public:
	YPlayListItemUI();
	virtual ~YPlayListItemUI();
public:
	static LPCTSTR GetClassName();
	LPVOID GetInterface(LPCTSTR pstrName);
	LPCTSTR GetClass() const;

	void SetPlayList(spPlayListInfoT spList);
	spPlayListInfoT GetPlayList() const;
	void ReName(bool bReName = true);

	void Update();

	bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);

	void OnCreate();
	void ChangeIcon(int bPlaying = em_play_status::em_play);
protected:
	void InitVar();
private:
	spPlayListInfoT _spList;
	DuiLib::CLabelUI* _spListName;
	DuiLib::CEditUI*  _spRenameEdit;
	DuiLib::CControlUI* _spPlayIco;
	bool _bReName;
};

#define  PLAY_LIST  _T("PlayList")
typedef DuiLib::CTemplListUI CPlayListUI;

#endif//__YPlayListItem_h