#include "stdafx.h"
#include <windows.h>
#include <shellapi.h>

#include "chat_dialog.h"
#include "ColorPicker.h"
#include "mainwindow.h"
#include "RichEditUtil.h"
#include "FaceSelDlg.h"
#include "json/DataParser.h"

const TCHAR* const kTitleControlName = _T("apptitle");
const TCHAR* const kCloseButtonControlName = _T("closebtn");
const TCHAR* const kMinButtonControlName = _T("minbtn");
const TCHAR* const kMaxButtonControlName = _T("maxbtn");
const TCHAR* const kRestoreButtonControlName = _T("restorebtn");

const TCHAR* const kBackgroundControlName = _T("bg");

const TCHAR* const kLogoButtonControlName = _T("logo");
const TCHAR* const kNickNameControlName = _T("nickname");
const TCHAR* const kDescriptionControlName = _T("description");

const TCHAR* const kFontButtonControlName = _T("fontbtn");
const TCHAR* const kFontbarControlName = _T("fontbar");
const TCHAR* const kFontTypeControlName = _T("font_type");
const TCHAR* const kFontSizeControlName = _T("font_size");
const TCHAR* const kBoldButtonControlName = _T("boldbtn");
const TCHAR* const kItalicButtonControlName = _T("italicbtn");
const TCHAR* const KUnderlineButtonControlName = _T("underlinebtn");
const TCHAR* const kColorButtonControlName = _T("colorbtn");

const TCHAR* const kInputRichEditControlName = _T("input_richedit");
const TCHAR* const kViewRichEditControlName = _T("view_richedit");

const TCHAR* const kEmotionButtonControlName = _T("emotionbtn");
const TCHAR* const kPICButtonControlName = _T("imagebtn");
const TCHAR* const kSendButtonControlName = _T("sendbtn");

#define RECV_MSG_ID   WM_USER + 1024

const int kEmotionRefreshTimerId = 1001;
const int kEmotionRefreshInterval = 150;
const dui_byte IMG_BEGIN = '<';// 0x20;
const dui_byte IMG_END = '>';// 0x25;


CPtrList theChatDialogList;
CLock    thelockChatDlgList;

std::map<CDuiString,std::vector<CMsg*> > theMsgQueue;
CLock theLockMsgQue;

CFontInfo::CFontInfo(void)
{
	m_nSize = 9;
	m_clrText = RGB(0, 0, 0);
	m_strName = _T("宋体");
	m_bBold = FALSE;
	m_bItalic = FALSE;
	m_bUnderLine = FALSE;
}

