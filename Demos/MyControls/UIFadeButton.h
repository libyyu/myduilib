#ifndef __UIFADEBUTTON_H__
#define __UIFADEBUTTON_H__

#pragma once

namespace DuiLib {
	
	class MYCONTROLS_API CFadeButtonUI : public CButtonUI, public CFadeAnimation
	{
		DECLARE_CLASS(CFadeButtonUI)
	public:
		CFadeButtonUI();
		virtual ~CFadeButtonUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void DoEvent(TEventUI& event);
		void PaintStatusImage(HDC hDC);
	};

} // namespace UiLib

#endif // __UIFADEBUTTON_H__