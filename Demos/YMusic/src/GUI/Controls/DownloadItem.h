#ifndef _DOWNLOADITEM_H
#define _DOWNLOADITEM_H

#pragma once
#include "UIlib.h"
#include "Mgr/DB/ShareData.h"
#include "Utility/ThreadHelper.h"


class DownloadListItemUI 
	: public DuiLib::CListContainerElementUI
	, public em_utility::ThreadEvent
{
public:
	DownloadListItemUI();
	virtual ~DownloadListItemUI();
public:
	static LPCTSTR GetClassName();
	LPCTSTR GetClass() const;

	void SetSong(spSongInfoT song);
	spSongInfoT GetSong() const;

	void InitVar();
	void Update();
	void ResumeLoad();
	void PauseLoad();
protected:
	void OnPrepare();
	void ShowOrHideBtn(bool bShow = true);
	bool OnClick(void* event);
protected:
	static unsigned __stdcall ThreadProc(void *pvArg);
	virtual void _ThreadProc_();
	virtual bool ProcessEvent() ;
	virtual bool RespondEvent() ;
	virtual void Cleanup() ;
	//�������ص�
	static int LoadProgress(void* ptr, double rDlTotal, double rDlNow, double rUlTotal, double rUlNow);

	static DuiLib::CDuiString getSizeString(double dload,double size);
	static DuiLib::CDuiString getRateString(double rate);
private:
	spSongInfoT _spSongInfo;
	bool _bIsDowning;


	DuiLib::CProgressUI* _pProgress;
	DuiLib::CLabelUI*  _pSizeLab;
	DuiLib::CLabelUI*  _pRateLab;
	DuiLib::CLabelUI*  _pTitleLab;

	DuiLib::CButtonUI* _pStart;
	DuiLib::CButtonUI* _pPause;
	DuiLib::CButtonUI* _pRemove;

private:
	void*  m_thread;
	class HTTPprotocol* m_pHttp;
	double m_downloadOld;
	static const int DownloadItemHeight;
};
///////////////////////////////////////////////////
class DownloadFinishedListItemUI
	: public DuiLib::CListContainerElementUI
{
public:
	DownloadFinishedListItemUI();
	virtual ~DownloadFinishedListItemUI();
public:
	static LPCTSTR GetClassName();
	LPCTSTR GetClass() const;

	void SetSong(spSongInfoT song);
	spSongInfoT GetSong() const;
protected:
	void Update();
	void OnPrepare();
	bool Notify(void* param);
private:
	spSongInfoT _spSongInfo;
	DuiLib::CCheckBoxUI * _pCheckBox;
	DuiLib::CLabelUI * _pTitlelab;
	DuiLib::CLabelUI * _pSizelab;
	DuiLib::CButtonUI * _pPlaybtn;
	DuiLib::CButtonUI * _pAddbtn;
	DuiLib::CButtonUI * _pRemovebtn;

	static const int DownloadFinishedItemHeight;
};







#endif//_DOWNLOADITEM_H