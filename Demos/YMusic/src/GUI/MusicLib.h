#ifndef __MUSICLIB_H
#define __MUSICLIB_H

#pragma once
#include "BaseWindow.h"
#include "MyControls/MyControls.h"
#include "Mgr/DB/ShareData.h"
/*
 *���ֿ⣺��������������ĸ���
 */
class CSimpleJsHandler;
class YMusicLibWnd 
	: public YWindow
	, public DuiLib::INotifyUI
	, public SkinChangedReceiver
	, public DuiLib::CNotifyPump
	, public DuiLib::Singleton<YMusicLibWnd>
{
public:
	YMusicLibWnd();
	virtual ~YMusicLibWnd();
	static LPCTSTR GetWindowClass();
	LPCTSTR GetWindowClassName() const;
public:
	void InitJsHandler(CSimpleJsHandler* h);
public:
	void         InitWindow();
	virtual void Notify(DuiLib::TNotifyUI& msg);
	
	void         TransToURL(int iIndex,LPCTSTR url);

	void		 Downloading(spSongInfoT spSong);
	void		 Downloaded(spSongInfoT spSong);
protected:
	virtual BOOL Receive(SkinChangedParam param);
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnHttpfileDownload(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT ProcessWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void         OnClick(DuiLib::TNotifyUI& msg);
	void         OnSelectChange(DuiLib::TNotifyUI& msg);
	void         OnItemSelect(DuiLib::TNotifyUI& msg);
	void         OnMenu(DuiLib::TNotifyUI& msg);

	void		 OnRefreshBrowser();
	void         OnGoBack();
	void         OnSearch();
	//�ղ�;
	void OnShowFavSongs();
	//��ʷ
	void OnShowHisSongs();
	//����
	void OnShowDownloading();
	void OnShowDownloaded();
protected:
	static void OnMsgBox(CSimpleJsHandler* h,LPCTSTR param,DuiLib::CDuiString& sResult);
	static void OnPlay(CSimpleJsHandler* h,LPCTSTR param,DuiLib::CDuiString& sResult);
	static void OnPlayMv(CSimpleJsHandler* h,LPCTSTR param,DuiLib::CDuiString& sResult);
	static void OnShare(CSimpleJsHandler* h,LPCTSTR param,DuiLib::CDuiString& sResult);
	static void OnAdd(CSimpleJsHandler* h,LPCTSTR param,DuiLib::CDuiString& sResult);
	static void OnDownload(CSimpleJsHandler* h,LPCTSTR param,DuiLib::CDuiString& sResult);
private:
	DECLARE_SINGLETON_CLASS(YMusicLibWnd);
private:
	CSimpleJsHandler*    _pUserJsHandler;
	CSimpleJsHandler*    _pXiaMiJsHandler;

	DuiLib::CWebBrowserUI*       _pCurBrowser;
	struct ui
	{
		DuiLib::CFadeButtonUI* m_pButtonBack;
		DuiLib::CFadeButtonUI* m_pButtonRefresh;

		DuiLib::COptionUI* m_pOptMyMusicLib;
		DuiLib::COptionUI* m_pOptFindMusic;

		DuiLib::CFadeButtonUI* m_pButtonClose;

		DuiLib::CTabLayoutUI* m_pTabSwitch;
		DuiLib::CTabLayoutUI* m_pDownloadTab;
		DuiLib::CListUI* m_pDownloadingList;
		DuiLib::CListUI* m_pDownloadedList;

		DuiLib::CTreeViewUI* m_pTree;
		DuiLib::CTreeNodeUI* m_pTreeIListenlist;
		DuiLib::CTreeNodeUI* m_pTreeFavNode;
		DuiLib::CTreeNodeUI* m_pTreeHistory;
		DuiLib::CTreeNodeUI* m_pTreeIDownload;
		DuiLib::CTreeNodeUI* m_pTreeDownloading;
		DuiLib::CTreeNodeUI* m_pTreeDownloaded;

		DuiLib::CWebBrowserUI* m_pWebUserIe;
		DuiLib::CWebBrowserUI* m_pWebXiamiIe;

		DuiLib::CEditUI* m_pEditContent;
		DuiLib::CFadeButtonUI* m_pButtonSearch;
	};
	struct ui  ui_;
	DuiLib::CDuiString bgimage_;
	DWORD bkcolor_;
public:
	ui getUI() const
	{
		return ui_;
	}
};






#endif//__MUSICLIB_H