CFontInfo::~CFontInfo(void)
{

}
class RichEdit_ImageStringHandleImp : public DuiLib::IRichEdit_ImageStringHandle
{
public:
	void _Replace(CDuiString& strText, const TCHAR * lpOldStr, const TCHAR * lpNewStr)
	{
		if (NULL == lpOldStr || NULL == lpNewStr)
			return;

		strText.Replace(lpOldStr, lpNewStr);
	}
	virtual void MakeImageString(ImageString* pImageInfo, CDuiString& s_out)
	{
		if (NULL == pImageInfo)
		{
			return;
		}
		if (pImageInfo->nId > 0)
		{
			s_out += (TCHAR)IMG_BEGIN;
			TCHAR cText[2048] = { 0 };
			_stprintf(cText, _T("{img:[{id:%d,name:%s,tip:%s}]}"), pImageInfo->nId, pImageInfo->sName.GetData(), pImageInfo->sTip.GetData());
			s_out += cText;
			s_out += (TCHAR)IMG_END;
		}
		else if (pImageInfo->nIndex > 0)
		{
			s_out += (TCHAR)IMG_BEGIN;
			TCHAR cText[2048] = { 0 };
			_stprintf(cText, _T("{img:[{index:%d,name:%s,tip:%s}]}"), pImageInfo->nIndex, pImageInfo->sName.GetData(), pImageInfo->sTip.GetData());
			s_out += cText;
			s_out += (TCHAR)IMG_END;
		}
		else if (!pImageInfo->sFullPath.IsEmpty())
		{
			if (pImageInfo->bUrl)
			{
				s_out += (TCHAR)IMG_BEGIN;
				TCHAR cText[2048] = { 0 };
				_stprintf(cText, _T("{img:[{url:%s,name:%s,tip:%s}]}"), pImageInfo->sFullPath, pImageInfo->sName.GetData(), pImageInfo->sTip.GetData());
				s_out += cText;
				s_out += (TCHAR)IMG_END;
			}
			else
			{
				s_out += (TCHAR)IMG_BEGIN;
				TCHAR cText[2048] = { 0 };
				_stprintf(cText, _T("{img:[{file:%s,name:%s,tip:%s}]}"), pImageInfo->sFullPath, pImageInfo->sName.GetData(), pImageInfo->sTip.GetData());
				s_out += cText;
				s_out += (TCHAR)IMG_END;
			}
		}
	}
	virtual void MakeImageStringById(dui_uint nId, CDuiString& s_out)
	{
		s_out += (TCHAR)IMG_BEGIN;
		TCHAR cText[2048] = { 0 };
		_stprintf(cText, _T("{\'img\':[{\'id\':%d}]}"), nId);
		s_out += cText;
		s_out += (TCHAR)IMG_END;
	}
	virtual void MakeImageStringByFile(CDuiString& sFile, CDuiString& s_out)
	{
		_Replace(sFile, _T("\\"), _T("/"));
		s_out += (TCHAR)IMG_BEGIN;
		TCHAR cText[2048] = { 0 };
		_stprintf(cText, _T("{\'img\':[{\'file\':\'%s\'}]}"), sFile.GetData());
		s_out += cText;
		s_out += (TCHAR)IMG_END;
	}
	virtual void MakeImageStringByUrl(CDuiString& sUrl, CDuiString& s_out)
	{
		s_out += (TCHAR)IMG_BEGIN;
		TCHAR cText[2048] = { 0 };
		_stprintf(cText, _T("{\'img\':[{\'url\':\'%s\'}]}"), sUrl.GetData());
		s_out += cText;
		s_out += (TCHAR)IMG_END;
	}
	virtual void MakeNormalString(CDuiString& s_in, CDuiString& s_out)
	{
		s_out.Append(s_in);
	}
	virtual void PraseImageStringS(CDuiString& s_in, std::vector<RichEdit_Text>& t_out)
	{
		t_out.clear();
		size_t pos = 0;
		size_t beg = 0;
		CDuiString s_normal;
		for (; pos < s_in.GetLength();)
		{
			TCHAR ch = s_in[pos];
			if ((dui_byte)ch == IMG_BEGIN)
			{
				s_normal = s_in.Mid(beg, pos - beg);
				beg = pos;
				pos++;
				continue;
			}
			else if ((dui_byte)ch == IMG_END)
			{
				RichEdit_Text t;
				if (!s_normal.IsEmpty())
				{
					t.bImage = false;
					t.sText = s_normal;
					t_out.push_back(t);
				}

				t.bImage = true;				t.sText = s_in.Mid(beg + 1, pos - beg - 1);
				t_out.push_back(t);

				pos++;
				beg = pos;
				continue;
			}
			else
			{
				pos++;
			}
		}
		if (beg < s_in.GetLength())
		{
			RichEdit_Text t;
			t.bImage = false;
			t.sText = s_in.Mid(beg);
			t_out.push_back(t);
		}
	}
	virtual bool PraseImageString(CDuiString& s_in, ImageString* p_out)
	{
		if (NULL == p_out)
			return false;
		Json::Reader reader;
		Json::Value value;
		CDuiString strJson = s_in;
		_Replace(strJson, _T("\'"), _T("\""));
		_Replace(strJson, _T("\t"), _T(""));

		if (!reader.parse(TO_STDSTRING(strJson), value) || value.isNull()) return false;


		Json::Value images = JsonParser::getValue<Json::Value>(value, "img");
		if (images.isNull()) return false;
		Json::Value image = JsonParser::getValue<Json::Value>(images, 0);
		int nId = JsonParser::getValue<int>(image, "id");
		std::string file = JsonParser::getValue<std::string>(image, "file");
		p_out->nId = nId;
		p_out->sFullPath.Assign(TO_DUISTRING(file.c_str()));
		return true;
	}
};

struct MsgText
{
	static dui_uint nIndex;
	TCHAR sText[2048];
};
dui_uint MsgText::nIndex = -1;
MsgText* NewMsgText(CDuiString& s_in)
{
	MsgText* pMsg = new MsgText;
	pMsg->nIndex = MsgText::nIndex + 1;
	memset(pMsg->sText, 0, 2048);
	_stprintf(pMsg->sText, _T("%s"), s_in.GetData());

	return pMsg;
}

void DelMsgText(MsgText* pMsg)
{
	if (pMsg)
	{
		delete pMsg;
	}
}

ChatDialog::ChatDialog(const CDuiString& bgimage, DWORD bkcolor, const User& myselft_info, const User& friend_info)
: bgimage_(bgimage)
, bkcolor_(bkcolor)
, myselft_(myselft_info)
, friend_(friend_info)
, emotion_timer_start_(false)
, text_color_(0xFF000000)
, bold_(false)
, italic_(false)
, underline_(false)
, font_size_(12)
, font_face_name_(_T("微软雅黑"))
{
	m_lpFaceList = NULL;
}

ChatDialog::~ChatDialog()
{
	if (m_lpFaceList)
		delete m_lpFaceList;
	m_lpFaceList = NULL;
	RemoveObserver();
	return;
}

LPCTSTR ChatDialog::GetWindowClassName() const
{
	return _T("ChatDialog");
}

CControlUI* ChatDialog::CreateControl(LPCTSTR pstrClass)
{
	return NULL;
}
CDuiString ChatDialog::GetSkinPath()
{
	return CPaintManagerUI::GetInstancePath();
}
CDuiString ChatDialog::GetSkinFolder()
{
	return _T("skin");
}
UILIB_RESOURCETYPE ChatDialog::GetResourceType() const
{
	return UILIB_ZIP;
}
CDuiString ChatDialog::GetZIPFileName() const
{
	return _T("Cli.dat");
}

