#pragma once
#include "Mgr/DB/DB.h"

class CConfigBase
{
public:
	CConfigBase();
	virtual ~CConfigBase();

	virtual bool Load() = 0;
	virtual bool Save() = 0;

protected:
	spUserDBaseT GetDB();
	void UpdateDB();
	bool IsModify() const{return m_bModify;}
	void Modify(bool b = true){m_bModify = b;}
	bool RegSet(HKEY hKey,const DuiLib::CDuiString& cs_subkey,const DuiLib::CDuiString&  cs_value_name,const DuiLib::CDuiString&  cs_value);
		//ע������ã��ȼ���Ƿ���ڣ���������д;
	bool RegSetEx(HKEY hKey,const DuiLib::CDuiString&  cs_subkey,const DuiLib::CDuiString&  cs_value_name,const DuiLib::CDuiString&  cs_value);
	bool RegDelete(const DuiLib::CDuiString&  cs_subkey,const DuiLib::CDuiString&  cs_value_name);
	//ע������ѯ
	DuiLib::CDuiString RegQuery(const DuiLib::CDuiString&  cs_subkey,const DuiLib::CDuiString&  cs_value_name);  
	DuiLib::CDuiString GetAppName();
private:
	spUserDBaseT m_spUserDataBase;
	bool m_bModify;
};

/************************************************************************/
/*                        ��������                                      */
/************************************************************************/
class CNormalConfig : public CConfigBase
{
	static const TCHAR CloseModeKey[];//�رհ�ť��Ϊ
	static const TCHAR PlayModeKey[];//����ģʽ
	static const TCHAR VolumeKey[];//������С
	static const TCHAR VolumeOffKey[];//��������
	static const TCHAR KaraokModeKey[];//����okģʽ
	static const TCHAR AutoLoginKey[];//�Զ���½
	static const TCHAR AutoStartKey[];//��������
	static const TCHAR AutoPlayKey[];//�Զ�����
	static const TCHAR AutoLrcKey[];//�Զ���������
	static const TCHAR InsertModeKey[];//���뷽ʽ
	static const TCHAR PlayRoamingKey[];//�Զ����������б�		
	static const TCHAR ShowInfoKey[];
	static const TCHAR RelateAACKey[];
	static const TCHAR RelateAPEKey[];
	static const TCHAR RelateFLACKey[];
	static const TCHAR RelateMP3Key[];
	static const TCHAR RelateOGGKey[];
	static const TCHAR RelateWAVKey[];

	static const TCHAR EnableUrlProtoKey[];
public:
	CNormalConfig();
	~CNormalConfig();

	bool Load();
	bool Save();
	//bool RegisterFileRelation(const DuiLib::CDuiString&  strExtension,const DuiLib::CDuiString&  strApplicationName,
	//const DuiLib::CDuiString&  strRunKey,const DuiLib::CDuiString&  strDefaultIcon,const DuiLib::CDuiString&  strDescription,bool bRelate = true);
	bool IsCloseToTray();
	void SetCloseMode(bool bToTray = true);
	int GetPlayMode();
	void SetPlayMode(int nMode);
	bool IsVolumeOff();
	void SetVolumeOff(bool bOff = true);
	int GetVolume();
	void SetVolume(int nVol);
	bool IsKaraokMode();
	void SetKaraokMode(bool bKaraok = true);
	bool IsEnableAutoLogin()const;
	void EnableAutoLogin(bool bEnable = true);
	bool IsEnableAutoStart();
	void EnableAutoStart(bool bEnable = true);
	bool IsEnableAutoPlay()const;
	void EnableAutoPlay(bool bEnable = true);
	bool IsEnableAutoOpenLrc()const;
	void EnableAutoOpenLrc(bool bEnable = true);
// 		enum InsertMode
// 		{
// 			im_begin = 0,  //���б�ͷ����;
// 			im_current = 1,//�ӵ�ǰ���Ÿ��������;
// 			im_end = 2       //��ĩβ����;
// 		};
	unsigned int GetInsertMode()const;
	void SetInsertMode(unsigned int nMode);
	bool IsPlayRoaming() const;
	void EnablePlayRoaming(bool enable);
	bool IsEnableAutoShowSongInfo()const;
	void EnableAutoShowSongInfo(bool bEnable = true);
	bool IsEnableRelateAAC()const;
	void EnableRelateAAC(bool bEnable = true);
	bool IsEnableRelateAPE()const;
	void EnableRelateAPE(bool bEnable = true);
	bool IsEnableRelateFLAC()const;
	void EnableRelateFLAC(bool bEnable = true);
	bool IsEnableRelateMP3()const;
	void EnableRelateMP3(bool bEnable = true);
	bool IsEnableRelateOGG()const;
	void EnableRelateOGG(bool bEnable = true);
	bool IsEnableRelateWAV()const;
	void EnableRelateWAV(bool bEnable = true);
	bool IsAllRelate() const;
	void EnableAllRelate(bool bEnable = true);

