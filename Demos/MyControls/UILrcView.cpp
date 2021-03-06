#include "MyControls.h"
#include "UILrcView.h"

#include <GdiPlus.h>
#include <GdiPlusGraphics.h>
#include <GdiPlusPen.h>
using namespace Gdiplus;

namespace DuiLib {
	const UINT MyTimerID = 1;
	CLrcViewUI::CLrcViewUI()
	:CContainerUI()
	,m_handler(NULL)
	,m_pSearchLrcBtn(NULL)
	,m_eStatus(CLrcViewUI::emStatus::none)
	{
		m_sIdleText     	= _T("libyyu@qq.com");
		m_nLineHeight   	= 20;
		m_fTimeOffset   	= 0.0;
		m_fTimeDelta        = 500;

		memset(&m_lrcFont,0,sizeof(m_lrcFont));
		m_lrcFont.lfWidth       = 0;
		m_lrcFont.lfUnderline   = FALSE;
		m_lrcFont.lfStrikeOut   = FALSE;
		m_lrcFont.lfOrientation = 0;
		m_lrcFont.lfEscapement  = 0;
		m_lrcFont.lfCharSet     = DEFAULT_CHARSET;
		m_lrcFont.lfHeight      = -12;
		m_lrcFont.lfWeight      = FW_NORMAL;
		m_lrcFont.lfItalic      = FALSE;
		_tcscpy_s(m_lrcFont.lfFaceName, _T("微软雅黑"));

	}

	CLrcViewUI::~CLrcViewUI()
	{
		this->RemoveAll();
		if (m_pSearchLrcBtn) m_pSearchLrcBtn->Delete();
		m_pSearchLrcBtn = NULL;
	}

	LPCTSTR CLrcViewUI::GetClass() const
	{
		return DUI_CTR_LRCVIEW;
	}

