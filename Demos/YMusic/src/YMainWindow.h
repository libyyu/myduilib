#ifndef __YMainWindow_h
#define __YMainWindow_h

#pragma once
#include "GUI/BaseWindow.h"
#include "GUI/MusicLib.h"
#include "GUI/DesktopLrcWindow.h"
#include "Utility/win_io/FileMgr.h"
#include "GUI/trayIcon.h"
#include <atlcomcli.h>
#include <ShlObj_core.h>
#include <vlc/src/AVPlayer.h>
#include "GUI/Controls/LrcView.h"
#include "GUI/Controls/YPlayListItem.h"
#include "GUI/Controls/YSongListItem.h"

class YMainWindow
	: public YWindow
	, public DuiLib::INotifyUI
	, public IAVPlayerCallBack
{
public:
	YMainWindow();
	virtual ~YMainWindow();

public:
	static LPCTSTR GetWindowClsss() ;
	LPCTSTR GetWindowClassName() const;
	UINT GetClassStyle() const;
	void OnFinalMessage( HWND hWnd );
	DuiLib::CControlUI* CreateControl(LPCTSTR pstrClass);
	void Notify(DuiLib::TNotifyUI& msg);
	void OnValueChanged(const DuiLib::TNotifyUI& msg);
	void OnItemActivate(const DuiLib::TNotifyUI& msg);
	void OnItemSelect(const DuiLib::TNotifyUI& msg);
	void OnSelectChanged(const DuiLib::TNotifyUI& msg);
	void OnClick(const DuiLib::TNotifyUI& msg);
	void OnMenu(const DuiLib::TNotifyUI& msg);
	void OnBlur(const DuiLib::TNotifyUI& msg);
	void OnReturn(const DuiLib::TNotifyUI& msg);
	void OnTimer(const DuiLib::TNotifyUI& msg);
	void OnPlayListMenu(const DuiLib::TNotifyUI& msg);
	void OnSongListMenu(const DuiLib::TNotifyUI& msg);
	void OnSongItemMenu(const DuiLib::TNotifyUI& msg);
	bool OnVolumeLayoutUIEvent(void* pParam);
	void OnShowPlayModeMenu();
	void OnShowMainMenu();
	void OnShowMiniMode();
	void OnShowSkinWindow();
	void OnShowLoginWindow();
	void OnShowShareWindow();
	void OnGoNextLayout(const DuiLib::TNotifyUI& msg);
	void OnGoBackLayout(const DuiLib::TNotifyUI& msg);
	void OnGoDeskLayout(const DuiLib::TNotifyUI& msg);
	void OnShowLrcViewMenu(const DuiLib::TNotifyUI& msg);
	void SendNotify(LPCTSTR lpControl,LPCTSTR sType);
public:
	void ShowTrayInfo(spSongInfoT spSong);
	void ShowTrayTooltip(TCHAR* szTip,TCHAR* szTitle);
	void ShowTrayMenu();
	void TryAutoLogin();
	void AddTracyIcon();
	void PlayOrPause(bool bPlay = true);
	void NextSong();
	void PrevSong();
	void Stop();
	void Play();
	void Pause();
	
	virtual void OnPlaying(CAVPlayer* player, void* event);         // 设置文件头读取完毕时的回调函数
	virtual void OnPosChanged(CAVPlayer* player, void* event);      // 设置文件位置改变时的回调函数
	virtual void OnEndReached(CAVPlayer* player, void* event);      // 设置文件头读取完毕时的回调函数

	int ShowDetailAddingInfo(CFileMgr::filesArrayT files,void* param);
protected:
	void InitVariable();
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMenuCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnHotKey(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMoving(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnSizing(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnTaskbarCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnTaskbarButtonCreate(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL& bHandled);
	virtual	LRESULT OnTrayNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnWebService(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT ProcessWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnPlayerPlaying(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnPlayerPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnPlayerEndReached(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnUserLogin(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnHttpfileDownload(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void CallbackPlayer(void *data, UINT uMsg);
/*protected:*/
public:
	void OnPlayListChanged(/*CNotifyCenter::NotifyT& notify*/);
	void OnCurPlayListChanged(/*CNotifyCenter::NotifyT& notify*/);
	void OnPlayListSongChanged(/*CNotifyCenter::NotifyT& notify*/);
	void OnNetChanged(/*CNotifyCenter::NotifyT& notify*/);
	void OnUserChanged(/*CNotifyCenter::NotifyT& notify*/);
	void OnVersionExpired(/*CNotifyCenter::NotifyT& notify*/);		
	void OnSkinChanged(/*CNotifyCenter::NotifyT& notify*/);
	void OnSongStatusChanged(bool bPlay = true);

	void UpdateDowningProgress();
	void UpdatePlayingProgress();
	void UpdateBufferingProgress();
	void ShowSongInfo(spSongInfoT song);
	void UpdateUserName();
	void UpdatePlayList();
	void UpdateSongList(int nPlayListId);
	void UpdateFavList();//收藏列表
	void CreateAddSongThread(bool bOpenFile);
	static DWORD WINAPI _ThreadProc_AddSong(void* param);
	void _AddSongProc(void* param);
	void _AddSongProc2(unsigned int u_listId,CFileMgr::filesArrayT files);

	int OnAddPlayList();//添加列表
	void OnOpenProperty(spSongInfoT spSong);//查看属性
	void OnOpenFolderPath(LPCTSTR sPath);//打开所在目录
	void ShowInfo(const DuiLib::CDuiString& sInfo);//操作提示
	int OnMenuCommandPlayList(DuiLib::MenuCmdMsg* pMsg);
	int OnMenuCommandSongList(DuiLib::MenuCmdMsg* pMsg);
	int OnMenuCommadnPlayMode(DuiLib::MenuCmdMsg* pMsg);
	int OnMenuCommandSongItem(DuiLib::MenuCmdMsg* pMsg);
	int OnMenuCommandLrcView(DuiLib::MenuCmdMsg* pMsg);
	int OnMenuCommandMainMenu(DuiLib::MenuCmdMsg* pMsg);
	void OnRemoveSongFromList(unsigned int nPlayListID,unsigned int nSongIndex);
	void OnRemoveSongsFromList(unsigned int nPlayListID);
	void OnCollectSongFromList(unsigned int nPlayListID,unsigned int nSongIndex);
	void OnCollectSongsFromList(unsigned int nPlayListID);
	void OnShareSongFromList(unsigned int nPlayListID,unsigned int nSongIndex);
	void OnShareSongsFromList(unsigned int nPlayListID);
	void OnDownLoadSongFromList(unsigned int nPlayListID,unsigned int nSongIndex);
	void OnDownLoadSongsFromList(unsigned int nPlayListID);
	void OnAddSongtoNewList(unsigned int nSongIndex,unsigned int nPlayListID,unsigned int nNewListID,bool bMove = false);
	void OnAddSongstoNewList(unsigned int nPlayListID,unsigned int nNewListID,bool bMove = false);
	void OnRemoveDumplicate(unsigned int nPlayListID); // 去重
public:
	typedef CComPtr<ITaskbarList4>        spTaskbarListT;
public:
	spTaskbarListT GetTaskbarList() const;
	YMusicLibWnd* GetMusicLibWin() const;	
	CDesktopLrcWindow*  GetDesktopLrcWin()const;
	YMusicLibWnd* GetMusicLibWin2() const;	
	CDesktopLrcWindow*  GetDesktopLrcWin2()const;
	DuiLib::CPaintManagerUI* GetPaintMgr2() const { return GetPaintMgr(); }

	void ShowMusicLibWin(BOOL bShow = TRUE);
	void ShowDesktopLrcWin(BOOL bShow = TRUE);
	void ShowPanelLrcTab(BOOL bShow = TRUE);
	void ShowSelectLrcWin();
	void ShowSettingWin();
	void ShowSkinWin();
	void LoadSongLrc();
	void SetBkColor(DWORD dwCor);
	void SetBkImage(LPCTSTR szImage);
	void AddReceive(SkinChangedReceiver* win);
	void BroadCast();

	void ShowInfoWindow(POINT rt, LPCTSTR pszText);
	void ShowInfoWindow(RECT rt, LPCTSTR pszText);
private:
	DECLARE_SINGLETON_CLASS(YMainWindow);
private:
	typedef struct uiCompent
	{
		CPlayListUI* _pPlayList;
		CSongListUI* _pSongList;
		DuiLib::CTabLayoutUI *_pTabLayout;
		DuiLib::CFadeCheckBoxUI* _pVolumeBtn;
		DuiLib::CSliderUI* _pVolumeSlider;
		DuiLib::CTabLayoutUI* _pVolumeTab;
		DuiLib::CLabelUI* _pTimeUsed;
		DuiLib::CLabelUI* _pTimeTotal;
		DuiLib::CSliderUI* _pPlayProgress;
		DuiLib::CLabelUI* _pSongName;
		DuiLib::CLabelUI* _pAlbumName;
		DuiLib::CFadeCheckBoxUI* _pCollect;
		DuiLib::CFadeButtonUI* _pShare;
		DuiLib::CHorizontalLayoutUI* _pInfoLayout;
		DuiLib::CLabelUI* _pInfoLabel;
		DuiLib::CLabelUI* _pCountLabel;
		DuiLib::CTabLayoutUI *_pLrcViewTab;
		CMyLrcViewUI* _pLrcView1;
	}_UICompent;
	uiCompent  _ui;
	bool _bOpenFile;

	HICON                _hOnlineIcon;
	HICON                _hOfflineIcon;
	DuiLib::CTrayIconController  _trayIcon;
	spTaskbarListT       _pTaskbarList;

	YMusicLibWnd*          m_pMusicLibWnd;	
	CDesktopLrcWindow*     m_pDesktopLrcWnd;

	UINT _uTaskbarButtonCreateMsg;
	UINT _uTaskbarCreatedMsg;

	SkinChangedObserver skin_changed_observer_;
};





#endif//__YMainWindow_h