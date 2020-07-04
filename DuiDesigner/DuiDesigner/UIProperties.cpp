#include "StdAfx.h"
#include "UIProperties.h"
#include "DuiDesigner.h"

#include "UIDesignerView.h"
#include "ImageDialog.h"
#include "DialogCustomFonts.h"
#include "DialogDefaultAttribList.h"
#include "UIUtil.h"
using DuiLib::TPercentInfo;
using DuiLib::IListOwnerUI;

static CString FormatVariant(const COleVariant& var)
{
	CString strVal;

	switch (var.vt)
	{
	case VT_BSTR:
		strVal = var.bstrVal;
		break;

	case VT_I2:
		strVal.Format(_T("%d"), (short)var.iVal);
		break;

	case VT_I4:
	case VT_INT:
		strVal.Format(_T("%ld"), (long)var.lVal);
		break;

	case VT_UI1:
		if ((BYTE)var.bVal != 0)
		{
			strVal.Format(_T("%c"), (TCHAR)(BYTE)var.bVal);
		}
		break;

	case VT_UI2:
		strVal.Format(_T("%u"), var.uiVal);
		break;

	case VT_UINT:
	case VT_UI4:
		strVal.Format(_T("%u"), var.ulVal);
		break;

	case VT_R4:
		strVal.Format(_T("%f"), (float)var.fltVal);
		break;

	case VT_R8:
		strVal.Format(_T("%lf"), (double)var.dblVal);
		break;

	case VT_BOOL:
		strVal = var.boolVal == VARIANT_TRUE ? _T("True") : _T("False");
		break;

	default:
		// Unsupported type
		ASSERT(FALSE && "Unsupported type");
		strVal = _T("*** error ***");
	}

	return strVal;
}

void CMFCPropertyGridPropertyBase::SetDataFromString(const CString& pstrValue)
{
	SetValue((_variant_t)pstrValue);
	SetOriginalValue((_variant_t)pstrValue);
}
CString CMFCPropertyGridPropertyBase::GetAttributeName() const
{
	CString value = UTF8_TO_CSTRING(item.title.c_str());
	value.MakeLower();
	return value;
}
bool CMFCPropertyGridPropertyBase::UpdateFromControl(CControlUI* pControl)
{
	ASSERT(pControl);
	CString key = GetAttributeName();

	DuiLib::CDuiString value = pControl->GetXMLAttribute(key);
	if (value.IsEmpty())
		return false;

	CString cvalue = value.GetData();
	SetDataFromString(cvalue);

	return true;
}
bool CMFCPropertyGridPropertyBase::UpdateToControl(CControlUI* pControl)
{
	ASSERT(pControl);
	CString key = GetAttributeName();
	CString strNewValue = GetValueFromUI(false);
	bool ret = pControl->SetXMLAttribute(key, strNewValue);
	if (!ret)
		return false;

	SetOriginalValue(GetValue());

	return true;
}
CString CMFCPropertyGridPropertyBase::GetValueFromUI(bool useOrigin)
{
	COleVariant val = useOrigin? GetOriginalValue() : GetValue();
	return FormatVariant(val);
}
//////////////////////////////////////////////////////////////////////////
//CMFCPropertyGridColor32Property

