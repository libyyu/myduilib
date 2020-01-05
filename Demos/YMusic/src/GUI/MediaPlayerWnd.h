#ifndef __MediaPlayerWnd_H
#define __MediaPlayerWnd_H

#pragma once
#include "BaseWindow.h"
#include "Controls/UIMedia.h"

class CMediaWin :
	public YWindow,
	public DuiLib::INotifyUI,
	public SkinChangedReceiver,
	public DuiLib::IMessageFilterUI,
	public DuiLib::Singleton<CMediaWin>
{
public:

	CMediaWin();
	~CMediaWin();
	static LPCTSTR GetWindowClass();
	LPCTSTR GetWindowClassName() const;
	static CMediaWin* GetMediaPlayer() ;

	void ReadyToPlay(LPCTSTR pstrName);

	bool Play();
	void Pause();
	void Resume();
	void Stop();
	void FullScreen(bool bFull = true);
	void SetVolume(int nVal);
	void SwitchVol(bool bOn = true);
	void SetTimeText(LPCTSTR szText);
	void SetProgress(int nVal,bool bShowLayout = true);
	void ChangeBtnStatus(bool bPlaying = true);
protected:
	virtual void OnFinalMessage( HWND hWnd );
	virtual DuiLib::CControlUI* CreateControl(LPCTSTR pstrClassName);
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual void Notify(DuiLib::TNotifyUI& msg);
	
	void OnClick(const DuiLib::TNotifyUI& msg);

	bool    OnPosChanged(void* param);                              // ���ȸı䣬�û������ı����
	bool    OnVolumeChanged(void* param);                           // �����ı�

protected:
	LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
	virtual BOOL Receive(SkinChangedParam param);
	virtual LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnWebService(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual void    OnWebService(void *pRequest ,void *pProto);
	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnNcHitTest( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	virtual LRESULT ProcessWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, bool& /*bHandled*/);
	LRESULT OnPlayerPlaying(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPlayerPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPlayerEndReached(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
protected:
	void UpdatePlayingProgress();

private:
	struct ui{
		DuiLib::CLabelUI* m_pTitle;
		DuiLib::CButtonUI* m_pMaxBtn;
		DuiLib::CButtonUI* m_pResBtn;
		CMediaUI* m_pMediaPlayer;
		DuiLib::CHorizontalLayoutUI* m_pSliLayout;
		DuiLib::CSliderUI* m_pProgressSli;
		DuiLib::CButtonUI* m_pBackWard;
		DuiLib::CButtonUI* m_pForWard;
		DuiLib::CLabelUI* m_pTimeUse;
		//	DuiLib::CLabelUI* m_pTimeAll;
		DuiLib::CButtonUI* m_pPlayBtn;
		DuiLib::CButtonUI* m_pPauseBtn;
		DuiLib::CButtonUI* m_pPrevBtn;
		DuiLib::CButtonUI* m_pNextBtn;
		DuiLib::CButtonUI* m_pStopBtn;
		DuiLib::CButtonUI* m_pVolumeOffBtn;
		DuiLib::CButtonUI* m_pVolumeOnBtn;
		DuiLib::CSliderUI* m_pVolumeSli;
		DuiLib::CButtonUI* m_pFullScreenBtn;
		DuiLib::CButtonUI* m_pNormalBtn;
	};
	ui _ui;

	int m_nVol;
	bool m_bVolOn;
	std::string m_strCurPlay;
	bool m_bFullScreenMode;
	WINDOWPLACEMENT m_OldWndPlacement;  // ���洰��ԭ����λ��
private:
	DECLARE_SINGLETON_CLASS(CMediaWin);
};


#endif//__MediaPlayerWnd_H