BOOL ChatDialog::Receive(SkinChangedParam param)
{
	bgimage_ = param.bgimage;
	bkcolor_ = param.bkcolor;
	CControlUI* background = GetPaintMgr()->FindControl(kBackgroundControlName);
	if (background != NULL)
	{
		if (!param.bgimage.IsEmpty())
		{
			TCHAR szBuf[MAX_PATH] = {0};
#if defined(UNDER_WINCE)
			_stprintf(szBuf, _T("file='%s' corner='600,200,1,1'"), param.bgimage.c_str());
#else
			_stprintf_s(szBuf, MAX_PATH - 1, _T("file='%s' corner='600,200,1,1'"), param.bgimage);
#endif
			background->SetBkImage(szBuf);
		}
		else
			background->SetBkImage(_T(""));

		background->SetBkColor(param.bkcolor);
	}

	return TRUE;
}

LRESULT ChatDialog::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}
LRESULT ChatDialog::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}
LRESULT ChatDialog::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
#if defined(WIN32) && !defined(UNDER_CE)
	BOOL bZoomed = ::IsZoomed(m_hWnd);
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	if (::IsZoomed(m_hWnd) != bZoomed)
	{
		if (!bZoomed)
		{
			CControlUI* pControl = static_cast<CControlUI*>(GetPaintMgr()->FindControl(kMaxButtonControlName));
			if( pControl ) pControl->SetVisible(false);
			pControl = static_cast<CControlUI*>(GetPaintMgr()->FindControl(kRestoreButtonControlName));
			if( pControl ) pControl->SetVisible(true);
		}
		else 
		{
			CControlUI* pControl = static_cast<CControlUI*>(GetPaintMgr()->FindControl(kMaxButtonControlName));
			if( pControl ) pControl->SetVisible(true);
			pControl = static_cast<CControlUI*>(GetPaintMgr()->FindControl(kRestoreButtonControlName));
			if( pControl ) pControl->SetVisible(false);
		}
	}
#else
	return __super::OnSysCommand(uMsg, wParam, lParam, bHandled);
#endif

	return 0;
}
LRESULT ChatDialog::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	try
	{
		ModifyStyle(WS_CAPTION,WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
		if(!RegisterSkin(_T("chatwindow.xml"),NULL,this))
			return 0;

		GetPaintMgr()->AddNotifier(this);
		RECT rcClient;
		::GetClientRect(*this, &rcClient);
		::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
			rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);
		GetPaintMgr()->SetNextTabControl(true);
	}
	catch (...)
	{
		throw _T("ChatDialog::OnCreate");
	}

	return 0;
}
LRESULT ChatDialog::ProcessWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (FACE_CTRL_SEL == uMsg)
		return OnFaceSelected(uMsg, wParam, lParam, bHandled);
	else if (RECV_MSG_ID == uMsg)
		return OnRecvChatMsg(uMsg, wParam, lParam, bHandled);
	else if (uMsg == WM_NOTIFY && (EN_PASTE == ((LPNMHDR)lParam)->code))
	{
		ITextServices * pTextServices = m_pInput->GetTextServices();
		if (NULL == pTextServices) return FALSE;
		if ((UINT)pTextServices == ((LPNMHDR)lParam)->idFrom)
		{
			OnPaste(((NMRICHEDITOLECALLBACK *)lParam)->lpszText);
		}
		if (pTextServices != NULL)
			pTextServices->Release();
	}
	else if ((uMsg == WM_KEYDOWN) && (wParam == 'V') && (lParam & VK_CONTROL))//Ctrl+V
	{
		if (m_pInput != NULL && m_pInput->IsFocused())
		{
			m_pInput->Paste();
			if (IsClipboardFormatAvailable(CF_TEXT))
				m_pInput->PasteSpecial(CF_TEXT);
			else if (IsClipboardFormatAvailable(CF_BITMAP))
				m_pInput->PasteSpecial(CF_BITMAP);
		}
	}
	return __super::ProcessWindowMessage(uMsg, wParam, lParam, bHandled);
}
LRESULT ChatDialog::ResponseDefaultKeyEvent(WPARAM wParam)
{
	if (wParam == VK_RETURN)
	{
		return FALSE;
	}
	else if (wParam == VK_ESCAPE)
	{
		return TRUE;
	}
	return FALSE;
}
LRESULT ChatDialog::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM, bool& bHandled)
{
	if (uMsg == WM_KEYDOWN)
	{
		return ResponseDefaultKeyEvent(wParam);
	}
	return FALSE;

}

void ChatDialog::OnTimer(TNotifyUI& msg)
{}

void ChatDialog::OnExit(TNotifyUI& msg)
{
	thelockChatDlgList.Lock();
	POSITION pos,temp;
	for (pos=theChatDialogList.GetHeadPosition();pos!=NULL;)
	{
		temp = pos;
		ChatDialog* pDlg = (ChatDialog*)theChatDialogList.GetNext(pos);
		if (pDlg&&pDlg==this)
		{
			theChatDialogList.RemoveAt(temp);
		}
	}
	thelockChatDlgList.UnLock();
	Close();
}