CMFCPropertyGridColor32Property::CMFCPropertyGridColor32Property(DuiLib::UIPropertyGridItem& item_, const CString& strName, const COLORREF& color,CPalette* pPalette/*=NULL*/,LPCTSTR lpszDescr/*=NULL*/,DWORD_PTR dwData/*=0*/)
:CMFCPropertyGridColorProperty(strName,color,pPalette,lpszDescr,dwData), item(item_)
{
	EnableOtherButton(_T("其他..."));
	EnableAutomaticButton(_T("默认"), ::GetSysColor(COLOR_3DFACE));
}
BOOL CMFCPropertyGridColor32Property::OnUpdateValue()
{
	ASSERT_VALID(this);

	if (m_pWndInPlace == NULL)
	{
		return FALSE;
	}

	ASSERT_VALID(m_pWndInPlace);
	ASSERT(::IsWindow(m_pWndInPlace->GetSafeHwnd()));

	CString strText;
	m_pWndInPlace->GetWindowText(strText);

	COLORREF colorCurr = m_Color;
	int nA = 0,nR = 0, nG = 0, nB = 0;
	_stscanf_s(strText, _T("%2x%2x%2x%2x"), &nA, &nR, &nG, &nB);
	if(nA == 0 && (nR !=0 || nG != 0 || nB !=  0))
		nA = 0xFF;
	m_Color = ARGB(nA, nR, nG, nB);

	if (colorCurr != m_Color)
	{
		m_pWndList->OnPropertyChanged(this);
	}

	return TRUE;
}
void CMFCPropertyGridColor32Property::OnDrawValue(CDC* pDC, CRect rect)
{
	CRect rectColor = rect;

	rect.left += rect.Height();
	CMFCPropertyGridProperty::OnDrawValue(pDC, rect);

	rectColor.right = rectColor.left + rectColor.Height();
	rectColor.DeflateRect(1, 1);
	rectColor.top++;
	rectColor.left++;

	CBrush br(m_Color == (COLORREF)-1 ? m_ColorAutomatic : (m_Color&0x00FFFFFF));
	pDC->FillRect(rectColor, &br);
	pDC->Draw3dRect(rectColor, 0, 0);
}
CString CMFCPropertyGridColor32Property::FormatProperty()
{
	ASSERT_VALID(this);

	CString str;
	str.Format(_T("%02x%02x%02x%02x"),GetAValue(m_Color),GetRValue(m_Color),GetGValue(m_Color),GetBValue(m_Color));

	return str;
}
void CMFCPropertyGridColor32Property::SetDataFromString(const CString& strValue)
{
	ASSERT_VALID(this);
	LPCTSTR pstrValue = strValue;
	while (*pstrValue > _T('\0') && *pstrValue <= _T(' ')) pstrValue = ::CharNext(pstrValue);
	if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
	LPTSTR pstr = NULL;
	DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
	
	SetColor((_variant_t)(LONG)(clrColor));
	SetOriginalValue((_variant_t)(LONG)(clrColor));
}
CString CMFCPropertyGridColor32Property::GetAttributeName() const
{
	CString value = UTF8_TO_CSTRING(item.title.c_str());
	value.MakeLower();
	return value;
}
bool CMFCPropertyGridColor32Property::UpdateFromControl(CControlUI* pControl)
{
	ASSERT(pControl);
	CString key = GetAttributeName();
	DuiLib::CDuiString value = pControl->GetXMLAttribute(key);
	CString cvalue = value.GetData();
	SetDataFromString(cvalue);

	return true;
}
bool CMFCPropertyGridColor32Property::UpdateToControl(CControlUI* pControl)
{
	ASSERT(pControl);
	CString key = GetAttributeName();
	CString strNewValue = GetValueFromUI(false);
	bool ret = pControl->SetXMLAttribute(key, strNewValue);
	if (!ret)
		return false;

	SetOriginalValue(GetValue());

	return true;
}
CString CMFCPropertyGridColor32Property::GetValueFromUI(bool useOrigin)
{
	if (useOrigin) {
		CString str;
		str.Format(_T("%02x%02x%02x%02x"), GetAValue(m_ColorOrig), GetRValue(m_ColorOrig), GetGValue(m_ColorOrig), GetBValue(m_ColorOrig));
		return str;
	}
	else {
		CString str;
		str.Format(_T("%02x%02x%02x%02x"), GetAValue(m_Color), GetRValue(m_Color), GetGValue(m_Color), GetBValue(m_Color));
		return str;
	}
}
//////////////////////////////////////////////////////////////////////////
//CMFCPropertyGridRectProperty
CMFCPropertyGridRectProperty::CMFCPropertyGridRectProperty(DuiLib::UIPropertyGridItem& item_, const CString& strName, DWORD_PTR dwData /* = 0 */,
	LPCTSTR lpszNameLeft, LPCTSTR lpszNameTop, LPCTSTR lpszNameRight, LPCTSTR lpszNameBottom,
	LPCTSTR lpszDescrLeft, LPCTSTR lpszDescrTop, LPCTSTR lpszDescrRight, LPCTSTR lpszDescrBottom)
	: CMFCPropertyGridPropertyBase(item_, strName, dwData, TRUE)
{
	CMFCPropertyGridProperty* pProp = NULL;
	pProp = new CMFCPropertyGridProperty(lpszNameLeft ? lpszNameLeft : _T("Left"), (_variant_t)(LONG)0, lpszDescrLeft ? lpszDescrLeft : _T("Left位置"));
	AddSubItem(pProp);
	pProp = new CMFCPropertyGridProperty(lpszNameTop ? lpszNameTop : _T("Top"), (_variant_t)(LONG)0, lpszDescrTop ? lpszDescrTop : _T("Top位置"));
	AddSubItem(pProp);
	pProp = new CMFCPropertyGridProperty(lpszNameRight ? lpszNameRight : _T("Right"), (_variant_t)(LONG)0, lpszDescrRight ? lpszDescrRight :  _T("Right位置"));
	AddSubItem(pProp);
	pProp = new CMFCPropertyGridProperty(lpszNameBottom ? lpszNameBottom : _T("Bottom"), (_variant_t)(LONG)0, lpszDescrBottom ? lpszDescrBottom : _T("Bottom位置"));
	AddSubItem(pProp);
}
void CMFCPropertyGridRectProperty::UpdateValue(const RECT& rect)
{
	GetSubItem(0)->SetValue((_variant_t)(LONG)rect.left);
	GetSubItem(1)->SetValue((_variant_t)(LONG)rect.top);
	GetSubItem(2)->SetValue((_variant_t)(LONG)rect.right);
	GetSubItem(3)->SetValue((_variant_t)(LONG)rect.bottom);
	GetSubItem(0)->SetOriginalValue((_variant_t)(LONG)rect.left);
	GetSubItem(1)->SetOriginalValue((_variant_t)(LONG)rect.top);
	GetSubItem(2)->SetOriginalValue((_variant_t)(LONG)rect.right);
	GetSubItem(3)->SetOriginalValue((_variant_t)(LONG)rect.bottom);
}
void CMFCPropertyGridRectProperty::SetDataFromString(const CString& strValue)
{
	ASSERT_VALID(this);
	RECT rect = { 0 };
	LPCTSTR pstrValue = strValue;
	LPTSTR pstr = NULL;
	rect.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
	rect.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
	rect.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
	rect.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
	UpdateValue(rect);
}
CString CMFCPropertyGridRectProperty::FormatProperty()
{
	return CMFCPropertyGridPropertyBase::FormatProperty();
}
bool CMFCPropertyGridRectProperty::UpdateFromControl(CControlUI* pControl)
{
	if (CMFCPropertyGridPropertyBase::UpdateFromControl(pControl))
		return true;

	for (int i = 0; i < 4; ++i)
	{
		DuiLib::UIPropertyGridItem& subitem = item.childs[i];
		CString key = UTF8_TO_CSTRING(subitem.title.c_str()); key.MakeLower();
		DuiLib::CDuiString value = pControl->GetXMLAttribute(key);
		GetSubItem(i)->SetValue((_variant_t)(LONG)_ttoi(value.GetData()));
		GetSubItem(i)->SetOriginalValue((_variant_t)(LONG)_ttoi(value.GetData()));
	}
	
	return true;
}
bool CMFCPropertyGridRectProperty::UpdateToControl(CControlUI* pControl)
{
	if (CMFCPropertyGridPropertyBase::UpdateToControl(pControl))
		return true;

	ASSERT(pControl);
	for (int i = 0; i < 4; ++i)
	{
		DuiLib::UIPropertyGridItem& subitem = item.childs[i];
		CString key = UTF8_TO_CSTRING(subitem.title.c_str()); key.MakeLower();
		COleVariant val = GetSubItem(i)->GetValue();
		CString strNewValue = FormatVariant(val);

		pControl->SetXMLAttribute(key, strNewValue);
		GetSubItem(i)->SetOriginalValue(val);
	}
	SetOriginalValue(GetValue());
	return true;
}
CString CMFCPropertyGridRectProperty::GetValueFromUI(bool useOrigin)
{
	CString sReturn;
	for (int i=0;i<4;++i)
	{
		COleVariant val = useOrigin ? GetSubItem(i)->GetOriginalValue() : GetSubItem(i)->GetValue();
		sReturn += FormatVariant(val);
		if (i != 4 - 1)
		{
			sReturn += _T(',');
		}
	}
	return sReturn;
}
//////////////////////////////////////////////////////////////////////////
//CMFCPropertyGridSizeProperty
CMFCPropertyGridSizeProperty::CMFCPropertyGridSizeProperty(DuiLib::UIPropertyGridItem& item_, const CString& strName, DWORD_PTR dwData /* = 0 */, LPCTSTR lpszNameCX, LPCTSTR lpszNameCY, LPCTSTR lpszDescrCX /* = NULL */, LPCTSTR lpszDescrCY)
	:CMFCPropertyGridPropertyBase(item_, strName, dwData, TRUE)
{
	CMFCPropertyGridProperty* pProp = NULL;
	pProp = new CMFCPropertyGridProperty(lpszNameCX ? lpszNameCX : _T("Width"), (_variant_t)(LONG)0, lpszDescrCX ? lpszDescrCX: _T("宽度"));
	AddSubItem(pProp);
	pProp = new CMFCPropertyGridProperty(lpszNameCY ? lpszNameCY : _T("Height"), (_variant_t)(LONG)0, lpszDescrCY ? lpszDescrCY: _T("高度"));
	AddSubItem(pProp);
}
void CMFCPropertyGridSizeProperty::UpdateValue(const SIZE& sz)
{
	GetSubItem(0)->SetValue((_variant_t)(LONG)sz.cx);
	GetSubItem(0)->SetOriginalValue((_variant_t)(LONG)sz.cx);
	GetSubItem(1)->SetValue((_variant_t)(LONG)sz.cy);
	GetSubItem(1)->SetOriginalValue((_variant_t)(LONG)sz.cy);
}
void CMFCPropertyGridSizeProperty::SetDataFromString(const CString& strValue)
{
	ASSERT_VALID(this);
	SIZE cxySize = { 0 };
	LPCTSTR pstrValue = strValue;
	LPTSTR pstr = NULL;
	cxySize.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
	cxySize.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
	UpdateValue(cxySize);
}
CString CMFCPropertyGridSizeProperty::FormatProperty()
{
	return CMFCPropertyGridPropertyBase::FormatProperty();
}
bool CMFCPropertyGridSizeProperty::UpdateFromControl(CControlUI* pControl)
{
	if (CMFCPropertyGridPropertyBase::UpdateFromControl(pControl))
		return true;

	for (int i = 0; i < 2; ++i)
	{
		DuiLib::UIPropertyGridItem& subitem = item.childs[i];
		CString key = UTF8_TO_CSTRING(subitem.title.c_str()); key.MakeLower();
		DuiLib::CDuiString value = pControl->GetXMLAttribute(key);
		GetSubItem(i)->SetValue((_variant_t)(LONG)_ttoi(value.GetData()));
		GetSubItem(i)->SetOriginalValue((_variant_t)(LONG)_ttoi(value.GetData()));
	}

	return true;
}
bool CMFCPropertyGridSizeProperty::UpdateToControl(CControlUI* pControl)
{
	if (CMFCPropertyGridPropertyBase::UpdateToControl(pControl))
		return true;

	ASSERT(pControl);
	for (int i = 0; i < 2; ++i)
	{
		DuiLib::UIPropertyGridItem& subitem = item.childs[i];
		CString key = UTF8_TO_CSTRING(subitem.title.c_str()); key.MakeLower();
		COleVariant val = GetSubItem(i)->GetValue();
		CString strNewValue = FormatVariant(val);

		pControl->SetXMLAttribute(key, strNewValue);
		GetSubItem(i)->SetOriginalValue(val);
	}
	SetOriginalValue(GetValue());

	return true;
}
CString CMFCPropertyGridSizeProperty::GetValueFromUI(bool useOrigin)
{
	CString sReturn;
	for (int i = 0; i < 2; ++i)
	{
		COleVariant val = useOrigin ? GetSubItem(i)->GetOriginalValue() : GetSubItem(i)->GetValue();
		sReturn += FormatVariant(val);
		if (i != 2 - 1)
		{
			sReturn += _T(',');
		}
	}
	return sReturn;
}
//////////////////////////////////////////////////////////////////////////
//CMFCPropertyGridBoolProperty
CMFCPropertyGridBoolProperty::CMFCPropertyGridBoolProperty(DuiLib::UIPropertyGridItem& item_, const CString& strName, bool b, DWORD_PTR dwData /* = 0 */, LPCTSTR lpszDescrLeft /* = NULL */)
	: CMFCPropertyGridPropertyBase(item_, strName, b ? _T("True") : _T("False"), lpszDescrLeft, dwData)
{
	AddOption(_T("False"));
	AddOption(_T("True"));
	AllowEdit(FALSE);
}
void CMFCPropertyGridBoolProperty::UpdateValue(bool b)
{
	CString value = _T("False");
	if (b) value = _T("True");
	SetValue((_variant_t)value);
	SetOriginalValue((_variant_t)value);
}
void CMFCPropertyGridBoolProperty::SetDataFromString(const CString& strValue)
{
	ASSERT_VALID(this);
	LPCTSTR pstrValue = strValue;
	bool b = _tcsicmp(pstrValue, _T("true")) == 0;
	UpdateValue(b);
}
CString CMFCPropertyGridBoolProperty::FormatProperty()
{
	return CMFCPropertyGridPropertyBase::FormatProperty();
}
bool CMFCPropertyGridBoolProperty::UpdateFromControl(CControlUI* pControl)
{
	if (CMFCPropertyGridPropertyBase::UpdateFromControl(pControl))
		return true;

	return false;
}
bool CMFCPropertyGridBoolProperty::UpdateToControl(CControlUI* pControl)
{
	ASSERT(pControl);
	CString key = GetAttributeName();
	CString strNewValue = GetValueFromUI(false);
	strNewValue.MakeLower();
	bool ret = pControl->SetXMLAttribute(key, strNewValue);
	if (!ret)
		return false;

	SetOriginalValue(GetValue());

	return true;
}
//////////////////////////////////////////////////////////////////////////
//CMFCPropertyGridNumberProperty
void CMFCPropertyGridNumberProperty::SetDataFromString(const CString& strValue)
{
	LPCTSTR pstrValue = strValue;
	LONG dw = _ttoi(pstrValue);
	SetValue((_variant_t)dw);
	SetOriginalValue((_variant_t)dw);
}
bool CMFCPropertyGridNumberProperty::UpdateFromControl(CControlUI* pControl)
{
	if (CMFCPropertyGridPropertyBase::UpdateFromControl(pControl))
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////
//CMFCPropertyGridStringSelectProperty
CMFCPropertyGridStringSelectProperty::CMFCPropertyGridStringSelectProperty(DuiLib::UIPropertyGridItem& item_, const CString& strName, const std::vector<CString>& valuelist, const CString& defaultv, DWORD_PTR dwData /* = 0 */, LPCTSTR lpszDescrLeft /* = NULL */)
	: CMFCPropertyGridPropertyBase(item_, strName, (_variant_t)defaultv, lpszDescrLeft, dwData)
{
	for (size_t i = 0; i < valuelist.size(); ++i)
	{
		m_valuelist.push_back(valuelist[i]);
		AddOption(valuelist[i]);
	}

	AllowEdit(FALSE);
}
void CMFCPropertyGridStringSelectProperty::UpdateValue(LPCTSTR v)
{
	SetValue((_variant_t)v);
	SetOriginalValue((_variant_t)v);
}
void CMFCPropertyGridStringSelectProperty::UpdateValue(int i)
{
	ASSERT(i >= 0 && i < GetOptionCount());
	LPCTSTR v = GetOption(i);
	UpdateValue(v);
}
void CMFCPropertyGridStringSelectProperty::SetDataFromString(const CString& strValue)
{
	SetValue((_variant_t)strValue);
	SetOriginalValue((_variant_t)strValue);
}
CString CMFCPropertyGridStringSelectProperty::FormatProperty()
{
	return CMFCPropertyGridPropertyBase::FormatProperty();
}

//////////////////////////////////////////////////////////////////////////
//CMFCPropertyGridImageProperty

IMPLEMENT_DYNAMIC(CMFCPropertyGridImageProperty, CMFCPropertyGridProperty)

CMFCPropertyGridImageProperty::CMFCPropertyGridImageProperty(DuiLib::UIPropertyGridItem& item_, const CString& strName, const CString& strImage, LPCTSTR lpszDescr/* = NULL*/, DWORD_PTR dwData/* = 0*/)
: CMFCPropertyGridPropertyBase(item_, strName, COleVariant((LPCTSTR)strImage), lpszDescr, dwData)
{
	m_dwFlags = AFX_PROP_HAS_BUTTON;

	AllowEdit(FALSE);
}

void CMFCPropertyGridImageProperty::OnClickButton(CPoint point)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndList);
	ASSERT_VALID(m_pWndInPlace);
	ASSERT(::IsWindow(m_pWndInPlace->GetSafeHwnd()));

	m_bButtonIsDown = TRUE;
	Redraw();

	CString strImage = StringConvertor::ToMFCString(m_varValue.bstrVal);
	CImageDialog dlg(strImage);
	if(dlg.DoModal()==IDOK)
	{
		strImage = dlg.GetImage();
	}
	strImage.Replace(_T("\\"), _T("/"));
	if (m_pWndInPlace != NULL)
	{
		m_pWndInPlace->SetWindowText(strImage);
	}
	m_varValue = (LPCTSTR) strImage;

	m_bButtonIsDown = FALSE;
	Redraw();

	if (m_pWndInPlace != NULL)
	{
		m_pWndInPlace->SetFocus();
	}
	else
	{
		m_pWndList->SetFocus();
	}
}

