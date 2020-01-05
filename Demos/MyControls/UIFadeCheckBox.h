#ifndef __UIFADECHECKBOX_H__
#define __UIFADECHECKBOX_H__

#pragma once

namespace DuiLib
{
	class MYCONTROLS_API CFadeCheckBoxUI :
		public CFadeOptionUI
	{
		DECLARE_CLASS(CFadeCheckBoxUI)
	public:
		CFadeCheckBoxUI();
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetCheck(bool bCheck,bool bSendMsg = true);
		bool GetCheck() const;
	};
}

#endif // __UIFADECHECKBOX_H__
