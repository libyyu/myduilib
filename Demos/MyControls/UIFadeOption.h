#ifndef __UIFADEOPTION_H__ 
#define __UIFADEOPTION_H__


#pragma once

namespace DuiLib {
	class MYCONTROLS_API CFadeOptionUI : 
		public COptionUI,
		public CFadeAnimation
	{
		DECLARE_CLASS(CFadeOptionUI)
	public:
		CFadeOptionUI();
		virtual ~CFadeOptionUI();
		
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void DoEvent(TEventUI& event);
		void PaintStatusImage(HDC hDC);
	protected:
		void PaintAnimationButton(HDC hDC);
	};
}
#endif//__UIFADEOPTION_H__