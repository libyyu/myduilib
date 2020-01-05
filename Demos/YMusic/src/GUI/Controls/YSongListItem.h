#ifndef __YSongListItem_h
#define __YSongListItem_h
#pragma once
#include "UIlib.h"
#include "Mgr/DB/ShareData.h"
#include "Marco.hpp"
#include <MyControls/MyControls.h>
class YSongListItemUI : public DuiLib::CTemplListItemUI
{
public:
	YSongListItemUI();
	virtual ~YSongListItemUI();
public:
	static LPCTSTR GetClassName();
	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	void SetSong(spSongInfoT song);
	spSongInfoT GetSong() const;

	void OnCreate();
	void Update();
	bool DoPaint(HDC hDC, const RECT& rcPaint, DuiLib::CControlUI* pStopControl);
	void DoEvent(DuiLib::TEventUI& event);
	bool DoNotify(void* param);

	void ChangeIcon(int bPlaying = em_play_status::em_play);
protected:
	void InitVar();

private:
	spSongInfoT _spSongInfo;
	DuiLib::CLabelUI* _pNorLabelIndex;
	DuiLib::CLabelUI* _pNorLabelName;
	DuiLib::CLabelUI* _pNorLabelAritst;
	DuiLib::CButtonUI* _pNorButtonMenu;
	DuiLib::CHorizontalLayoutUI* _pNormalLayout;

	DuiLib::CControlUI* _pItemIco;
	DuiLib::CLabelUI* _pPushLabelIndex;
	DuiLib::CLabelUI* _pPushLabelName;
	DuiLib::CLabelUI* _pPushLabelAritst;
	DuiLib::CLabelUI* _pPushLabelAlbum;
	DuiLib::CButtonUI* _pPushDownLoad;
	DuiLib::CCheckBoxUI* _pPushCollect;
	DuiLib::CButtonUI* _pPushShare;
	DuiLib::CButtonUI* _pPushRemove;
	DuiLib::CHorizontalLayoutUI* _pPushLayout;
};

#define SONG_LIST _T("SongList")
typedef DuiLib::CTemplListUI  CSongListUI;

#endif//__YSongListItem_h