void ChatDialog::InitWindow()
{
	 m_pInput = static_cast<CRichEditUI*>(GetPaintMgr()->FindControl(kInputRichEditControlName));
	 m_pMsgView = static_cast<CRichEditUI*>(GetPaintMgr()->FindControl(kViewRichEditControlName));

	 RichEdit_SetOleCallback(m_pMsgView, m_hWnd);
	 RichEdit_SetOleCallback(m_pInput, m_hWnd);

	 RichEdit_RegisterDragDrop(m_pInput, m_hWnd);

	 if (theApp->HasMsgForUserInQueue(friend_.m_loadName))
	 {
		 theLockMsgQue.Lock();
		 typedef std::map<CDuiString,std::vector<CMsg*> >::iterator iter;
		 iter it;
		 for (it=theMsgQueue.begin();it!=theMsgQueue.end();++it)
		 {
			 if(it->first==friend_.m_loadName)
			 {
				 std::vector<CMsg*> msg = it->second;
				 for (int i=0;i<msg.size();++i)
				 {
					 ShowMsg(msg[i]);
				 }
				theMsgQueue.erase(it);
				 break;
			 }
		 }
		 theLockMsgQue.UnLock();
	 }

	 m_lpFaceList = new CFaceList;
	 CDuiString strPath = Path::CombinePath(CPaintManagerUI::GetResourcePath(), _T("face\\FaceConfig.xml"));// "..\\Face\\FaceConfig.xml");
	 m_lpFaceList->LoadConfigFile(strPath);
}

void ChatDialog::OnPrepare(TNotifyUI& msg)
{
	TCHAR szBuf[MAX_PATH] = {0};

	CControlUI* background = GetPaintMgr()->FindControl(kBackgroundControlName);
	if (background != NULL)
	{
#if defined(UNDER_WINCE)
		_stprintf(szBuf, _T("file='%s' corner='600,200,1,1'"), bgimage_.c_str());
#else
		_stprintf_s(szBuf, MAX_PATH - 1, _T("file='%s' corner='600,200,1,1'"), bgimage_.GetData());
#endif

		//background->SetBkImage(szBuf);
		//background->SetBkColor(bkcolor_);
	}

	CButtonUI* log_button = static_cast<CButtonUI*>(GetPaintMgr()->FindControl(kLogoButtonControlName));
// 	if (log_button != NULL)
// 	{
// #if defined(UNDER_WINCE)
// 		_stprintf(szBuf, _T("%s"), friend_.logo.c_str());
// #else
// 		_stprintf_s(szBuf, MAX_PATH - 1, _T("%s"), friend_.m_callName);
// #endif
// 		log_button->SetNormalImage(szBuf);
// 	}

	CControlUI* nick_name = GetPaintMgr()->FindControl(kNickNameControlName);
	if (nick_name != NULL)
		nick_name->SetText(friend_.m_loadName);

	CControlUI* desciption = GetPaintMgr()->FindControl(kDescriptionControlName);
	if (desciption != NULL)
		desciption->SetText(friend_.m_callName);

// 	CContainerUI* pFontbar = static_cast<CContainerUI*>(GetPaintMgr()->FindControl(kFontbarControlName));
// 	if (pFontbar != NULL)
// 		pFontbar->SetVisible(!pFontbar->IsVisible());
}

