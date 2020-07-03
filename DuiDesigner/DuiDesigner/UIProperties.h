#pragma once

//////////////////////////////////////////////////////////////////////////
//CMFCPropertyGridColor32Property

#define ARGB(a,r,g,b)        ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16))|(((DWORD)(BYTE)(a))<<24))
#define GetAValue(argb)      (LOBYTE((argb)>>24))

enum MFCPropertyGridType
{
	typeNULL = 0,

	typeColor,
	typeBase,
	typeRect,
	typeSize,
	typeBool,
	typeNumber,
	typeString,
	typeSelect,
	typeImage,
	typeFont,
	typeDefaultAttri,
};

class CMFCPropertyGridPropertyBase : public CMFCPropertyGridProperty
{
public:
	// Group constructor
	CMFCPropertyGridPropertyBase(DuiLib::UIPropertyGridItem& item_, const CString& strGroupName, DWORD_PTR dwData = 0, BOOL bIsValueList = FALSE)
		:CMFCPropertyGridProperty(strGroupName, dwData, bIsValueList), item(item_){}

	// Simple property
	CMFCPropertyGridPropertyBase(DuiLib::UIPropertyGridItem& item_, const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL, LPCTSTR lpszValidChars = NULL)
		: CMFCPropertyGridProperty(strName, varValue, lpszDescr, dwData, lpszEditMask, lpszEditTemplate, lpszValidChars), item(item_) {}


	DuiLib::UIPropertyGridItem& item;
public:
	virtual void SetDataFromString(const CString& lpstrValue);
	virtual CString GetAttributeName() const;


	virtual bool UpdateFromControl(CControlUI* pControl);
	virtual bool UpdateToControl(CControlUI* pControl);
	virtual CString GetValueFromUI(bool useOrigin = false);
};

class CMFCPropertyGridColor32Property : public CMFCPropertyGridColorProperty
{
public:
	CMFCPropertyGridColor32Property(DuiLib::UIPropertyGridItem& item_, const CString& strName,const COLORREF& color,CPalette* pPalette=NULL,LPCTSTR lpszDescr=NULL,DWORD_PTR dwData=0);

public:
	virtual BOOL OnUpdateValue();
	virtual void OnDrawValue(CDC* pDC, CRect rect);
	virtual CString FormatProperty();

	DuiLib::UIPropertyGridItem& item;
public:
	virtual void SetDataFromString(const CString& lpstrValue);
	virtual CString GetAttributeName() const;

	virtual bool UpdateFromControl(CControlUI* pControl);
	virtual bool UpdateToControl(CControlUI* pControl);
	virtual CString GetValueFromUI(bool useOrigin = false);
};

//////////////////////////////////////////////////////////////////////////
//CMFCPropertyGridRectProperty
class CMFCPropertyGridRectProperty : public CMFCPropertyGridPropertyBase
{
public:
	CMFCPropertyGridRectProperty(DuiLib::UIPropertyGridItem& item_, const CString& strName, DWORD_PTR dwData = 0,
		LPCTSTR lpszNameLeft = NULL, LPCTSTR lpszNameTop = NULL, LPCTSTR lpszNameRight = NULL, LPCTSTR lpszNameButtom = NULL,
		LPCTSTR lpszDescrLeft = NULL, LPCTSTR lpszDescrTop = NULL, LPCTSTR lpszDescrRight = NULL, LPCTSTR lpszDescrButtom = NULL);

public:

	void UpdateValue(const RECT& rect);

	virtual void SetDataFromString(const CString& lpstrValue);
	virtual CString FormatProperty();

	virtual bool UpdateFromControl(CControlUI* pControl);
	virtual bool UpdateToControl(CControlUI* pControl);
	virtual CString GetValueFromUI(bool useOrigin = false);
};
//////////////////////////////////////////////////////////////////////////
//CMFCPropertyGridSizeProperty
class CMFCPropertyGridSizeProperty : public CMFCPropertyGridPropertyBase
{
public:
	CMFCPropertyGridSizeProperty(DuiLib::UIPropertyGridItem& item_, const CString& strName, DWORD_PTR dwData = 0,
		LPCTSTR lpszNameCX = NULL, LPCTSTR lpszNameCY = NULL, 
		LPCTSTR lpszDescrCX = NULL, LPCTSTR lpszDescrCY = NULL);

public:

	void UpdateValue(const SIZE& sz);
	virtual void SetDataFromString(const CString& lpstrValue);
	virtual CString FormatProperty();

	virtual bool UpdateFromControl(CControlUI* pControl);
	virtual bool UpdateToControl(CControlUI* pControl);
	virtual CString GetValueFromUI(bool useOrigin = false);
};
//////////////////////////////////////////////////////////////////////////
//CMFCPropertyGridRectProperty
class CMFCPropertyGridBoolProperty : public CMFCPropertyGridPropertyBase
{
public:
	CMFCPropertyGridBoolProperty(DuiLib::UIPropertyGridItem& item_, const CString& strName, bool b, DWORD_PTR dwData = 0, LPCTSTR lpszDescrLeft = NULL);

public:

	void UpdateValue(bool b);

	virtual void SetDataFromString(const CString& lpstrValue);
	virtual CString FormatProperty();