	LPVOID CLrcViewUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_LRCVIEW) == 0 ) return static_cast<CLrcViewUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	void CLrcViewUI::SetLyrics(const std::vector<LrcNode>& v)
	{
		m_Lyrics = v;
		m_pManager->KillTimer(this, MyTimerID);
		m_pManager->SetTimer(this, MyTimerID, 1000);
	}
	void CLrcViewUI::SetStatus(emStatus status)
	{
		m_eStatus = status;
	}

	void CLrcViewUI::SetIdleText(LPCTSTR s)
	{ 
		if (m_sIdleText == s) return;
		m_sIdleText = s; 
		NeedUpdate();
	}
	void CLrcViewUI::SetSearchButtonText(LPCTSTR s)
	{
		if (m_sSearchButtonText == s) return;
		m_sSearchButtonText = s;
		if (m_pSearchLrcBtn) m_pSearchLrcBtn->SetText(s);
	}

	void CLrcViewUI::SetSearchButtonTooltip(LPCTSTR s)
	{
		if (m_sSearchButtonTooltip == s) return;
		m_sSearchButtonTooltip = s;
		if (m_pSearchLrcBtn) m_pSearchLrcBtn->SetToolTip(s);
	}

	void CLrcViewUI::SetSearchButtonTextColor(DWORD c)
	{
		if (m_pSearchLrcBtn) m_pSearchLrcBtn->SetTextColor(c);
	}

	void CLrcViewUI::SetPos(RECT rc, bool bNeedInvalidate)
	{
		CContainerUI::SetPos(rc, bNeedInvalidate);

		if (m_pSearchLrcBtn && m_pSearchLrcBtn->IsVisible()) {
			int iLeft = rc.left + m_rcInset.left;
			int iRight = rc.right - m_rcInset.right;
			int iWidth = iRight - iLeft;
			int iTop = rc.top + m_rcInset.top;
			int iBottom = rc.bottom - m_rcInset.bottom;
			int iHeight = iBottom - iTop;
			SIZE v = m_pSearchLrcBtn->GetFixedXY();
			LONG left = (LONG)(iWidth / 2 - v.cx / 2);
			LONG top = (LONG)(iHeight / 2 - v.cy / 2);
			LONG right = left + v.cx;
			LONG bottom = top + v.cy;
			m_pSearchLrcBtn->SetPos(CDuiRect(left, top, right, bottom));
		}
	}

	void CLrcViewUI::Move(SIZE szOffset, bool bNeedInvalidate)
	{
		CContainerUI::Move(szOffset, bNeedInvalidate);
		if (m_pSearchLrcBtn && m_pSearchLrcBtn->IsVisible()) m_pSearchLrcBtn->Move(szOffset, false);
	}

	void CLrcViewUI::SetVisible(bool bVisible)
	{
		CContainerUI::SetVisible(bVisible);
		if (m_pSearchLrcBtn) {
			m_pSearchLrcBtn->SetVisible(bVisible);
		}
	}
	void CLrcViewUI::SetInternVisible(bool bVisible)
	{
		CContainerUI::SetInternVisible(bVisible);
	}

	void CLrcViewUI::SetSearchButtonAttributeList(LPCTSTR pstrValue)
	{
		CDuiString sItem;
		CDuiString sValue;
		while (*pstrValue != _T('\0')) {
			sItem.Empty();
			sValue.Empty();
			while (*pstrValue != _T('\0') && *pstrValue != _T('=')) {
				LPTSTR pstrTemp = ::CharNext(pstrValue);
				while (pstrValue < pstrTemp) {
					sItem += *pstrValue++;
				}
			}
			ASSERT(*pstrValue == _T('='));
			if (*pstrValue++ != _T('=')) return;
			ASSERT(*pstrValue == _T('\"'));
			if (*pstrValue++ != _T('\"')) return;
			while (*pstrValue != _T('\0') && *pstrValue != _T('\"')) {
				LPTSTR pstrTemp = ::CharNext(pstrValue);
				while (pstrValue < pstrTemp) {
					sValue += *pstrValue++;
				}
			}
			ASSERT(*pstrValue == _T('\"'));
			if (*pstrValue++ != _T('\"')) return;
			else if (_tcscmp(sItem, _T("searchbtntext")) == 0) SetSearchButtonText(sValue);
			else if (_tcscmp(sItem, _T("searchbtntooltip")) == 0) SetSearchButtonTooltip(sValue);
			else if (_tcscmp(sItem, _T("searchbtntextcolor")) == 0) {
				LPCTSTR ssValue = sValue;
				while (*ssValue > _T('\0') && *ssValue <= _T(' ')) ssValue = ::CharNext(ssValue);
				if (*ssValue == _T('#')) ssValue = ::CharNext(ssValue);
				LPTSTR pstr = NULL;
				DWORD clrColor = _tcstoul(ssValue, &pstr, 16);
				SetSearchButtonTextColor(clrColor);
			}
			else if (m_pSearchLrcBtn)
				m_pSearchLrcBtn->SetXMLAttribute(sItem, sValue);
			if (*pstrValue++ != _T(' ')) return;
		}
	}

	bool CLrcViewUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("idletext")) == 0) {
			SetIdleText(pstrValue); return true;
		}
		else if (_tcscmp(pstrName, _T("showsearchbtn")) == 0) { EnableSearchButton(_tcscmp(pstrValue, _T("true")) == 0); return true; }
		else if (_tcscmp(pstrName, _T("searchbtntext")) == 0) { SetSearchButtonText(pstrValue); return true; }
		else if (_tcscmp(pstrName, _T("searchbtntooltip")) == 0) { SetSearchButtonTooltip(pstrValue); return true; }
		else if (_tcscmp(pstrName, _T("searchbtntextcolor")) == 0) {
			while (*pstrValue > _T('\0') && *pstrValue <= _T(' ')) pstrValue = ::CharNext(pstrValue);
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetSearchButtonTextColor(clrColor);
			return true;
		}
		else if (_tcscmp(pstrName, _T("searchbtnattri")) == 0)
		{
			SetSearchButtonAttributeList(pstrValue);
			return true;
		}
		else return CContainerUI::SetAttribute(pstrName, pstrValue);
	}

	void CLrcViewUI::SetMouseEnabled(bool bEnabled)
	{
		if (m_pSearchLrcBtn != NULL) m_pSearchLrcBtn->SetMouseEnabled(bEnabled);
		CContainerUI::SetMouseEnabled(bEnabled);
	}

	void CLrcViewUI::SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit)
	{
		if (m_pSearchLrcBtn != NULL) m_pSearchLrcBtn->SetManager(pManager, this, bInit);
		CContainerUI::SetManager(pManager, pParent, bInit);
	}

	CControlUI* CLrcViewUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
	{
		// Check if this guy is valid
		if ((uFlags & UIFIND_VISIBLE) != 0 && !IsVisible()) return NULL;
		if ((uFlags & UIFIND_ENABLED) != 0 && !IsEnabled()) return NULL;
		if ((uFlags & UIFIND_HITTEST) != 0 && !::PtInRect(&m_rcItem, *(static_cast<LPPOINT>(pData)))) return NULL;
		if ((uFlags & UIFIND_UPDATETEST) != 0 && Proc(this, pData) != NULL) return NULL;

		CControlUI* pResult = NULL;
		if ((uFlags & UIFIND_ME_FIRST) != 0) {
			if ((uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled()) pResult = Proc(this, pData);
		}
		if (pResult == NULL && m_pCover != NULL) {
			if ((uFlags & UIFIND_HITTEST) == 0 || IsMouseChildEnabled()) pResult = m_pCover->FindControl(Proc, pData, uFlags);
		}
		if (pResult == NULL && m_pSearchLrcBtn != NULL) {
			if ((uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled()) pResult = m_pSearchLrcBtn->FindControl(Proc, pData, uFlags);
		}

		if (pResult != NULL) return pResult;

		return CContainerUI::FindControl(Proc, pData, uFlags);
	}

	void CLrcViewUI::DoInit()
	{
		if (m_pSearchLrcBtn)
		{
			if (m_eStatus == emStatus::play && m_Lyrics.empty())
				m_pSearchLrcBtn->SetVisible(true);
			else
				m_pSearchLrcBtn->SetVisible(false);
		}
	}

	void CLrcViewUI::DoEvent(TEventUI& event)
	{
		if (event.Type == UIEVENT_BUTTONDOWN)
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(32651)));
		}
		else if(event.Type == UIEVENT_TIMER && event.wParam == MyTimerID)
		{
			if(m_pSearchLrcBtn)
			{
				if(m_eStatus == emStatus::play && m_Lyrics.empty())
					m_pSearchLrcBtn->SetVisible(true);
				else 
					m_pSearchLrcBtn->SetVisible(false);
			}
			NeedUpdate();
		}
		else if(UIEVENT_KEYDOWN == event.Type && (event.wKeyState&MK_CONTROL) && (m_eStatus == emStatus::play || m_eStatus == emStatus::pause))
		{
			switch( event.chKey )
			{
			case VK_UP: 
				{
					m_fTimeOffset -= m_fTimeDelta;
					if(m_handler)
						m_handler(this, emEvent::back, NULL);
				}
				break;
			case VK_DOWN:
				{
					m_fTimeOffset += m_fTimeDelta;
					if(m_handler)
						m_handler(this, emEvent::forward, NULL);
				}
				break;
			default:
				return __super::DoEvent(event);
			}
		}
		else
			return __super::DoEvent(event);
	}

	bool CLrcViewUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl)
	{
		__super::DoPaint(hDC, rcPaint, pStopControl);

		RECT rcTemp = { 0 };

		if (m_pSearchLrcBtn != NULL) {
			if (m_pSearchLrcBtn == pStopControl) return false;
			if (m_pSearchLrcBtn->IsVisible()) {
				if (::IntersectRect(&rcTemp, &rcPaint, &m_pSearchLrcBtn->GetPos())) {
					if (!m_pSearchLrcBtn->Paint(hDC, rcPaint, pStopControl)) return false;
				}
			}
		}
		return true;
	}

	void CLrcViewUI::PaintText(HDC hDC)
	{
		__int64 iPlayedTime = 0;
		if(m_handler)
		{
			m_handler(this, emEvent::gettime, &iPlayedTime);
		}
		PaintTextRaw(hDC, iPlayedTime);
	}

	void CLrcViewUI::PaintTextRaw(HDC hDC, __int64 iTimeElapse)
	{
		COLORREF upRow = m_cPlayedColor;
		COLORREF curRow = m_cPlayingColor;
		COLORREF downRow = m_cUnPlayColor;

		CDuiRect rc  = GetPos();
		HRGN hRgn    = ::CreateRectRgnIndirect(&rc);
		HRGN hOldRgn = (HRGN)::SelectObject(hDC,hRgn);
		CDuiRect rcClient(rc.left,rc.top,rc.right,rc.bottom);

		HDC memDC = hDC;
		SetBkMode(memDC, TRANSPARENT);

		float  fTimeOffset     = m_fTimeOffset;
		size_t nLineHeight     = m_nLineHeight;
		int    nTotalHeight    = rcClient.bottom + rcClient.top;
		size_t nTotalLines     = nTotalHeight / nLineHeight;
		int    nTimeElapse     = 0;
		int    nBeginShowIndex = 0;
		size_t nCurShowIndex   = 0;

		COLORREF dwCurTextColor = 0;

		HFONT  hFont   = ::CreateFontIndirect(&m_lrcFont);
		HFONT  hOld    = (HFONT)::SelectObject(hDC,hFont);

		if (m_eStatus == emStatus::stop 
			|| m_eStatus == emStatus::none
			|| m_Lyrics.size() == 0) //空闲或者木有歌词;
		{
			::SetTextColor(hDC, curRow);
			::DrawText(hDC,m_sIdleText,m_sIdleText.GetLength(),&rc, DT_CENTER | DT_VCENTER |DT_SINGLELINE);
		}
		else
		{
			if (m_Lyrics.size() > 0)
			{
				if (m_Lyrics[0].length == 0)
				{
					for (size_t i = 0; i < m_Lyrics.size(); i++)
					{
						SIZE size;
						GetTextExtentPoint32(memDC,m_Lyrics[i].sLyric,m_Lyrics[i].sLyric.GetLength(),&size);
						m_Lyrics[i].length = size.cx;
					}
				}
			}

			nTimeElapse = (iTimeElapse) + (int)(fTimeOffset);
			while (nCurShowIndex + 1 < m_Lyrics.size())
			{
				int NextNodeTime = m_Lyrics[nCurShowIndex+1].StartShowTime;
				if (nTimeElapse > NextNodeTime)
					nCurShowIndex++;
				else 
					break;
			}
		
			int DisplayTime   = 0;
			int DisplayOffset = 0;

			if (nCurShowIndex + 1 < m_Lyrics.size())
			{
				if (nCurShowIndex >= 0)
				{	
					DisplayTime   =	m_Lyrics[nCurShowIndex+1].StartShowTime - m_Lyrics[nCurShowIndex].StartShowTime;
					DisplayOffset = (int)(( float(nTimeElapse - m_Lyrics[nCurShowIndex].StartShowTime)) / DisplayTime * nLineHeight);
				}
				else
				{
					DisplayTime   =	m_Lyrics[nCurShowIndex+1].StartShowTime;
					DisplayOffset = (int)(( float(nTimeElapse )) / DisplayTime * nLineHeight);
				}
			}

			nBeginShowIndex = nCurShowIndex - nTotalLines - 1;

			if (nBeginShowIndex < 0)
				nBeginShowIndex = 0;

			for (unsigned int i = nBeginShowIndex; i < m_Lyrics.size(); i++)
			{
				int top   =  nTotalHeight / 2 - (nCurShowIndex - i) * nLineHeight - DisplayOffset;
				RECT rect = { rcClient.left, top, rcClient.right, top + nLineHeight };
				if (top > nTotalHeight )
					break;
				if(i < nCurShowIndex)
				{
					dwCurTextColor = upRow;				
				}
				else if (i == nCurShowIndex)
				{
					dwCurTextColor = curRow;
				}
				else
				{
					dwCurTextColor = downRow;
				}
				CRenderEngine::DrawText(hDC,m_pManager,rect,m_Lyrics[i].sLyric,dwCurTextColor,0,DT_CENTER | DT_VCENTER |DT_SINGLELINE);
			}

		}
		::SelectClipRgn(hDC, hOldRgn);
		::SelectObject(hDC,hOld);
		::DeleteObject(hFont);
		::DeleteObject(hRgn);	
	}

	void CLrcViewUI::EnableSearchButton(bool enable)
	{
		if (enable && !m_pSearchLrcBtn) {
			m_pSearchLrcBtn = new CButtonUI;
			m_pSearchLrcBtn->SetManager(m_pManager, this, false);
			m_pSearchLrcBtn->SetText(m_sSearchButtonText);
			m_pSearchLrcBtn->SetToolTip(m_sSearchButtonTooltip);
			m_pSearchLrcBtn->SetName(this->GetName()+_T(".search_lrc"));
			m_pSearchLrcBtn->SetFloat(true);
			if (m_pManager) {
				LPCTSTR pDefaultAttributes = m_pManager->GetDefaultAttributeList(_T("Button"));
				if (pDefaultAttributes) {
					m_pSearchLrcBtn->SetAttributeList(pDefaultAttributes);
				}
			}
		}
		else if (!enable && m_pSearchLrcBtn) {
			m_pSearchLrcBtn->Delete();
			m_pSearchLrcBtn = NULL;
		}
		
		NeedUpdate();
	}

}