CDuiString GetCurrentTimeString()
{
	SYSTEMTIME time = {0};
	TCHAR szTime[MAX_PATH] = {0};
	::GetLocalTime( &time );
#if defined(UNDER_CE)
	_stprintf( szTime, _T("%04d-%02d-%02d %02d:%02d:%02d"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
#else
	_stprintf_s( szTime, MAX_PATH, _T("%04d-%02d-%02d %02d:%02d:%02d"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
#endif

	return szTime;
}

void ChatDialog::SendMsg()
{
 
    if( m_pInput == NULL ) return;
    m_pInput->SetFocus();

	CDuiString s_text;
	s_text.Empty();
	_RichEdit_GetText(m_pInput, s_text);
	//MsgText* pmsg = NewMsgText(s_text);
	//PostMessage(RECV_MSG_ID, (WPARAM)pmsg);
	FormatMsgToEditView(myselft_.m_loadName, mci::format_currenttime(), s_text);

	m_pInput->SetText(_T(""));
	if (s_text.IsEmpty()) return;
    if( m_pMsgView == NULL ) return;

    CDuiString sMsg = s_text;

//	theApp->SendMsg(sMsg);
	User user;
	if (theApp->FindUserFromList(friend_.m_loadName,user))
	{
		theApp->SendTo(user,sMsg);
	}
	
}

void ChatDialog::ShowMsg(CMsg* pMsg)
{
	if(pMsg==NULL) return ;
	CDuiString sWho = pMsg->m_nomalMsg.who;
	CDuiString sToWho = pMsg->m_nomalMsg.toWho;
	CDuiString sMsg = pMsg->m_nomalMsg.msgItem;
	CDuiString sTime = pMsg->m_nomalMsg.sndTime;

	FormatMsgToEditView(sWho,sTime,sMsg);
}

void ChatDialog::FormatMsgToEditView(CDuiString sName,CDuiString sTime,CDuiString sMsg)
{
	/*long lSelBegin = 0, lSelEnd = 0;
	CHARFORMAT2 cf;
	ZeroMemory(&cf, sizeof(CHARFORMAT2));
	cf.cbSize = sizeof(cf);
	cf.dwReserved = 0;
	cf.dwMask = CFM_COLOR  | CFM_UNDERLINE | CFM_UNDERLINETYPE | CFM_BOLD;
	cf.dwEffects = CFE_LINK;
	cf.bUnderlineType = CFU_UNDERLINE;
	cf.crTextColor = RGB(0, 0, 255);
	if(sName==myselft_.m_loadName)
		cf.crTextColor = RGB(255, 0, 0);
	lSelEnd = lSelBegin = m_pMsgView->GetTextLength();
	m_pMsgView->SetSel(lSelEnd, lSelEnd);//移到末尾
	sName = CDuiString(_T("[ "))+sName;
	sName += _T(" ");
	sName += sTime;
	sName += _T(" ]\n");
	m_pMsgView->ReplaceSel(sName, false);

	lSelEnd = m_pMsgView->GetTextLength();
	m_pMsgView->SetSel(lSelBegin, lSelEnd);
	m_pMsgView->SetSelectionCharFormat(cf);

	cf.dwMask = CFM_COLOR;
	cf.crTextColor = RGB(0, 0, 0);
	cf.dwEffects = 0;

	PARAFORMAT2 pf;
	ZeroMemory(&pf, sizeof(PARAFORMAT2));
	pf.cbSize = sizeof(pf);
	pf.dwMask = PFM_STARTINDENT;
	pf.dxStartIndent = 0;
	m_pMsgView->SetParaFormat(pf);

	lSelEnd = lSelBegin = m_pMsgView->GetTextLength();

	m_pMsgView->SetSel(-1, -1);
	m_pMsgView->ReplaceSel(sMsg, false);

	m_pMsgView->SetSel(-1, -1);
	m_pMsgView->ReplaceSel(_T("\n"), false);

	cf.crTextColor = RGB(0, 0, 0);
	lSelEnd = m_pMsgView->GetTextLength();
	m_pMsgView->SetSel(lSelBegin, lSelEnd);
	m_pMsgView->SetSelectionCharFormat(cf);

	ZeroMemory(&pf, sizeof(PARAFORMAT2));
	pf.cbSize = sizeof(pf);
	pf.dwMask = PFM_STARTINDENT;
	pf.dxStartIndent = 220;
	m_pMsgView->SetParaFormat(pf);

	m_pMsgView->EndDown();*/

	AddMsgTitle(sName, sTime);
	AddMsgToRecvEdit(sMsg);
}

void ChatDialog::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, _T("windowinit")) == 0)
	{
		OnPrepare(msg);
		InitWindow();
	}
	else if (_tcsicmp(msg.sType, _T("killfocus")) == 0)
	{
	}
	else if (_tcsicmp(msg.sType, _T("click")) == 0)
	{
		if (_tcsicmp(msg.pSender->GetName(), _T("shockbtn"))==0)
		{
			theApp->ShakeToWho(friend_);
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("trans_file"))==0)
		{
			OnTransFile(msg);
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("empty_richedit")) == 0)
		{
			int lSelBegin,lSelEnd;
			lSelEnd = lSelBegin = m_pMsgView->GetTextLength();
			m_pMsgView->SetSel(0, lSelEnd);//选择全部	
			m_pMsgView->ReplaceSel(_T(""), false);
		}
		else if (_tcsicmp(msg.pSender->GetName(), kCloseButtonControlName) == 0)
		{
			OnExit(msg);
		}
		else if (_tcsicmp(msg.pSender->GetName(), kMinButtonControlName) == 0)
		{
#if defined(UNDER_CE)
			::ShowWindow(m_hWnd, SW_MINIMIZE);
#else
			SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
#endif
		}
		else if (_tcsicmp(msg.pSender->GetName(), kMaxButtonControlName) == 0)
		{
#if defined(UNDER_CE)
			::ShowWindow(m_hWnd, SW_MAXIMIZE);
			CControlUI* pControl = static_cast<CControlUI*>(GetPaintMgr()->FindControl(kMaxButtonControlName));
			if( pControl ) pControl->SetVisible(false);
			pControl = static_cast<CControlUI*>(GetPaintMgr()->FindControl(kRestoreButtonControlName));
			if( pControl ) pControl->SetVisible(true);
#else
			SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
#endif
		}
		else if (_tcsicmp(msg.pSender->GetName(), kRestoreButtonControlName) == 0)
		{
#if defined(UNDER_CE)
			::ShowWindow(m_hWnd, SW_RESTORE);
			CControlUI* pControl = static_cast<CControlUI*>(GetPaintMgr()->FindControl(kMaxButtonControlName));
			if( pControl ) pControl->SetVisible(true);
			pControl = static_cast<CControlUI*>(GetPaintMgr()->FindControl(kRestoreButtonControlName));
			if( pControl ) pControl->SetVisible(false);
#else
			SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
#endif
		}
		else if (_tcsicmp(msg.pSender->GetName(), kFontButtonControlName) == 0)
		{
			CContainerUI* pFontbar = static_cast<CContainerUI*>(GetPaintMgr()->FindControl(kFontbarControlName));
			if (pFontbar != NULL)
				pFontbar->SetVisible(!pFontbar->IsVisible());
		}
		else if (_tcsicmp(msg.pSender->GetName(), kPICButtonControlName) == 0)
		{
			static LPCTSTR lpszFilter = _T("选择图片(*.bmp;*.jpg;*.jpeg;*.gif;*.png)\0*.bmp;*.jpg;*.jpeg;*.gif;*.png\0\0");;
			if (NULL == m_pInput)
				return;
			DuiLib::CDuiString szFilter(lpszFilter);
			DuiLib::CFileDialog * pFileDlg = new DuiLib::CFileDialog();
			pFileDlg->SetFilter(szFilter);
			pFileDlg->SetParentWnd(m_hWnd);
			if (pFileDlg->ShowOpenFileDlg())
			{
				ITextServices * pTextServices = m_pInput->GetTextServices();
				ITextHost * pTextHost = m_pInput->GetTextHost();
				if (pTextServices != NULL && pTextHost != NULL)
				{
					BOOL bRet = DuiLib::RichEdit_InsertFace(pTextServices, pTextHost,
						pFileDlg->GetPathName(), -1, -1, RGB(255, 255, 255), TRUE, 40);

					if (pTextServices != NULL)
						pTextServices->Release();
					if (pTextHost != NULL)
						pTextHost->Release();
				}

				m_pInput->SetFocus();
			}
			delete pFileDlg;
		}
		else if (_tcsicmp(msg.pSender->GetName(), kEmotionButtonControlName) == 0)
		{
			POINT pt = {0};
			CDuiRect rcEmotionBtn = msg.pSender->GetPos();
			RECT rcWindow;
			GetWindowRect( &rcWindow );

			pt.y = rcWindow.top + rcEmotionBtn.top;
			pt.x = rcWindow.left + rcEmotionBtn.left;
			//emotion_list_window_.SelectEmotion(pt);
			CFaceSelDlg* pFaceDialog = new CFaceSelDlg();
			if (pFaceDialog == NULL)
				return;
			pFaceDialog->SetFaceList(m_lpFaceList);
#if defined(WIN32) && !defined(UNDER_CE)
			pFaceDialog->Create(m_hWnd, _T("FaceDialog"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
#else
			pFaceDialog->Create(m_hWnd, _T("FaceDialog"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
#endif
			pFaceDialog->CenterWindow();
			::ShowWindow(*pFaceDialog, SW_SHOW);
		}
		else if (_tcsicmp(msg.pSender->GetName(), kSendButtonControlName) == 0)
        {
            SendMsg();
		}
	}
    else if( _tcsicmp(msg.sType, _T("return")) == 0 ) 
    {
        if (_tcsicmp(msg.pSender->GetName(), kInputRichEditControlName) == 0)
        {
            SendMsg();
        }
    }
	else if (_tcsicmp(msg.sType, _T("timer")) == 0)
	{
		return OnTimer(msg);
	}
	else if (_tcsicmp(msg.sType, _T("selectchanged")) == 0)
	{
		if (_tcsicmp(msg.pSender->GetName(), kColorButtonControlName) == 0)
		{
// 			CContainerUI* pFontbar = static_cast<CContainerUI*>(GetPaintMgr()->FindControl(kFontbarControlName));
// 			if (pFontbar != NULL)
// 			{
// 				POINT pt = {0};
// 				CDuiRect rcFontbar = pFontbar->GetPos();
// 				CDuiRect rcColorBtn = msg.pSender->GetPos();
// 				RECT rcWindow;
// 				GetWindowRect( &rcWindow);
// 
// 				pt.y = rcWindow.top + rcFontbar.top;
// 				pt.x = rcWindow.left + rcColorBtn.left + static_cast<LONG>(rcColorBtn.right - rcColorBtn.left / 2);
// 				new CColorPicker(this, pt);
// 			}
			static CHOOSECOLOR cc;
			static COLORREF crCustCoLors[16];
			cc.lStructSize=sizeof(CHOOSECOLOR);
			cc.hwndOwner= this->m_hWnd;
			cc.hInstance=NULL;
			cc.rgbResult = RGB(0,0,0);
			cc.lpCustColors=crCustCoLors;
			cc.Flags=CC_RGBINIT|CC_FULLOPEN | CC_ANYCOLOR; 
			cc.lCustData=0;
			cc.lpfnHook=NULL;
			cc.lpTemplateName=NULL;
			if(TRUE == ChooseColor(&cc))
			{ 
				TCHAR szBuf[MAX_PATH] = {0};
#if defined(UNDER_CE)
				_stprintf(szBuf, _T("FF%02X%02X%02X"), GetRValue(cc.rgbResult), GetGValue(cc.rgbResult), GetBValue(cc.rgbResult));
#else
				_stprintf_s(szBuf, MAX_PATH - 1, _T("FF%02X%02X%02X"), GetRValue(cc.rgbResult), GetGValue(cc.rgbResult), GetBValue(cc.rgbResult));
#endif
				LPTSTR pstr = NULL;
				text_color_ = _tcstoul(szBuf, &pstr, 16);
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kBoldButtonControlName) == 0)
		{
			COptionUI* bold_button = static_cast<COptionUI*>(msg.pSender);
			if (bold_button != NULL)
			{
				bold_ = bold_button->IsSelected();
				FontStyleChanged();
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kItalicButtonControlName) == 0)
		{
			COptionUI* italic_button = static_cast<COptionUI*>(msg.pSender);
			if (italic_button != NULL)
			{
				italic_ = italic_button->IsSelected();
				FontStyleChanged();
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), KUnderlineButtonControlName) == 0)
		{
			COptionUI* underline_button = static_cast<COptionUI*>(msg.pSender);
			if (underline_button != NULL)
			{
				underline_ = underline_button->IsSelected();
				FontStyleChanged();
			}
		}
	}
	else if (_tcsicmp(msg.sType, _T("itemselect")) == 0)
	{
		if (_tcsicmp(msg.pSender->GetName(), kFontTypeControlName) == 0)
		{
			CComboUI* font_type = static_cast<CComboUI*>(msg.pSender);
			if (font_type != NULL)
			{
				font_face_name_ = font_type->GetText();
				FontStyleChanged();
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kFontSizeControlName) == 0)
		{
			CComboUI* font_size = static_cast<CComboUI*>(msg.pSender);
			if (font_size != NULL)
			{
				font_size_ = _ttoi(font_size->GetText());
				FontStyleChanged();
			}
		}
	}
}

void ChatDialog::SetTextColor(DWORD dwColor)
{
	COptionUI* color_button = static_cast<COptionUI*>(GetPaintMgr()->FindControl(kColorButtonControlName));
	if (color_button != NULL)
	{
		color_button->Selected(false);
		if (dwColor != 0)
		{
			text_color_ = dwColor;
			FontStyleChanged();
		}
	}
}

void ChatDialog::FontStyleChanged()
{}

void ChatDialog::OnTransFile(TNotifyUI& msg)
{
	OPENFILENAME openfilename;
	ZeroMemory(&openfilename, sizeof(OPENFILENAME)); 
	TCHAR szfiletitle[MAX_PATH] = _T("\0");
	CDuiString s_title=_T("请选择要传输的文件");
	LPTSTR szFile = new TCHAR[MAX_PATH * 101 + 1]; //最多可以选中100项
	szFile[0] = 0x0;

	static TCHAR lpstr[] = _T("文件(*.*;)\0*.*\0\0");
	openfilename.lStructSize = sizeof(OPENFILENAME) ;
	openfilename.hwndOwner = this->m_hWnd ;
	openfilename.hInstance = NULL ;
	openfilename.lpstrFilter = lpstr;//lpstr;
	openfilename.lpstrCustomFilter = (LPTSTR)NULL ;
	openfilename.nMaxCustFilter = 0L ;
	openfilename.nFilterIndex = 1L ;
	openfilename.lpstrFile = szFile ;
	openfilename.nMaxFile = MAX_PATH * 101 + 1 ;
	openfilename.lpstrFileTitle = szfiletitle ;
	openfilename.nMaxFileTitle = sizeof(szfiletitle) ;
	openfilename.lpstrInitialDir = NULL ;
	openfilename.lpstrTitle = s_title;
	openfilename.nFileOffset = 0;
	openfilename.nFileExtension = 0;
	openfilename.lpstrDefExt = NULL ;
	openfilename.lCustData = 0 ;

	openfilename.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_READONLY | OFN_ALLOWMULTISELECT | OFN_EXPLORER /*|OFN_LONGNAMES*/;

//	m_FilePath.erase(m_FilePath.begin(),m_FilePath.end());
	if (GetOpenFileName(&openfilename))
	{
		LPTSTR p = szFile;
		while(*p++);
		if (*p == NULL) // 只选了一个文件
		{
			CDuiString s_file = szFile;

//			m_FilePath.push_back(s_file);
		}
		else    ///////////////////////////////////////////////////////// 多选，循环取出文件
		{
			CDuiString folder = szFile;
			while(*p)
			{
				CDuiString fileName = p;
				CDuiString file = folder + _T("\\") + fileName;
//				m_FilePath.push_back(file);

				while(*p++);
			}
		}
	}
	delete[] szFile;
}

// LRESULT ChatDialog::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
// {
//	return 0;
//}
LRESULT ChatDialog::OnRecvChatMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	MsgText* pmsg = (MsgText*)wParam;

	CDuiString s_text(pmsg->sText);

	AddMsgToRecvEdit(s_text);

	DelMsgText(pmsg);
	return 0;
}

LRESULT ChatDialog::OnFaceSelected(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	if (m_pInput == NULL) return 0;

	CFaceInfo * lpFaceInfo = (CFaceInfo*)wParam;

	CDuiString sFacePath = CFaceCtrl::GetFaceFloder() + lpFaceInfo->m_strFileName;


	BOOL bRet = _RichEdit_InsertFace(m_pInput, sFacePath, lpFaceInfo->m_nId, lpFaceInfo->m_nIndex, RGB(255, 255, 255), TRUE, 40);

	return 0;
}

void ChatDialog::OnPaste(LPCTSTR lpText)
{

	if (!IsClipboardFormatAvailable(CF_BITMAP))
	{
		DUI__Trace(_T("IsClipboardFormatAvailable() failed!\n"));
		return;
	}
	if (!OpenClipboard(NULL))
	{
		DUI__Trace(_T("OpenClipboard() failed!\n"));
		return;
	}
	HANDLE hMem = GetClipboardData(CF_BITMAP);
	ASSERT(hMem != NULL);

	HBITMAP hBMP = (HBITMAP)GlobalLock(hMem);
	if (!hBMP)
	{
		DUI__Trace(_T("GlobalLock() failed: %d!\n"), GetLastError());

	}
	//Do something here using hBMP
	GlobalUnlock(hMem);
	//Free the memory
	CloseClipboard();

	if (!OpenClipboard(NULL))
		return;
	hMem = GetClipboardData(CF_UNICODETEXT);
	if (hMem != NULL)
	{
		LPTSTR lpStr = (LPTSTR)GlobalLock(hMem);
		if (lpStr != NULL)
		{
			OutputDebugString(lpStr);
		}
		GlobalUnlock(hMem);
	}
	CloseClipboard();
	if (NULL == lpText || NULL == *lpText)
		return;
	m_pInput->EndDown();
}

void ChatDialog::_RichEdit_ReplaceSel(CRichEditUI * pRichEdit, LPCTSTR lpszNewText, BOOL bIsLink,
	int nStartIndent, BOOL bCanUndo)
{
	ITextServices * pTextServices = pRichEdit->GetTextServices();
	DuiLib::RichEdit_ReplaceSel(pTextServices, lpszNewText,
		m_fontInfo.m_strName, m_fontInfo.m_nSize, m_fontInfo.m_clrText, m_fontInfo.m_bBold, m_fontInfo.m_bItalic, m_fontInfo.m_bUnderLine, bIsLink, nStartIndent, bCanUndo);

	pTextServices->Release();
}

void ChatDialog::_RichEdit_GetText(CRichEditUI* pRichEdit, CDuiString& s_text)
{
	RichEdit_ImageStringHandleImp* pHandle = new RichEdit_ImageStringHandleImp;
	ITextServices * pTextServices = pRichEdit->GetTextServices();
	DuiLib::RichEdit_GetText(pTextServices, s_text, pHandle);
	pTextServices->Release();
	delete pHandle;
}

void ChatDialog::AddMsgTitle(LPCTSTR lpName, LPCTSTR lpTime)
{
	ITextServices * pTextServices = m_pMsgView->GetTextServices();
	DuiLib::RichEdit_SetSel(pTextServices, -1, -1);

	TCHAR cText[2048] = { 0 };
	_stprintf(cText, _T("%s("), lpName);

	RichEdit_ReplaceSel(pTextServices, cText,
		_T("宋体"), 9, RGB(0, 0, 255), FALSE, FALSE, FALSE, FALSE, 0);

	_stprintf(cText, _T(")  %s\r\n"), lpTime);
	RichEdit_ReplaceSel(pTextServices, cText,
		_T("宋体"), 9, RGB(0, 0, 255), FALSE, FALSE, FALSE, FALSE, 0);

	pTextServices->Release();
}

void ChatDialog::AddMsgToRecvEdit(LPCTSTR lpText)
{
	if (NULL == lpText || NULL == *lpText)
		return;

	m_pMsgView->SetAutoURLDetect(true);

	ITextServices * pTextServices = m_pMsgView->GetTextServices();
	DuiLib::RichEdit_SetSel(pTextServices, -1, -1);

	RichEdit_ImageStringHandleImp* pHandle = new RichEdit_ImageStringHandleImp;
	std::vector<DuiLib::RichEdit_Text> t_out;
	CDuiString s_in(lpText);
	pHandle->PraseImageStringS(s_in, t_out);

	for (size_t it = 0; it < t_out.size(); ++it)
	{
		DuiLib::RichEdit_Text t = t_out[it];
		if (!t.bImage)
		{
			_RichEdit_ReplaceSel(m_pMsgView, t.sText, FALSE, 300);
		}
		else
		{
			ImageString* pImageString = new ImageString;

			if (pHandle->PraseImageString(t.sText, pImageString))
			{
				if (pImageString->nId > 0)
				{
					CFaceInfo * lpFaceInfo = m_lpFaceList->GetFaceInfoById(pImageString->nId);
					if (lpFaceInfo)
					{

						CDuiString sFacePath = CFaceCtrl::GetFaceFloder() + lpFaceInfo->m_strFileName;


						BOOL bRet = _RichEdit_InsertFace(m_pMsgView, sFacePath, lpFaceInfo->m_nId, lpFaceInfo->m_nIndex, RGB(255, 255, 255), TRUE, 40, 300);
					}
				}
				else if (pImageString->sFullPath.GetLength()>0)
				{
					//CDuiString sFacePath = CFaceCtrl::GetFaceFloder() + lpFaceInfo->m_strFileName;


					BOOL bRet = _RichEdit_InsertFace(m_pMsgView, pImageString->sFullPath, 0, 0, RGB(255, 255, 255), TRUE, 40, 300);
				}
			}

			delete pImageString;
		}
	}

	delete pHandle;


	RichEdit_ReplaceSel(pTextServices, _T("\r\n"));
	RichEdit_SetStartIndent(pTextServices, 0);
	m_pMsgView->EndDown();

	pTextServices->Release();
}

BOOL ChatDialog::_RichEdit_InsertFace(CRichEditUI* pRichEdit, LPCTSTR lpszFileName, int nFaceId, int nFaceIndex, COLORREF clrBg, BOOL bAutoScale, int nReservedWidth, int nIndent)
{
	ITextServices * pTextServices = pRichEdit->GetTextServices();
	ITextHost * pTextHost = pRichEdit->GetTextHost();

	RichEdit_SetStartIndent(pTextServices, nIndent);

	BOOL bRet = RichEdit_InsertFace(pTextServices, pTextHost, lpszFileName, nFaceId, nFaceIndex, clrBg, bAutoScale, nReservedWidth);
	if (pTextServices != NULL)
		pTextServices->Release();
	if (pTextHost != NULL)
		pTextHost->Release();

	return bRet;
}