//////////////////////////////////////////////////////////////////////////
//CMFCPropertyGridCustomFontsProperty

IMPLEMENT_DYNAMIC(CMFCPropertyGridCustomFontsProperty, CMFCPropertyGridProperty)

CMFCPropertyGridCustomFontsProperty::CMFCPropertyGridCustomFontsProperty(DuiLib::UIPropertyGridItem& item_, const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr/* = NULL*/, DWORD_PTR dwData/* = 0*/)
: CMFCPropertyGridPropertyBase(item_, strName, varValue, lpszDescr, dwData)
{
	m_dwFlags = AFX_PROP_HAS_BUTTON;
	AllowEdit(FALSE);
}

void CMFCPropertyGridCustomFontsProperty::OnClickButton(CPoint point)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndList);
	ASSERT_VALID(m_pWndInPlace);
	ASSERT(::IsWindow(m_pWndInPlace->GetSafeHwnd()));

	m_bButtonIsDown = TRUE;
	Redraw();

	CDialogCustomFonts dlg;
	dlg.DoModal();

	m_bButtonIsDown = FALSE;
	Redraw();

	if (m_pWndInPlace != NULL)
	{
		m_pWndInPlace->SetFocus();
	}
	else
	{
		m_pWndList->SetFocus();
	}
}