	virtual bool UpdateFromControl(CControlUI* pControl);
	virtual bool UpdateToControl(CControlUI* pControl);
};
//////////////////////////////////////////////////////////////////////////
//CMFCPropertyGridNumberProperty
class CMFCPropertyGridNumberProperty : public CMFCPropertyGridPropertyBase
{
public:
	CMFCPropertyGridNumberProperty(DuiLib::UIPropertyGridItem& item_, const CString& strName, LONG, LPCTSTR lpszDescrLeft = NULL, DWORD_PTR dwData = 0)
		: CMFCPropertyGridPropertyBase(item_, strName, (_variant_t)(LONG)0, lpszDescrLeft, dwData)
	{

	}

	virtual void SetDataFromString(const CString& lpstrValue);

	virtual bool UpdateFromControl(CControlUI* pControl);
};
//////////////////////////////////////////////////////////////////////////
//CMFCPropertyGridStringProperty
class CMFCPropertyGridStringProperty : public CMFCPropertyGridPropertyBase
{
public:
	CMFCPropertyGridStringProperty(DuiLib::UIPropertyGridItem& item_, const CString& strName, LPCTSTR v, LPCTSTR lpszDescrLeft = NULL, DWORD_PTR dwData = 0)
		: CMFCPropertyGridPropertyBase(item_, strName, (_variant_t)v, lpszDescrLeft, dwData)
	{
	}
};

//////////////////////////////////////////////////////////////////////////
//CMFCPropertyGridStringSelectProperty
class CMFCPropertyGridStringSelectProperty : public CMFCPropertyGridPropertyBase
{
public:
	CMFCPropertyGridStringSelectProperty(DuiLib::UIPropertyGridItem& item, const CString& strName, const std::vector<CString>& valuelist , const CString& defaultv, DWORD_PTR dwData = 0, LPCTSTR lpszDescrLeft = NULL);

public:
	std::vector<CString> m_valuelist;

	void UpdateValue(LPCTSTR v);
	void UpdateValue(int i);

	virtual void SetDataFromString(const CString& lpstrValue);
	virtual CString FormatProperty();
};

//////////////////////////////////////////////////////////////////////////
//CMFCPropertyGridImageProperty

#define AFX_PROP_HAS_BUTTON 0x0002

class CMFCPropertyGridImageProperty : public CMFCPropertyGridPropertyBase
{
	DECLARE_DYNAMIC(CMFCPropertyGridImageProperty)

public:
	CMFCPropertyGridImageProperty(DuiLib::UIPropertyGridItem& item_, const CString& strName, const CString& strImage, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);

public:
	virtual void OnClickButton(CPoint point);
};

//////////////////////////////////////////////////////////////////////////
//CMFCPropertyGridCustomFontsProperty

class CMFCPropertyGridCustomFontsProperty : public CMFCPropertyGridPropertyBase
{
	DECLARE_DYNAMIC(CMFCPropertyGridCustomFontsProperty)

public:
	CMFCPropertyGridCustomFontsProperty(DuiLib::UIPropertyGridItem& item_, const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);

public:
	virtual void OnClickButton(CPoint point);
};

//////////////////////////////////////////////////////////////////////////
//CMFCPropertyGridDefaultAttribListProperty

class CMFCPropertyGridDefaultAttribListProperty : public CMFCPropertyGridPropertyBase
{
	DECLARE_DYNAMIC(CMFCPropertyGridDefaultAttribListProperty)

public:
	CMFCPropertyGridDefaultAttribListProperty(DuiLib::UIPropertyGridItem& item_, const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);

public:
	virtual void OnClickButton(CPoint point);
};

//////////////////////////////////////////////////////////////////////////
//CPropertiesToolBar

class CPropertiesToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

//////////////////////////////////////////////////////////////////////////
//CUIProperties

extern UINT WM_UI_PROPERTY_CHANGED;

class CUIProperties : public CWnd
{
	DECLARE_DYNAMIC(CUIProperties)

public:
	CUIProperties(void);
	virtual ~CUIProperties(void);

public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	void SetVSDotNetLook(BOOL bSet);
	void AdjustLayout();

	void ShowProperty(CControlUI* pControl);
	CControlUI* GetCurUI() const { return m_pControl; }

	CMFCPropertyGridProperty* FindPropByData(DWORD_PTR dwData, BOOL bSearchSubProps = TRUE) const;
	static CString FormatOrigProperty(CMFCPropertyGridProperty* pProp, int nType);

protected:
	void InitPropList();
	void SetPropListFont();

	void HideAllProperties(BOOL bAdjustLayout=FALSE);

	//Control Property
	void ShowControlProperty(CControlUI* pControl);
protected:
	CMFCPropertyGridCtrl m_wndPropList;
	CPropertiesToolBar m_wndToolBar;
	CFont m_fntPropList;

	std::vector<DuiLib::UIPropertyGrid> m_GridProperty;
	CControlUI* m_pControl;//save the control pointer

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnPropertyChanged(WPARAM wp, LPARAM lp);
	afx_msg void OnPropertiesSort();
	afx_msg void OnUpdatePropertiesSort(CCmdUI* pCmdUI);
	afx_msg void OnPropertiesAlphaBetic();
	afx_msg void OnUpdatePropertiesAlphaBetic(CCmdUI* pCmdUI);
};