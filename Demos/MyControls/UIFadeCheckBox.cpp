#include "MyControls.h"
#include "UIFadeCheckBox.h"

namespace DuiLib
{
	//REGIST_DUICONTROL(CFadeCheckBoxUI);
	CFadeCheckBoxUI::CFadeCheckBoxUI():CFadeOptionUI()
	{}
	LPCTSTR CFadeCheckBoxUI::GetClass() const
	{
		return DUI_CTR_FADECHECKBOX;
	}

	LPVOID CFadeCheckBoxUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_FADECHECKBOX) == 0 ) 
			return static_cast<CFadeCheckBoxUI*>(this);
		return CFadeOptionUI::GetInterface(pstrName);
	}

	void CFadeCheckBoxUI::SetCheck(bool bCheck,bool bSendMsg /* = true */)
	{
		Selected(bCheck,bSendMsg);
	}

	bool  CFadeCheckBoxUI::GetCheck() const
	{
		return IsSelected();
	}
}