//////////////////////////////////////////////////////////////////////////
//CMFCPropertyGridDefaultAttribListProperty

IMPLEMENT_DYNAMIC(CMFCPropertyGridDefaultAttribListProperty, CMFCPropertyGridProperty)

CMFCPropertyGridDefaultAttribListProperty::CMFCPropertyGridDefaultAttribListProperty(DuiLib::UIPropertyGridItem& item_, const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr/* = NULL*/, DWORD_PTR dwData/* = 0*/)
: CMFCPropertyGridPropertyBase(item_, strName, varValue, lpszDescr, dwData)
{
	m_dwFlags = AFX_PROP_HAS_BUTTON;
	AllowEdit(FALSE);
}

void CMFCPropertyGridDefaultAttribListProperty::OnClickButton(CPoint point)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndList);
	ASSERT_VALID(m_pWndInPlace);
	ASSERT(::IsWindow(m_pWndInPlace->GetSafeHwnd()));

	m_bButtonIsDown = TRUE;
	Redraw();

	CDialogDefaultAttribList dlg;
	dlg.DoModal();

	m_bButtonIsDown = FALSE;
	Redraw();

	if (m_pWndInPlace != NULL)
	{
		m_pWndInPlace->SetFocus();
	}
	else
	{
		m_pWndList->SetFocus();
	}
}

