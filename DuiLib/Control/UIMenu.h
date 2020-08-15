/**************************************************************************
注意：像图标或者子菜单图标以及占位控件等都要设置属性mouse="false"
**************************************************************************/


#ifndef __UIMENU_H__
#define __UIMENU_H__

#ifdef _MSC_VER
#pragma once
#endif

#include "../Utils/observer_impl_base.hpp"

namespace DuiLib {

/////////////////////////////////////////////////////////////////////////////////////
//
struct ContextMenuParam
{
	// 1: remove all
	// 2: remove the sub menu
	WPARAM wParam;
	HWND hWnd;
};

enum MenuAlignment
{
	eMenuAlignment_Left = 1 << 1,
	eMenuAlignment_Top = 1 << 2,
	eMenuAlignment_Right = 1 << 3,
	eMenuAlignment_Bottom = 1 << 4,
};

#pragma warning(push)
#pragma warning(disable:4251)
typedef class ObserverImpl<BOOL, ContextMenuParam> ContextMenuObserver;
typedef class ReceiverImpl<BOOL, ContextMenuParam> ContextMenuReceiver;
#pragma warning(pop) 

UILIB_API extern ContextMenuObserver s_context_menu_observer;

// MenuUI
UILIB_API extern const TCHAR* const kMenuUIClassName;// = _T("MenuUI");
UILIB_API extern const TCHAR* const kMenuUIInterfaceName;// = _T("Menu");

class CListUI;
class UILIB_API CMenuUI : public CListUI
{
	DECLARE_CLASS(CMenuUI)
public:
	CMenuUI();
	~CMenuUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

	virtual void DoEvent(TEventUI& event);

    virtual bool Add(CControlUI* pControl);
    virtual bool AddAt(CControlUI* pControl, int iIndex);

    virtual int GetItemIndex(CControlUI* pControl) const;
    virtual bool SetItemIndex(CControlUI* pControl, int iIndex);
    virtual bool Remove(CControlUI* pControl);

	SIZE EstimateSize(SIZE szAvailable);

protected:
	virtual bool SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
};

/////////////////////////////////////////////////////////////////////////////////////
//

// MenuElementUI
UILIB_API extern const TCHAR* const kMenuElementUIClassName;// = _T("MenuElementUI");
UILIB_API extern const TCHAR* const kMenuElementUIInterfaceName;// = _T("MenuElement);

class CMenuElementUI;
class CMenuWnd;
struct UILIB_API MenuCmdMsg
{
	UINT_PTR    nMenuTag;
	UINT_PTR    nMenuUserData;
	UINT_PTR    nItemTag;
	CDuiString  sItemName;
	CDuiString  sItemText;
	CMenuWnd*   pMenuWnd;
};

#pragma warning(push)
#pragma warning(disable:4251)
class UILIB_API CMenuWnd : public CWindowWnd, public ContextMenuReceiver
{
	DECLARE_CLASS(CMenuWnd)
public:

	CMenuWnd(HWND hParent = NULL);
    void Init(CMenuElementUI* pOwner, STRINGorID xml, LPCTSTR pSkinType, POINT point);
    LPCTSTR GetWindowClassName() const;
    void OnFinalMessage(HWND hWnd);

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	BOOL Receive(ContextMenuParam param);

	void SetTag(UINT_PTR t);
	UINT_PTR GetTag() const;
	void SetUserData(UINT_PTR t);
	UINT_PTR GetUserData() const;
	CPaintManagerUI* GetPaintManager() const;

	BOOL IsEnableItem(LPCTSTR lpszID);
	void EnableItem(LPCTSTR lpszID,BOOL bEnable = TRUE);
public:
	HWND m_hParent;
	POINT m_BasedPoint;
	STRINGorID m_xml;
	CDuiString m_sType;
    CPaintManagerUI m_pm;
    CMenuElementUI* m_pOwner;
    CMenuUI* m_pLayout;
protected:
	UINT_PTR    _tag;
	UINT_PTR    _data;
};
#pragma warning(pop) 

class CListContainerElementUI;
class UILIB_API CMenuElementUI : public CListContainerElementUI
{
	DECLARE_CLASS(CMenuElementUI)
	friend CMenuWnd;
public:
    CMenuElementUI();
	~CMenuElementUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

	bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);

	void DrawItemText(HDC hDC, const RECT& rcItem);

	SIZE EstimateSize(SIZE szAvailable);

	bool Activate();

	void DoEvent(TEventUI& event);


	CMenuWnd* GetMenuWnd();

	void CreateMenuWnd();

protected:
	CMenuWnd* m_pWindow;
};

} // namespace DuiLib

#endif // __UIMENU_H__
