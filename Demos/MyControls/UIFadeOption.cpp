#include "MyControls.h"
#include "UIFadeOption.h"

#pragma warning (disable : 4355)
namespace DuiLib {

	//REGIST_DUICONTROL(CFadeOptionUI);

	CFadeOptionUI::CFadeOptionUI():CFadeAnimation(this),COptionUI()
	{

	}

	CFadeOptionUI::~CFadeOptionUI()
	{

	}

	LPCTSTR CFadeOptionUI::GetClass() const
	{
		return DUI_CTR_FADEOPTION;
	}

	LPVOID CFadeOptionUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_FADEOPTION) == 0 ) 
			return static_cast<CFadeOptionUI*>(this);
		return COptionUI::GetInterface(pstrName);
	}

	void CFadeOptionUI::DoEvent(TEventUI& event)
	{
		COptionUI::DoEvent( event );
		DoAnimationEvent(event);
	}
	void CFadeOptionUI::PaintStatusImage(HDC hDC)
	{
		{
			if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
			else m_uButtonState &= ~ UISTATE_FOCUSED;
			if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
			else m_uButtonState &= ~ UISTATE_DISABLED;

			if ((m_uButtonState & UISTATE_SELECTED) != 0 )
			{
				if( (m_uButtonState & UISTATE_DISABLED) != 0 ) {
					if( DrawImage(hDC, m_diDisabled) ) goto Label_ForeImage;
				}
				else if( (m_uButtonState & UISTATE_PUSHED) != 0 ) {
					if( DrawImage(hDC, m_diSelectedPushed) ) goto Label_ForeImage;
				}
				else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
					if( DrawImage(hDC, m_diSelectedHot) ) goto Label_ForeImage;
				}

				if( !m_diSelected.sDrawString.IsEmpty() ) {
					CFadeAnimation::PaintAnimationStatusImage(hDC,m_diSelected,m_diSelectedHot,m_uButtonState);
				}
			}
			else
			{
				if( (m_uButtonState & UISTATE_DISABLED) != 0 ) {
					if( DrawImage(hDC, m_diDisabled) ) goto Label_ForeImage;
				}
				else if( (m_uButtonState & UISTATE_PUSHED) != 0 ) {
					if( DrawImage(hDC, m_diPushed) ) goto Label_ForeImage;
				}
				else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
					if( DrawImage(hDC, m_diHot) ) goto Label_ForeImage;
				}
				else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) {
					if( DrawImage(hDC, m_diFocused) ) goto Label_ForeImage;
				}

				if( !m_diNormal.sDrawString.IsEmpty() ) {
					CFadeAnimation::PaintAnimationStatusImage(hDC,m_diNormal,m_diHot,m_uButtonState);
				}
			}		
		}
	
Label_ForeImage:
		DrawImage(hDC, m_diFore);
	}
	void CFadeOptionUI::PaintAnimationButton(HDC hDC)
	{
// 		if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
// 		else m_uButtonState &= ~ UISTATE_FOCUSED;
// 		if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
// 		else m_uButtonState &= ~ UISTATE_DISABLED;
// 
// 		if( (m_uButtonState & UISTATE_DISABLED) != 0 ) {
// 			if( !m_sDisabledImage.IsEmpty() ) {
// 				if( !DrawImage(hDC, (LPCTSTR)m_sDisabledImage) ) m_sDisabledImage.Empty();
// 				else return;
// 			}
// 		}
// 		else if( (m_uButtonState & UISTATE_PUSHED) != 0 ) {
// 			if( !m_sPushedImage.IsEmpty() ) {
// 				if( !DrawImage(hDC, (LPCTSTR)m_sPushedImage) ) m_sPushedImage.Empty();
// 				else return;
// 			}
// 		}
// 		else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) {
// 			if( !m_sFocusedImage.IsEmpty() ) {
// 				if( !DrawImage(hDC, (LPCTSTR)m_sFocusedImage) ) m_sFocusedImage.Empty();
// 				else return;
// 			}
// 		}
// 
// 		if (!IsSelected())
// 		{
// 			CFadeAnimation::PaintAnimationStatusImage(hDC,(LPCTSTR)m_sNormalImage,(LPCTSTR)m_sHotImage,m_uButtonState);
// 		}
// 		else
// 		{
// 			CFadeAnimation::PaintAnimationStatusImage(hDC,(LPCTSTR)m_sSelectedImage,(LPCTSTR)m_sSelectedHotImage,m_uButtonState);
// 		}
			
	}
}