//////////////////////////////////////////////////////////////////////////
//CUIProperties

UINT WM_UI_PROPERTY_CHANGED = ::RegisterWindowMessage(_T("WM_UI_PROPERTY_CHANGED"));

IMPLEMENT_DYNAMIC(CUIProperties, CWnd)

CUIProperties::CUIProperties(void)
{
}

CUIProperties::~CUIProperties(void)
{
}

BEGIN_MESSAGE_MAP(CUIProperties, CWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
	ON_COMMAND(ID_PROPERTIES_TOOLBAR_SORT, OnPropertiesSort)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES_TOOLBAR_SORT, OnUpdatePropertiesSort)
	ON_COMMAND(ID_PROPERTIES_TOOLBAR_ALPHABETIC, OnPropertiesAlphaBetic)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES_TOOLBAR_ALPHABETIC, OnUpdatePropertiesAlphaBetic)
END_MESSAGE_MAP()

BOOL CUIProperties::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CWnd::Create(afxGlobalData.RegisterWindowClass(_T("UIPropWnd")), _T(""), dwStyle, rect, pParentWnd, nID, NULL);
}

int CUIProperties::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建组合:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if(!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE_ERROR("未能创建属性网格\n");
		return -1;      // 未能创建
	}

	InitPropList();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* 已锁定*/);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* 锁定*/);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// 所有命令将通过此控件路由，而不是通过主框架路由:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();

	return 0;
}