	bool IsEnableUrlProto() const;
	void EnableUrlProto(bool bEnable = true);
private:
	void SendAgentPacket(int index,std::string& packet);
	void SendRelationPacket(const DuiLib::CDuiString&  strExtension,bool bRelate);
	void SendRunPacket(bool bAutoRun);
	void SendEnableUrlProtoPacket(bool bEnable = true);

private:
	DuiLib::CDuiString m_sAppName;
	bool m_bToTray;
	int m_nVol;
	bool m_bVolOff;
	int m_nPlayMode;
	bool m_bKaraok;
	bool m_bAutoLogin;
	bool m_bAutoStart;
	bool m_bAutoPlay;
	bool m_bAutoLrc;
	unsigned int m_nInsertMode;
	bool m_bShowInfo;
	bool m_bRelateAAC;
	bool m_bRelateAPE;
	bool m_bRelateFLAC;
	bool m_bRelateMP3;
	bool m_bRelateOGG;
	bool m_bRelateWAV;
	bool m_bPlayRoaming;

	bool m_bEnableUrlProto;
};

/************************************************************************/
/*                        ��������                                      */
/************************************************************************/
class CAlertConfig : public CConfigBase
{
	static const TCHAR PrivateMsgKey[];
	static const TCHAR CommentMsgKey[];
	static const TCHAR PraiseMsgKey[];
public:
	CAlertConfig();
	~CAlertConfig();

	bool Load();
	bool Save();
public:
	bool IsEnablePrivateMsg()const;
	void EnablePrivateMsg(bool bEnable = true);

	bool IsEnableCommentMsg()const;
	void EnableCommentMsg(bool bEnable = true);

	bool IsEnablePraiseMsg()const;
	void EnablePraiseMsg(bool bEnable = true);
protected:
	bool          _enablePrivateMsg;
	bool          _enableCommentMsg;
	bool          _enablePraiseMsg;
};
/************************************************************************/
/*                     �ȼ�����                                         */
/************************************************************************/
class CHotKeyConfig:public CConfigBase
{
	static const TCHAR ChangeModeHotKey[];
	static const TCHAR LrcWindowHotKey[];
	static const TCHAR PlayControl[];
	static const TCHAR PlayPreSong[];
	static const TCHAR PlayNextSong[];
	static const TCHAR UpVolKey[];
	static const TCHAR DownVolKey[];
	static const TCHAR MusicLibWindowHotKey[];
public:
	CHotKeyConfig();
	~CHotKeyConfig();

	bool Load();
	bool Save();

	void RegAllHotKey();
	bool RegHotKey(DWORD dwkey);
	//bool UnRegHotKey();

	DWORD GetChangeModeHotKey() const;
	void SetChangeModeHotKey(DWORD dwkey);

	DWORD GetLrcWindowHotKey() const;
	void SetLrcWindowHotKey(DWORD dwkey);

	DWORD GetPlayControlHotKey() const;
	void SetPlayControlHotKey(DWORD dwkey);

	DWORD GetPlayPreHotKey() const;
	void SetPlayPreHotKey(DWORD dwkey);

	DWORD GetPlayNextHotKey() const;
	void SetPlayNextHotKey(DWORD dwkey);

	DWORD GetUpVolHotKey() const;
	void SetUpVolHotKey(DWORD dwkey);

	DWORD GetDownVolHotKey() const;
	void SetDownVolHotKey(DWORD dwkey);

	DWORD GetMusiclibHotKey() const;
	void SetMusiclibHotKey(DWORD dwkey);
private:
	DWORD    _dwChangeModeHotKey;
	DWORD    _dwLrcWindowHotKey;
	DWORD    _dwPlayControlHotKey;
	DWORD    _dwPreSongHotKey;
	DWORD    _dwNextSongHotKey;
	DWORD    _dwUpVolHotKey;
	DWORD    _dwDownVolHotKey;
	DWORD    _dwMusiclibWindowHotKey;
};



/************************************************************************/
/*                        ��������                                      */
/************************************************************************/

class CCacheConfig:public CConfigBase
{
	static const TCHAR CacheDirKey[];
	static const TCHAR MaxCacheKey[];
public:
	CCacheConfig();
	~CCacheConfig();

	bool Load();
	bool Save();

	DuiLib::CDuiString GetCacheDir();
	void SetCacheDir(const DuiLib::CDuiString&  sPath);

	DuiLib::CDuiString GetMaxCacheSpace();
	void SetMaxCacheSpace(const DuiLib::CDuiString& sMax);

private:
	DuiLib::CDuiString m_sCacheDir;
	DuiLib::CDuiString m_sMaxCacheSpace;

};

/************************************************************************/
/*                                   Ƥ������                           */
/************************************************************************/
class CSkinConfig : public CConfigBase
{
	static const TCHAR SkinCorKey[];
	static const TCHAR SkinBgKey[];
	static const TCHAR SkinUnityBgKey[];
	static const TCHAR SkinStyleKey[];
	static const TCHAR SkinItemBgKey[];
	static const TCHAR SkinItemHoverBgKey[];
	static const TCHAR SkinItemPlayBgKey[];
public:
	CSkinConfig();
	~CSkinConfig();

