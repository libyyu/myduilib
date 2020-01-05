#ifndef CHATDIALOG_HPP
#define CHATDIALOG_HPP

#include "skin_change_event.hpp"
#include "Msg.h"
#include "lock.h"

#include "FaceList.h"

class CFontInfo				// ×ÖÌåÐÅÏ¢
{
public:
	CFontInfo(void);
	~CFontInfo(void);

public:
	int m_nSize;			// ×ÖÌå´óÐ¡
	COLORREF m_clrText;		// ×ÖÌåÑÕÉ«
	CDuiString m_strName;		// ×ÖÌåÃû³Æ
	BOOL m_bBold;			// ÊÇ·ñ¼Ó´Ö
	BOOL m_bItalic;			// ÊÇ·ñÇãÐ±
	BOOL m_bUnderLine;		// ÊÇ·ñ´øÏÂ»®Ïß
};

class ChatDialog 
	: public CWin
	, public INotifyUI
	, public IDialogBuilderCallback
	, public IMessageFilterUI
	, public SkinChangedReceiver
{
public:

	ChatDialog(const CDuiString& bgimage, DWORD bkcolor, const User& myselft_info, const User& friend_info);
	~ChatDialog();
protected:
	virtual CDuiString GetSkinPath();
	virtual CDuiString GetSkinFolder();
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual CDuiString GetZIPFileName() const;
public:

	LPCTSTR GetWindowClassName() const;	

	virtual void InitWindow();

	void OnPrepare(TNotifyUI& msg);
	
	virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam);

	virtual CControlUI* CreateControl(LPCTSTR pstrClass);

	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM , bool& bHandled);

	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam,BOOL& bHandled);

//	virtual LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT ProcessWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnFaceSelected(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRecvChatMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	BOOL _RichEdit_InsertFace(CRichEditUI* pRichEdit,
		LPCTSTR lpszFileName, int nFaceId, int nFaceIndex,
		COLORREF clrBg, BOOL bAutoScale, int nReservedWidth, int nIndent = 0);

	void _RichEdit_ReplaceSel(CRichEditUI * pRichEdit, LPCTSTR lpszNewText, BOOL bIsLink,
		int nStartIndent, BOOL bCanUndo = FALSE);

	void _RichEdit_GetText(CRichEditUI* pRichEdit, CDuiString& s_text);

	void AddMsgTitle(LPCTSTR lpName, LPCTSTR lpTime);
	void AddMsgToRecvEdit(LPCTSTR lpText);

	void SetTextColor(DWORD dwColor);

	void SendMsg();

	virtual BOOL Receive(SkinChangedParam param);

	void ShowMsg(CMsg* pMsg);

	void FormatMsgToEditView(CDuiString sName,CDuiString sTime,CDuiString sMsg);
protected:	

	void Notify(TNotifyUI& msg);
	void OnExit(TNotifyUI& msg);
	void OnTimer(TNotifyUI& msg);
	void OnTransFile(TNotifyUI& msg);
private:
	void FontStyleChanged();
	void OnPaste(LPCTSTR lpText);
private:

	CRichEditUI* m_pMsgView;
	CRichEditUI* m_pInput;

	bool emotion_timer_start_;

	bool bold_;
	bool italic_;
	bool underline_;
	DWORD text_color_;
	DWORD font_size_;
	CDuiString font_face_name_;

	CDuiString bgimage_;
	DWORD bkcolor_;
	User myselft_;
	User friend_;

	CFaceList* m_lpFaceList;
	CFontInfo m_fontInfo;
};

extern CPtrList theChatDialogList;
extern CLock    thelockChatDlgList;

//extern std::map<CDuiString sWhoSnd,std::vector<CMsg*> > theMsgQueue;
extern std::map<CDuiString,std::vector<CMsg*> > theMsgQueue;
extern CLock theLockMsgQue;
#endif // CHARTDIALOG_HPP