void CUIProperties::AdjustLayout()
{
	if(GetSafeHwnd() == NULL)
		return;

	CRect rectClient;
	GetClientRect(rectClient);
	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + cyTlb, rectClient.Width(), rectClient.Height() -cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CUIProperties::SetVSDotNetLook(BOOL bSet)
{
	m_wndPropList.SetVSDotNetLook(bSet);
	m_wndPropList.SetGroupNameFullWidth(bSet);
}

void CUIProperties::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);

	// TODO: Add your message handler code here
	m_wndPropList.SetFocus();
}

void CUIProperties::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	AdjustLayout();
}

LRESULT CUIProperties::OnPropertyChanged(WPARAM wp, LPARAM lp)
{
	CMFCPropertyGridProperty* pProp = (CMFCPropertyGridProperty *)lp;

	int nLevel = pProp->GetHierarchyLevel();
	for(; nLevel>1; nLevel--)
		pProp = pProp->GetParent();

	GetOwner()->SendMessage(WM_UI_PROPERTY_CHANGED, wp, LPARAM(pProp));

	return TRUE;
}

void CUIProperties::OnPropertiesSort()
{
	m_wndPropList.SetAlphabeticMode(FALSE);
}

void CUIProperties::OnUpdatePropertiesSort(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(!m_wndPropList.IsAlphabeticMode());
}