	bool Load();
	bool Save();

	DuiLib::CDuiString GetStyleName() const;
	void SetStyleName(LPCTSTR lpszName);

	DWORD GetColor() const;
	void SetColor(DWORD dwCor);

	DuiLib::CDuiString GetBg() const;
	void SetBg(LPCTSTR lpszName);

	DuiLib::CDuiString GetItemBg() const;
	void SetItemBg(LPCTSTR lpszName); 

	DuiLib::CDuiString GetItemHoverBg() const;
	void SetItemHoverBg(LPCTSTR lpszName);

	DuiLib::CDuiString GetItemPlayBg() const;
	void SetItemPlayBg(LPCTSTR lpszName);

	bool IsEnableUnityBg() const;
	void EnableUnityBg(bool b = true);
private:
	DuiLib::CDuiString m_sStyle;
	DWORD   m_dwCor;
	DuiLib::CDuiString m_sBg;
	bool    m_bEnableUnityBg;

	DuiLib::CDuiString m_sItemBg;
	DuiLib::CDuiString m_sItemHoverBg;
	DuiLib::CDuiString m_sItemPlayBg;
};
	
/************************************************************************/
/*                                   �����������                        */
/************************************************************************/
class CUpdateConfig : public CConfigBase
{
	static const TCHAR AutoUpdateKey[];
public:
	CUpdateConfig();
	~CUpdateConfig();

	bool Load();
	bool Save();
	bool IsAutoUpdate() const;
	void SetAutoUpdate(bool enable);

private:
	bool m_autoUpdate;
};
/************************************************************************/
/*                                   �������                           */
/************************************************************************/
class CLrcConfig : public CConfigBase
{
	static const TCHAR LrcPanelFontNameKey[];
	static const TCHAR LrcPanelFontStyleKey[];
	static const TCHAR LrcPanelFontSizeKey[];

	static const TCHAR LrcPanelPlayedCorKey[];
	static const TCHAR LrcPanelUnPlayedCorKey[];
	static const TCHAR LrcPanelPlayingCorKey[];

	static const TCHAR LrcDeskFontNameKey[];
	static const TCHAR LrcDeskFontStyleKey[];
	static const TCHAR LrcDeskFontSizeKey[];
	static const TCHAR LrcDeskPlayedCorKey[];
	static const TCHAR LrcDeskUnPlayedCorKey[];
public:
	CLrcConfig();
	~CLrcConfig();
	enum LrcFontStyle
	{
		NormalFont = 0,
		BoldFont,
		ItalicFont,
		BoldItalicFont
	};
	bool Load();
	bool Save();

	DuiLib::CDuiString GetLrcPanelFontName() const;
	void SetLrcPanelFontName(LPCTSTR lpszName);

	LrcFontStyle GetLrcPanelFontStyle()  const;
	void SetLrcPanelFontStyle(LrcFontStyle fontStyle);

	DWORD GetLrcPanelFontSize() const;
	void SetLrcPanelFontSize(DWORD fontSize);

	DWORD GetLrcPanelPlayedColor() const;
	void SetLrcPanelPlayedColor(DWORD dwCor);
	DWORD GetLrcPanelUnPlayedColor() const;
	void SetLrcPanelUnPlayedColor(DWORD dwCor);
	DWORD GetLrcPanelPlayingColor() const;
	void SetLrcPanelPlayingColor(DWORD dwCor);


	DuiLib::CDuiString GetLrcDeskFontName() const;
	void SetLrcDeskFontName(LPCTSTR lpszName);

	LrcFontStyle GetLrcDeskFontStyle()  const;
	void SetLrcDeskFontStyle(LrcFontStyle fontStyle);

	DWORD GetLrcDeskFontSize() const;
	void SetLrcDeskFontSize(DWORD fontSize);

	DWORD GetLrcDeskPlayedColor() const;
	void SetLrcDeskPlayedColor(DWORD dwCor);
	DWORD GetLrcDeskUnPlayedColor() const;
	void SetLrcDeskUnPlayedColor(DWORD dwCor);
private:
	DuiLib::CDuiString m_sLrcPanelFontName;
	LrcFontStyle   m_dLrcPanelFontStyle;
	DWORD   m_dLrcPanelFontSize;
	DWORD   m_dLrcPanelPlayedCor;
	DWORD   m_dLrcPanelUnPlayedCor;
	DWORD   m_dLrcPanelPlayingCor;
		
	DuiLib::CDuiString m_sLrcDeskFontName;
	LrcFontStyle   m_dLrcDeskFontStyle;
	DWORD   m_dLrcDeskFontSize;
	DWORD   m_dLrcDeskPlayedCor;
	DWORD   m_dLrcDeskUnPlayedCor;
};
