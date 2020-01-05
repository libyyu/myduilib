#include "MyControls.h"
#include "UIFadeButton.h"

#pragma warning (disable : 4355)
namespace DuiLib {

	//REGIST_DUICONTROL(CFadeButtonUI);

	CFadeButtonUI::CFadeButtonUI() : CButtonUI(),CFadeAnimation( this )
	{
	}

	CFadeButtonUI::~CFadeButtonUI()
	{
		
	}

	LPCTSTR CFadeButtonUI::GetClass() const
	{
		return DUI_CTR_FADEBUTTON;
	}

	LPVOID CFadeButtonUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_FADEBUTTON) == 0 ) 
			return static_cast<CFadeButtonUI*>(this);
		return CButtonUI::GetInterface(pstrName);
	}

// 	void CFadeButtonUI::SetNormalImage(LPCTSTR pStrImage)
// 	{
// 		m_sNormalImage = pStrImage;
// 		m_sLastImage = m_sNormalImage;
// 	}

	void CFadeButtonUI::DoEvent(TEventUI& event)
	{
		CButtonUI::DoEvent( event );
		DoAnimationEvent(event);
	}

	void CFadeButtonUI::PaintStatusImage(HDC hDC)
	{
		if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
		else m_uButtonState &= ~ UISTATE_FOCUSED;
		if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
		else m_uButtonState &= ~ UISTATE_DISABLED;

		if( (m_uButtonState & UISTATE_DISABLED) != 0 ) {
			if (DrawImage(hDC, m_diDisabled)) goto PaintAnimation;
		}
		else if( (m_uButtonState & UISTATE_PUSHED) != 0 ) {
			if (DrawImage(hDC, m_diPushed)) goto PaintAnimation;
		}
		else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) {
			if (DrawImage(hDC, m_diFocused)) goto PaintAnimation;
		}
PaintAnimation:
		CFadeAnimation::PaintAnimationStatusImage(hDC,m_diNormal,m_diHot,m_uButtonState);
	}

// 	void CFadeButtonUI::OnAnimationStep(INT nTotalFrame, INT nCurFrame, INT nAnimationID)
// 	{
// 		m_bFadeAlpha = (BYTE)((nCurFrame / (double)nTotalFrame) * 255);
// 		m_bFadeAlpha = m_bFadeAlpha == 0?10:m_bFadeAlpha;
// 		Invalidate();
// 	}


} // namespace DuiLib