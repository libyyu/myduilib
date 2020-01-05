#pragma once

#include "BaseWindow.h"
#include "MyControls/MyControls.h"

class CSettingWindow : 
	public YWindow ,
	public DuiLib::INotifyUI ,
	public DuiLib::Singleton<CSettingWindow>
{
	DECLARE_SINGLETON_CLASS(CSettingWindow)
public:
	CSettingWindow();
	~CSettingWindow();
	
protected:
	virtual void Notify(DuiLib::TNotifyUI& msg);

	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//void OnPaint(CDCHandle dc);
	//void OnShowWindow(BOOL bShow, UINT nStatus);
	/*virtual*/ DuiLib::CControlUI* CreateControl(LPCTSTR pstrClass);
public:
	enum emTabPages
	{
		AboutPage,
		NormalPage,
		LrcPanelPage,
		LrcDeskPage,
		CachePage,
		HotKeyPage,
		UpdataPage,

		PageCount
	};

	enum emInsertModes
	{
		BeginPosMode,
		CurPosMode,
		EndPosMode
	};
	void showPage(emTabPages emPage);
private:
	struct
	{
		DuiLib::CFadeOptionUI *ConfigAbout_Option;//软件关于
		DuiLib::CFadeOptionUI *ConfigNormal_Option;//常规设置
		DuiLib::CFadeOptionUI *ConfigLrcPanel_Option;//歌词显示
		DuiLib::CFadeOptionUI *ConfigLrcDesk_Option;//桌面歌词
		DuiLib::CFadeOptionUI *ConfigCache_Option;//缓存设置
		DuiLib::CFadeOptionUI *ConfigHotKey_Option;//热键设置
		DuiLib::CFadeOptionUI *ConfigUpdate_Option;//软件更新
		//常规设置页面
		DuiLib::CCheckBoxUI *AutoLogin_CheckBox;//自动登录账户
		DuiLib::CCheckBoxUI *AutoPlay_CheckBox;//自动播放
		DuiLib::CCheckBoxUI *AutoRun_CheckBox;//开机启动
		DuiLib::CCheckBoxUI *AutoOpenLRC_CheckBox;//自动显示桌面歌词
		DuiLib::COptionUI *MinTrayIcon_Option;
		DuiLib::COptionUI *Exit_Option;

		DuiLib::CCheckBoxUI *PlayRoaming_CheckBox;//漫游列表生成后立即播放
		DuiLib::CCheckBoxUI *RelateFile_CheckBox;//文件关联
		//歌词显示
		DuiLib::CButtonUI *ColorPicker_Btn1;
		DuiLib::CButtonUI *ColorPicker_Btn2;
		DuiLib::CButtonUI *ColorPicker_Btn3;
		DuiLib::CButtonUI *ColorPicker_Btn4;
		DuiLib::CButtonUI *ColorPicker_Btn5;
		DuiLib::CLabelUI *Color_Label1;
		DuiLib::CLabelUI *Color_Label2;
		DuiLib::CLabelUI *Color_Label3;
		DuiLib::CLabelUI *Color_Label4;
		DuiLib::CLabelUI *Color_Label5;
		DuiLib::CComboUI*LrcPanelFontName;
		DuiLib::CComboUI*LrcPanelFontStyle;
		DuiLib::CComboUI*LrcPanelFontSize;
		DuiLib::CComboUI*LrcDeskFontName;
		DuiLib::CComboUI*LrcDeskFontStyle;
		DuiLib::CComboUI*LrcDeskFontSize;
		DuiLib::CCheckBoxUI *LrcDeskKaraok;
		//桌面歌词
		//缓存设置页面
		DuiLib::CEditUI *CacheDir_Edit;
		DuiLib::CEditUI *MaxCache_Edit;
		DuiLib::CLabelUI *CachedLabel;
		DuiLib::CButtonUI *ChangeDir_Btn;
		DuiLib::CButtonUI *OpenDir_Btn;
		DuiLib::CButtonUI *ClearCache_Btn;
		//软件更新
		DuiLib::CCheckBoxUI *AutoUpdate_CheckBox;

		//热键设置页面
		DuiLib::CTabLayoutUI *PanelItemTab_TabLayout;
		DuiLib::CButtonUI *CheckUpdate_Btn;
		DuiLib::CHotKeyUI *HK_Mode;
		DuiLib::CHotKeyUI *HK_Lrc;
		DuiLib::CHotKeyUI *HK_PlayPause;
		DuiLib::CHotKeyUI *HK_PreSong;
		DuiLib::CHotKeyUI *HK_NextSong;
		DuiLib::CHotKeyUI *HK_VolUp;
		DuiLib::CHotKeyUI *HK_VolDown;

		DuiLib::CButtonUI *Close_Btn;
		DuiLib::CButtonUI *Ok_Btn;
		DuiLib::CButtonUI *Cancle_Btn;
		DuiLib::CButtonUI *Apply_Btn;
	}ui;
	bool m_bModify;

	void init();
	void ok();
	void cancle();
	void apply();
	void setModify(bool isModify);
	void initUiStruct();
	void colorBox(DuiLib::CControlUI* pUI);
	DWORD dwFromText(LPCTSTR lpszValue);
	int   intFromText(LPCTSTR lpszValue, DuiLib::CComboUI* pCombo);
};

