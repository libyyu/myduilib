#ifndef __UIHOTKEYKEDIT_H__
#define __UIHOTKEYKEDIT_H__
#pragma once

namespace DuiLib
{
	class CHotKeyWnd;
	
	class UILIB_API CHotKeyUI : public CLabelUI
	{
		DECLARE_CLASS(CHotKeyUI)
		friend CHotKeyWnd;
	public:
		CHotKeyUI();
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;
		void SetEnabled(bool bEnable = true);
		void SetText(LPCTSTR pstrText);
		LPCTSTR GetNormalImage();
		void SetNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetHotImage();
		void SetHotImage(LPCTSTR pStrImage);
		LPCTSTR GetFocusedImage();
		void SetFocusedImage(LPCTSTR pStrImage);
		LPCTSTR GetDisabledImage();
		void SetDisabledImage(LPCTSTR pStrImage);
		void SetNativeBkColor(DWORD dwBkColor);
		DWORD GetNativeBkColor() const;


		void SetPos(RECT rc);
		void SetVisible(bool bVisible = true);
		void SetInternVisible(bool bVisible = true);
		SIZE EstimateSize(SIZE szAvailable);
		void DoEvent(TEventUI& event);
	protected:
		virtual bool SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	public:
		void PaintStatusImage(HDC hDC);
		void PaintText(HDC hDC);
	public:
		void GetHotKey(WORD &wVirtualKeyCode, WORD &wModifiers) const;
		DWORD GetHotKey() const;
		void SetHotKey(WORD wVirtualKeyCode, WORD wModifiers);
		void SetHotKey(DWORD hotkey);
	protected:
		CHotKeyWnd * m_pWindow;
		UINT m_uButtonState;
		TDrawInfo m_diNormalImage;
		TDrawInfo m_diHotImage;
		TDrawInfo m_diFocusedImage;
		TDrawInfo m_diDisabledImage;
		DWORD m_dwHotKeybkColor;
	protected:
		WORD m_wVirtualKeyCode;
		WORD m_wModifiers;
	};


}
#endif // __UIHOTKEYKEDIT_H__