void CUIProperties::OnPropertiesAlphaBetic()
{
	m_wndPropList.SetAlphabeticMode(TRUE);
}

void CUIProperties::OnUpdatePropertiesAlphaBetic(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void CUIProperties::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();

	HideAllProperties();
}

void CUIProperties::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
}

void CUIProperties::HideAllProperties(BOOL bAdjustLayout/*=FALSE*/)
{
	for(int i=0; i<m_wndPropList.GetPropertyCount(); i++)
		m_wndPropList.GetProperty(i)->Show(FALSE, bAdjustLayout);

	m_pControl=NULL;
}

void CUIProperties::ShowProperty(CControlUI* pControl)
{
	if(m_pControl == pControl)
		return;
	
	if(pControl == NULL)
	{
		HideAllProperties(TRUE);
		return;
	}

	HideAllProperties();

	m_wndPropList.SetCurSel(NULL, FALSE);

	ExtendedAttributes mDummy;
	ExtendedAttributes* pExtended = (ExtendedAttributes*)pControl->GetTag();
	if (pExtended==NULL)
	{
		pExtended = &mDummy;
	}
	m_wndPropList.RemoveAll();

	ShowControlProperty(pControl);
	m_pControl=pControl;
	m_wndPropList.AdjustLayout();
}

static CMFCPropertyGridProperty* MakeGridProperty(DuiLib::UIPropertyGridItem& item)
{
	CMFCPropertyGridProperty* pPropUI = NULL;
	if (item.PT == DuiLib::PT_Size)
	{
		DuiLib::UIPropertyGridItem& pWidth = item.childs[0];
		DuiLib::UIPropertyGridItem& pHeight = item.childs[1];

		pPropUI = new CMFCPropertyGridSizeProperty(item, UTF8_TO_CSTRING(item.title.c_str()), typeSize,
			UTF8_TO_CSTRING(pWidth.title.c_str()), 
			UTF8_TO_CSTRING(pHeight.title.c_str()), 
			UTF8_TO_CSTRING(pWidth.desc.c_str()),
			UTF8_TO_CSTRING(pHeight.desc.c_str()));
	}
	else if (item.PT == DuiLib::PT_Rect)
	{
		DuiLib::UIPropertyGridItem& pLeft = item.childs[0];
		DuiLib::UIPropertyGridItem& pTop = item.childs[1];
		DuiLib::UIPropertyGridItem& pRight = item.childs[2];
		DuiLib::UIPropertyGridItem& pBottom = item.childs[3];

		pPropUI = new CMFCPropertyGridRectProperty(item, UTF8_TO_CSTRING(item.title.c_str()), typeRect,
			UTF8_TO_CSTRING(pLeft.title.c_str()),
			UTF8_TO_CSTRING(pTop.title.c_str()),
			UTF8_TO_CSTRING(pRight.title.c_str()),
			UTF8_TO_CSTRING(pBottom.title.c_str()),
			UTF8_TO_CSTRING(pLeft.desc.c_str()),
			UTF8_TO_CSTRING(pTop.desc.c_str()),
			UTF8_TO_CSTRING(pRight.desc.c_str()),
			UTF8_TO_CSTRING(pBottom.desc.c_str()));
	}
	else if (item.PT == DuiLib::PT_Boolean)
	{
		bool b = false;
		if (item.defaultv.vt == VT_BOOL)
			b = (bool)(item.defaultv);
		pPropUI = new CMFCPropertyGridBoolProperty(item, UTF8_TO_CSTRING(item.title.c_str()), b, typeBool, UTF8_TO_CSTRING(item.desc.c_str()));
	}
	else if (item.PT == DuiLib::PT_Image)
	{
		pPropUI = new CMFCPropertyGridImageProperty(item, UTF8_TO_CSTRING(item.title.c_str()), _T(""), UTF8_TO_CSTRING(item.desc.c_str()), typeImage);
	}
	else if (item.PT == DuiLib::PT_Number)
	{
		pPropUI = new CMFCPropertyGridNumberProperty(item, UTF8_TO_CSTRING(item.title.c_str()), (LONG)0, UTF8_TO_CSTRING(item.desc.c_str()), typeNumber);
	}
	else if (item.PT == DuiLib::PT_Color)
	{
		pPropUI = new CMFCPropertyGridColor32Property(item, UTF8_TO_CSTRING(item.title.c_str()), (LONG)ARGB(0, 0, 0, 0), NULL, UTF8_TO_CSTRING(item.desc.c_str()), typeColor);
	}
	else if (item.PT == DuiLib::PT_Font || item.PT == DuiLib::PT_CustomFont)
	{
		pPropUI = new CMFCPropertyGridCustomFontsProperty(item, UTF8_TO_CSTRING(item.title.c_str()), (_variant_t)_T("字体管理"), UTF8_TO_CSTRING(item.desc.c_str()), typeFont);
	}
	else if (item.PT == DuiLib::PT_String)
	{
		pPropUI = new CMFCPropertyGridStringProperty(item, UTF8_TO_CSTRING(item.title.c_str()), _T(""), UTF8_TO_CSTRING(item.desc.c_str()), typeString);
	}
	else if (item.PT == DuiLib::PT_CustomAttribute)
	{
		pPropUI = new CMFCPropertyGridDefaultAttribListProperty(item, UTF8_TO_CSTRING(item.title.c_str()), (_variant_t)_T("默认属性"), UTF8_TO_CSTRING(item.desc.c_str()), typeDefaultAttri);
	}
	else if (item.PT == DuiLib::PT_Select)
	{
		std::vector<CString> valuelist;
		for (size_t ci = 0; ci < item.childs.size(); ++ci)
		{
			DuiLib::UIPropertyGridItem& item_child = item.childs[ci];
			valuelist.push_back(UTF8_TO_CSTRING(item_child.title.c_str()));
		}	
		pPropUI = new CMFCPropertyGridStringSelectProperty(item, UTF8_TO_CSTRING(item.title.c_str()), valuelist, StringConvertor::ToMFCString(item.defaultv.bstrVal), typeSelect, UTF8_TO_CSTRING(item.desc.c_str()));
	}

	return pPropUI;
}

