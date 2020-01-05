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
		DuiLib::CFadeOptionUI *ConfigAbout_Option;//�������
		DuiLib::CFadeOptionUI *ConfigNormal_Option;//��������
		DuiLib::CFadeOptionUI *ConfigLrcPanel_Option;//�����ʾ
		DuiLib::CFadeOptionUI *ConfigLrcDesk_Option;//������
		DuiLib::CFadeOptionUI *ConfigCache_Option;//��������
		DuiLib::CFadeOptionUI *ConfigHotKey_Option;//�ȼ�����
		DuiLib::CFadeOptionUI *ConfigUpdate_Option;//�������
		//��������ҳ��
		DuiLib::CCheckBoxUI *AutoLogin_CheckBox;//�Զ���¼�˻�
		DuiLib::CCheckBoxUI *AutoPlay_CheckBox;//�Զ�����
		DuiLib::CCheckBoxUI *AutoRun_CheckBox;//��������
		DuiLib::CCheckBoxUI *AutoOpenLRC_CheckBox;//�Զ���ʾ������
		DuiLib::COptionUI *MinTrayIcon_Option;
		DuiLib::COptionUI *Exit_Option;

		DuiLib::CCheckBoxUI *PlayRoaming_CheckBox;//�����б����ɺ���������
		DuiLib::CCheckBoxUI *RelateFile_CheckBox;//�ļ�����
		//�����ʾ
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
		//������
		//��������ҳ��
		DuiLib::CEditUI *CacheDir_Edit;
		DuiLib::CEditUI *MaxCache_Edit;
		DuiLib::CLabelUI *CachedLabel;
		DuiLib::CButtonUI *ChangeDir_Btn;
		DuiLib::CButtonUI *OpenDir_Btn;
		DuiLib::CButtonUI *ClearCache_Btn;
		//�������
		DuiLib::CCheckBoxUI *AutoUpdate_CheckBox;

		//�ȼ�����ҳ��
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

