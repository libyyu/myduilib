#include "MyControls.h"

DuiLib::CControlUI* MyCreateControlRaw(LPCTSTR pstrClass)
{
	if (_tcscmp(pstrClass, DUI_CTR_FADEBUTTON) == 0) return new DuiLib::CFadeButtonUI;
	else if (_tcscmp(pstrClass, DUI_CTR_FADEOPTION) == 0) return new DuiLib::CFadeOptionUI;
	else if (_tcscmp(pstrClass, DUI_CTR_FADECHECKBOX) == 0) return new DuiLib::CFadeCheckBoxUI;
	else if (_tcscmp(pstrClass, DUI_CTR_ALBUMBUTTON) == 0) return new DuiLib::CAlbumButtonUI;
	//else if (_tcscmp(pstrClass, DUI_CTR_LRCVIEW) == 0) return new DuiLib::CLrcViewUI;
	else if (_tcscmp(pstrClass, DUI_CTR_TEMPLLIST) == 0) return new DuiLib::CTemplListUI;
	else if (_tcscmp(pstrClass, DUI_CTR_TEMPLLIST_ITEM) == 0) return new DuiLib::CTemplListItemUI;
	else if (_tcscmp(pstrClass, DUI_CTR_WKEWEBKIT) == 0) return new DuiLib::CWkeWebkitUI;
	else return NULL;
}
MYCONTROLS_API bool bIsRunByLua = true;
MYCONTROLS_API DuiLib::CControlUI* MyCreateControl(LPCTSTR pstrClass)
{
	if (bIsRunByLua)
	{
		if (_tcscmp(pstrClass, _T("SongListItem")) == 0) return new DuiLib::CTemplListItemUI;
		else if (_tcscmp(pstrClass, _T("PlayListItem")) == 0) return new DuiLib::CTemplListItemUI;
		else if (_tcscmp(pstrClass, DUI_CTR_LRCVIEW) == 0) return new DuiLib::CLrcViewUI;
	}

	return MyCreateControlRaw(pstrClass);
}

extern "C"
{
    MYCONTROLS_API DuiLib::CControlUI*  CreateControl(LPCTSTR pstrClass)
    {
		return MyCreateControl(pstrClass);
    }
}