void CUIProperties::ShowControlProperty(CControlUI* pControl)
{
	ASSERT(pControl);
	m_GridProperty.clear();
	pControl->GetPropertyList(m_GridProperty);

	for (size_t i=0; i< m_GridProperty.size(); ++i)
	{
		DuiLib::UIPropertyGrid& property = m_GridProperty[i];
		CMFCPropertyGridProperty* pPropUI = new CMFCPropertyGridProperty(UTF8_TO_CSTRING(property.title.c_str()), typeNULL);
		for (size_t j=0; j<property.items.size(); ++j)
		{
			DuiLib::UIPropertyGridItem& item = property.items[j];
			CMFCPropertyGridProperty* pProp = MakeGridProperty(item);
			ASSERT(pProp && item.PT);
			if (pProp) pPropUI->AddSubItem(pProp);
		}
		m_wndPropList.AddProperty(pPropUI);

		for (int i = 0; i < pPropUI->GetSubItemsCount(); ++i)
		{
			const CMFCPropertyGridProperty* pProp = pPropUI->GetSubItem(i);
			ASSERT(pProp);
			const DWORD_PTR dwData = pProp->GetData();
			MFCPropertyGridType dwType = (MFCPropertyGridType)(DWORD)(dwData);
			if (dwType == typeColor)
			{
				CMFCPropertyGridColor32Property* pColorItem = (CMFCPropertyGridColor32Property*)pProp;
				pColorItem->UpdateFromControl(pControl);
			}
			else if (dwType >= typeBase)
			{
				CMFCPropertyGridPropertyBase* pBaseItem = (CMFCPropertyGridPropertyBase*)pProp;
				pBaseItem->UpdateFromControl(pControl);
			}
			else
			{
				ASSERT(FALSE && dwData);
			}
		}

		pPropUI->Show(TRUE, FALSE);
	}
}

CMFCPropertyGridProperty* CUIProperties::FindPropByData(DWORD_PTR dwData, BOOL bSearchSubProps/* = TRUE*/) const
{
	return m_wndPropList.FindItemByData(dwData, bSearchSubProps);
}

CString CUIProperties::FormatOrigProperty(CMFCPropertyGridProperty* pProp, int nType)
{
	ASSERT(pProp);

	CString strOrigVal;

	if (nType == typeColor) {
		CMFCPropertyGridColor32Property* pColorItem = (CMFCPropertyGridColor32Property*)pProp;
		return pColorItem->GetValueFromUI(true);
	}
	else if (nType >= typeBase) {
		CMFCPropertyGridPropertyBase* pBaseItem = (CMFCPropertyGridPropertyBase*)pProp;
		return pBaseItem->GetValueFromUI(true);
	}
	else {
		ASSERT(FALSE && "Unsupported type");
	}

	return strOrigVal;
}
