#include "StdAfx.h"
#include "LayoutManager.h"
#include "UIUtil.h"
#include <vector>
#include <map>

using DuiLib::IListOwnerUI;
using DuiLib::UIPropertyGrid;
using DuiLib::UIPropertyGridItem;
using DuiLib::PropertyType;
//////////////////////////////////////////////////////////////////////////
//CWindowUI

CWindowUI::CWindowUI()
:m_nOpacity(0xFF),
m_bAlphaBackground(false),
m_dwDefaultDisabledFontColor(0xFFA7A6AA),
m_dwDefaultFontColor(0xFF000000),
m_dwDefaultLinkFontColor(0xFF0000FF),
m_dwDefaultLinkHoverFontColor(0xFFD3215F),
m_dwDefaultSelectedFontColor(0xFFBAE4FF)
{
}

CWindowUI::~CWindowUI()
{

}

LPCTSTR CWindowUI::GetClass() const
{
	return _T("Form");
}

LPVOID CWindowUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("Form")) == 0 )
		return static_cast<CWindowUI*>(this);

	return CContainerUI::GetInterface(pstrName);
}

SIZE CWindowUI::GetInitSize()
{
	if (m_pManager)
		return m_pManager->GetInitSize();
	return SIZE{ 0,0 };
}

void CWindowUI::SetInitSize(int cx, int cy)
{
	if(m_pManager) m_pManager->SetInitSize(cx,cy);

	SetPos(CRect(0,0,cx,cy));
}

RECT& CWindowUI::GetSizeBox()
{
	if (m_pManager) return m_pManager->GetSizeBox();
	return CRect(0, 0, 0, 0);
}

void CWindowUI::SetSizeBox(RECT& rcSizeBox)
{
	if (m_pManager) m_pManager->SetSizeBox(rcSizeBox);
}

RECT& CWindowUI::GetCaptionRect()
{
	if (m_pManager) return m_pManager->GetCaptionRect();
	return CRect(0, 0, 0, 0);
}

void CWindowUI::SetCaptionRect(RECT& rcCaption)
{
	if (m_pManager) m_pManager->SetCaptionRect(rcCaption);
}

RECT& CWindowUI::GetBottomCaptionRect()
{
	if (m_pManager) return m_pManager->GetBottomCaptionRect();
	return CRect(0, 0, 0, 0);
}
void CWindowUI::SetBottomCaptionRect(RECT& rcCaption)
{
	if (m_pManager) m_pManager->SetBottomCaptionRect(rcCaption);
}

SIZE CWindowUI::GetRoundCorner() const
{
	if (m_pManager) return m_pManager->GetRoundCorner();
	return CSize(0, 0);
}

void CWindowUI::SetRoundCorner(int cx, int cy)
{
	if (m_pManager) m_pManager->SetRoundCorner(cx,cy);
}

SIZE CWindowUI::GetMinInfo() const
{
	if (m_pManager) return m_pManager->GetMinInfo();
	return CSize(0, 0);
}

SIZE CWindowUI::GetMaxInfo() const
{
	if (m_pManager) return m_pManager->GetMaxInfo();
	return CSize(0, 0);
}

void CWindowUI::SetMinInfo(int cx, int cy)
{
	ASSERT(cx>=0 && cy>=0);
	if (m_pManager) m_pManager->SetMinInfo(cx,cy);
}

void CWindowUI::SetMaxInfo(int cx, int cy)
{
	ASSERT(cx>=0 && cy>=0);
	if (m_pManager) m_pManager->SetMaxInfo(cx,cy);
}

bool CWindowUI::IsShowUpdateRect() const
{
	if (m_pManager) return m_pManager->IsShowUpdateRect();
	return false;
}

void CWindowUI::SetShowUpdateRect(bool show)
{
	if (m_pManager) m_pManager->SetShowUpdateRect(show);
}

bool CWindowUI::IsNoActivate()
{
	if (m_pManager) return m_pManager->IsNoActivate();
	return true;
}

void CWindowUI::SetNoActivate(bool bNoActivate)
{
	if (m_pManager) m_pManager->SetNoActivate(bNoActivate);
}

void CWindowUI::SetPos(RECT rc)
{
	CControlUI::SetPos(rc);
	m_cxyFixed.cx = rc.right - rc.left;
	m_cxyFixed.cy = rc.bottom - rc.top;

	if(m_items.GetSize()==0)
		return;
	CControlUI* pControl=static_cast<CControlUI*>(m_items[0]);
	if(pControl==NULL)
		return;
	pControl->SetPos(rc);//放大到整个客户区
}

bool CWindowUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if( _tcscmp(pstrName, _T("size")) == 0 )
	{
		LPTSTR pstr = NULL;
		int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
		SetInitSize(cx, cy);
		return true;
	} 
	else if( _tcscmp(pstrName, _T("sizebox")) == 0 )
	{
		RECT rcSizeBox = { 0 };
		LPTSTR pstr = NULL;
		rcSizeBox.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		rcSizeBox.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
		rcSizeBox.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
		rcSizeBox.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
		SetSizeBox(rcSizeBox);
		return true;
	}
	else if( _tcscmp(pstrName, _T("caption")) == 0 )
	{
		RECT rcCaption = { 0 };
		LPTSTR pstr = NULL;
		rcCaption.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		rcCaption.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
		rcCaption.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
		rcCaption.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
		SetCaptionRect(rcCaption);
		return true;
	}
	else if (_tcscmp(pstrName, _T("bottom_caption")) == 0)
	{
		RECT rcCaption = { 0 };
		LPTSTR pstr = NULL;
		rcCaption.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
		rcCaption.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
		rcCaption.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
		rcCaption.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
		SetBottomCaptionRect(rcCaption);
		return true;
	}
	else if( _tcscmp(pstrName, _T("roundcorner")) == 0 )
	{
		LPTSTR pstr = NULL;
		int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
		SetRoundCorner(cx, cy);
		return true;
	} 
	else if( _tcscmp(pstrName, _T("mininfo")) == 0 )
	{
		LPTSTR pstr = NULL;
		int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
		SetMinInfo(cx, cy);
		return true;
	}
	else if( _tcscmp(pstrName, _T("maxinfo")) == 0 )
	{
		LPTSTR pstr = NULL;
		int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
		SetMaxInfo(cx, cy);
		return true;
	}
	else if( _tcscmp(pstrName, _T("showdirty")) == 0 )
	{
		SetShowUpdateRect(_tcscmp(pstrValue, _T("true")) == 0);
		return true;
	}
	else if( _tcscmp(pstrName, _T("pos")) == 0 )
	{
		RECT rcPos = { 0 };
		LPTSTR pstr = NULL;
		rcPos.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		rcPos.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
		rcPos.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
		rcPos.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
		SetInitSize(rcPos.right - rcPos.left, rcPos.bottom - rcPos.top);
		return true;
	}
	else if ( _tcscmp(pstrName, _T("opacity")) == 0 || _tcscmp(pstrName, _T("alpha")) == 0)
	{
		SetOpacity(_ttoi(pstrValue));
		return true;
	}
	else if (_tcscmp(pstrName, _T("layeredopacity")) == 0) 
	{
		SetLayeredOpacity(_ttoi(pstrValue));
		return true;
	}
	else if (_tcscmp(pstrName, _T("layeredimage")) == 0) 
	{
		SetLayered(true);
		SetLayeredImage(pstrValue);
		return true;
	}
	else if ( _tcscmp(pstrName, _T("defaultfontcolor")) == 0)
	{
		while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
		if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetDefaultFontColor(clrColor);
		return true;
	}
	else if ( _tcscmp(pstrName, _T("selectedcolor")) == 0)
	{
		while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
		if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetDefaultSelectedFontColor(clrColor);
		return true;
	}
	else if ( _tcscmp(pstrName, _T("disabledfontcolor")) == 0)
	{
		while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
		if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetDefaultDisabledFontColor(clrColor);
		return true;
	}
	else if ( _tcscmp(pstrName, _T("linkfontcolor")) == 0)
	{
		while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
		if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetDefaultLinkFontColor(clrColor);
		return true;
	}
	else if ( _tcscmp(pstrName, _T("linkhoverfontcolor")) == 0)
	{
		while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
		if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetDefaultLinkHoverFontColor(clrColor);
		return true;
	}
	else if (_tcsicmp(pstrName, _T("trayiconid")) == 0) 
	{
		return true;
	}
	else if (_tcscmp(pstrName, _T("traytiptext")) == 0) 
	{
		return true;
	}
	else 
	{
		if (m_pManager) return m_pManager->SetWindowAttribute(pstrName, pstrValue);
		return false;
	}
}

void CWindowUI::SetLayered( bool bTrans )
{
	if (m_pManager) {
		m_pManager->SetLayered(bTrans);
		m_bAlphaBackground = m_pManager->IsLayered();
	}
}

bool CWindowUI::IsLayered() const
{
	return m_bAlphaBackground;
}
BYTE CWindowUI::GetOpacity() const
{
	return m_nOpacity;
}
void CWindowUI::SetOpacity(BYTE nOpacity)
{
	m_nOpacity = nOpacity;
	if (m_pManager) {
		m_pManager->SetOpacity(nOpacity);
		m_bAlphaBackground = m_pManager->IsLayered();
	}
}
BYTE CWindowUI::GetLayeredOpacity() const
{
	return m_nOpacity;
}
void CWindowUI::SetLayeredOpacity(BYTE nOpacity)
{
	m_nOpacity = nOpacity;
	if (m_pManager) {
		m_pManager->SetLayeredOpacity(nOpacity);
		m_bAlphaBackground = m_pManager->IsLayered();
	}
}
LPCTSTR CWindowUI::GetLayeredImage() const
{
	if (m_pManager)return m_pManager->GetLayeredImage();
	return _T("");
}
void CWindowUI::SetLayeredImage(LPCTSTR pstrImage)
{
	if (m_pManager)m_pManager->SetLayeredImage(pstrImage);
}

void CWindowUI::SetDefaultDisabledFontColor( DWORD dwColor )
{
	m_dwDefaultDisabledFontColor=dwColor;
	if (m_pManager) {
		m_pManager->SetDefaultDisabledColor(dwColor);
	}
}

DWORD CWindowUI::GetDefaultDisabledFontColor() const
{
	return m_dwDefaultDisabledFontColor;
}

void CWindowUI::SetDefaultFontColor( DWORD dwColor )
{
	m_dwDefaultFontColor=dwColor;
	if (m_pManager) {
		m_pManager->SetDefaultFontColor(dwColor);
	}
}

DWORD CWindowUI::GetDefaultFontColor() const
{
	return m_dwDefaultFontColor;
}

void CWindowUI::SetDefaultLinkFontColor( DWORD dwColor )
{
	m_dwDefaultLinkFontColor=dwColor;
	if (m_pManager) {
		m_pManager->SetDefaultLinkFontColor(dwColor);
	}
}

DWORD CWindowUI::GetDefaultLinkFontColor() const
{
	return m_dwDefaultLinkFontColor;
}

void CWindowUI::SetDefaultLinkHoverFontColor( DWORD dwColor )
{
	m_dwDefaultLinkHoverFontColor=dwColor;
	if (m_pManager) {
		m_pManager->SetDefaultLinkHoverFontColor(dwColor);
	}
}

DWORD CWindowUI::GetDefaultLinkHoverFontColor() const
{
	return m_dwDefaultLinkHoverFontColor;
}

void CWindowUI::SetDefaultSelectedFontColor( DWORD dwColor )
{
	m_dwDefaultSelectedFontColor=dwColor;
	if (m_pManager) {
		m_pManager->SetDefaultSelectedBkColor(dwColor);
	}
}

DWORD CWindowUI::GetDefaultSelectedFontColor() const
{
	return m_dwDefaultSelectedFontColor;
}
LPCTSTR CWindowUI::GetTrayTipText() const
{
	return _T("");
}
void CWindowUI::SetTrayTipText(LPCTSTR pstrImage)
{
}

void CWindowUI::GetPropertyList(std::vector<DuiLib::UIPropertyGrid>& property_list)
{
	property_list.push_back(UIPropertyGrid("Window", "Window"));
	UIPropertyGrid& property = property_list.back();
	std::vector< UIPropertyGridItem >& items = property.items;
#define ARGBL(a,r,g,b) ((COLORREF)(((BYTE)(r) | ((WORD)((BYTE)(g)) << 8)) | (((DWORD)(BYTE)(b)) << 16)) | (((DWORD)(BYTE)(a)) << 24))
	items.push_back(UIPropertyGridItem(PropertyType::PT_Size, "Size", "窗体大小", _variant_t(0)));
	{
		UIPropertyGridItem& item = items.back();
		item.childs.push_back(UIPropertyGridItem(PropertyType::PT_Number, "Width", "窗体的宽度", _variant_t(0)));
		item.childs.push_back(UIPropertyGridItem(PropertyType::PT_Number, "Height", "窗体的高度", _variant_t(0)));
	}
	items.push_back(UIPropertyGridItem(PropertyType::PT_Rect, "Caption", "Caption"));
	{
		UIPropertyGridItem& item = items.back();
		item.childs.push_back(UIPropertyGridItem(PropertyType::PT_Number, "Left", "标题的Left位置", _variant_t(0)));
		item.childs.push_back(UIPropertyGridItem(PropertyType::PT_Number, "Top", "标题的Top位置", _variant_t(0)));
		item.childs.push_back(UIPropertyGridItem(PropertyType::PT_Number, "Right", "标题的Right位置", _variant_t(0)));
		item.childs.push_back(UIPropertyGridItem(PropertyType::PT_Number, "Bottom", "标题的Bottom位置", _variant_t(0)));
	}
	items.push_back(UIPropertyGridItem(PropertyType::PT_Rect, "BottomCaption", "BottomCaption"));
	{
		UIPropertyGridItem& item = items.back();
		item.childs.push_back(UIPropertyGridItem(PropertyType::PT_Number, "Left", "标题的Left位置", _variant_t(0)));
		item.childs.push_back(UIPropertyGridItem(PropertyType::PT_Number, "Top", "标题的Top位置", _variant_t(0)));
		item.childs.push_back(UIPropertyGridItem(PropertyType::PT_Number, "Right", "标题的Right位置", _variant_t(0)));
		item.childs.push_back(UIPropertyGridItem(PropertyType::PT_Number, "Bottom", "标题的Bottom位置", _variant_t(0)));
	}
	items.push_back(UIPropertyGridItem(PropertyType::PT_Rect, "SizeBox", "SizeBox"));
	{
		UIPropertyGridItem& item = items.back();
		item.childs.push_back(UIPropertyGridItem(PropertyType::PT_Number, "Left", "尺寸盒的Left位置", _variant_t(0)));
		item.childs.push_back(UIPropertyGridItem(PropertyType::PT_Number, "Top", "尺寸盒的Top位置", _variant_t(0)));
		item.childs.push_back(UIPropertyGridItem(PropertyType::PT_Number, "Right", "尺寸盒的Right位置", _variant_t(0)));
		item.childs.push_back(UIPropertyGridItem(PropertyType::PT_Number, "Bottom", "尺寸盒的Bottom位置", _variant_t(0)));
	}
	items.push_back(UIPropertyGridItem(PropertyType::PT_Size, "RoundCorner", "圆角大小"));
	{
		UIPropertyGridItem& item = items.back();
		item.childs.push_back(UIPropertyGridItem(PropertyType::PT_Number, "Width", "圆角的宽度", _variant_t(0)));
		item.childs.push_back(UIPropertyGridItem(PropertyType::PT_Number, "Height", "圆角的高度", _variant_t(0)));
	}
	items.push_back(UIPropertyGridItem(PropertyType::PT_Size, "MinInfo", "MinInfo", _variant_t(0)));
	{
		UIPropertyGridItem& item = items.back();
		item.childs.push_back(UIPropertyGridItem(PropertyType::PT_Number, "MinWidth", "窗口的最小跟踪宽度", _variant_t(0)));
		item.childs.push_back(UIPropertyGridItem(PropertyType::PT_Number, "MinHeight", "窗口的最小跟踪高度", _variant_t(0)));
	}
	items.push_back(UIPropertyGridItem(PropertyType::PT_Size, "MaxInfo", "MaxInfo", _variant_t(0)));
	{
		UIPropertyGridItem& item = items.back();
		item.childs.push_back(UIPropertyGridItem(PropertyType::PT_Number, "MaxWidth", "窗口的最大跟踪宽度", _variant_t(0)));
		item.childs.push_back(UIPropertyGridItem(PropertyType::PT_Number, "MaxHeight", "窗口的最大跟踪高度", _variant_t(0)));
	}

	items.push_back(UIPropertyGridItem(PropertyType::PT_Boolean, "ShowDirty", "指示是否显示更新区域", _variant_t(bool(false))));
	items.push_back(UIPropertyGridItem(PropertyType::PT_Boolean, "NoActive", "指示是否禁用鼠标", _variant_t(bool(false))));
	items.push_back(UIPropertyGridItem(PropertyType::PT_Number, "Opacity", "窗口的opacity值(0-255)", _variant_t(255)));
	items.push_back(UIPropertyGridItem(PropertyType::PT_Number, "LayeredOpacity", "窗口的LayeredOpacity值(0-255)", _variant_t(255)));
	items.push_back(UIPropertyGridItem(PropertyType::PT_Boolean, "Layered", "窗口是否使用静态透明背景", _variant_t(bool(false))));
	items.push_back(UIPropertyGridItem(PropertyType::PT_Image, "LayeredImage", "窗口是否使用静态透明背景"));
	items.push_back(UIPropertyGridItem(PropertyType::PT_Color, "DisabledFontColor", "指定默认的Disabled字体颜色", _variant_t((LONG)(ARGBL(0, 0, 0, 0)))));
	items.push_back(UIPropertyGridItem(PropertyType::PT_Color, "DefaultFontColor", "指定默认的字体颜色", _variant_t((LONG)(ARGBL(0, 0, 0, 0)))));
	items.push_back(UIPropertyGridItem(PropertyType::PT_Color, "SelectedColor", "指定默认的selected字体颜色", _variant_t((LONG)(ARGBL(0, 0, 0, 0)))));
	items.push_back(UIPropertyGridItem(PropertyType::PT_Color, "LinkFontColor", "指定默认的link字体颜色", _variant_t((LONG)(ARGBL(0, 0, 0, 0)))));
	items.push_back(UIPropertyGridItem(PropertyType::PT_Color, "LinkHoverFontColor", "指定默认的linkhoverfont字体颜色", _variant_t((LONG)(ARGBL(0, 0, 0, 0)))));
	items.push_back(UIPropertyGridItem(PropertyType::PT_Font, "CustomFonts", "管理自定义的字体"));
	items.push_back(UIPropertyGridItem(PropertyType::PT_CustomAttribute, "DefaultAttribList", "管理默认的属性列表"));


#undef ARGBL
}


//////////////////////////////////////////////////////////////////////////
//CWindowUI

CFormTestWnd::CFormTestWnd():m_pManager(NULL),m_pRoot(NULL)
{

}

CFormTestWnd::~CFormTestWnd()
{
	if(m_pManager)
		delete m_pManager;
}

LPCTSTR CFormTestWnd::GetWindowClassName() const
{
	return _T("UIFormTest");
}

UINT CFormTestWnd::GetClassStyle() const
{
	return UI_CLASSSTYLE_CHILD;
}

void CFormTestWnd::OnFinalMessage(HWND /*hWnd*/)
{
	delete this;
}

void CFormTestWnd::SetManager(CPaintManagerUI* pPaintManager)
{
	ASSERT(pPaintManager);

	m_pManager = pPaintManager;
}

CPaintManagerUI* CFormTestWnd::GetManager() const
{
	return m_pManager;
}

void CFormTestWnd::SetRoot(CControlUI* pControl)
{
	m_pRoot = pControl;
}

void CFormTestWnd::Notify(TNotifyUI& msg)
{
}

LRESULT CFormTestWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	styleValue &= ~WS_MAXIMIZEBOX; 
	styleValue &= ~WS_MINIMIZEBOX; 
	styleValue &= ~WS_THICKFRAME; 
	styleValue &= ~WS_BORDER; 
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	RECT rcClient;
	::GetClientRect(*this, &rcClient);
	::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
		rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

	return 0;
}

LRESULT CFormTestWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CFormTestWnd::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CFormTestWnd::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CFormTestWnd::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	if(m_pManager->IsPointAtCaption(pt, rcClient))
		return HTCAPTION;

	return HTCLIENT;
}

LRESULT CFormTestWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SIZE szRoundCorner = m_pManager->GetRoundCorner();
	if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
		RECT rcClient;
		::GetClientRect(*this, &rcClient);
		RECT rc = { rcClient.left, rcClient.top + szRoundCorner.cx, rcClient.right, rcClient.bottom };
		HRGN hRgn1 = ::CreateRectRgnIndirect( &rc );
		HRGN hRgn2 = ::CreateRoundRectRgn(rcClient.left, rcClient.top, rcClient.right + 1, rcClient.bottom - szRoundCorner.cx, szRoundCorner.cx, szRoundCorner.cy);
		::CombineRgn( hRgn1, hRgn1, hRgn2, RGN_OR );
		::SetWindowRgn(*this, hRgn1, TRUE);
		::DeleteObject(hRgn1);
		::DeleteObject(hRgn2);
	}

	bHandled = FALSE;
	return 0;
}

LRESULT CFormTestWnd::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CRect rcWork = oMonitor.rcWork;
	rcWork.OffsetRect(-rcWork.left, -rcWork.top);

	LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
	lpMMI->ptMaxPosition.x	= rcWork.left;
	lpMMI->ptMaxPosition.y	= rcWork.top;
	lpMMI->ptMaxSize.x		= rcWork.right;
	lpMMI->ptMaxSize.y		= rcWork.bottom;

	bHandled = FALSE;
	return 0;
}

LRESULT CFormTestWnd::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(wParam==VK_ESCAPE)
		Close();

	bHandled=FALSE;
	return 0;
}

LRESULT CFormTestWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch( uMsg ) {
		case WM_CREATE:        lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
		case WM_DESTROY:       lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
		case WM_NCPAINT:       lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
		case WM_NCCALCSIZE:    lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
		case WM_NCHITTEST:     lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
		case WM_SIZE:          lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
		case WM_GETMINMAXINFO: lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
		case WM_KEYDOWN:	   lRes = OnKeyDown(uMsg, wParam, lParam, bHandled); break;
		default:
			bHandled = FALSE;
	}
	if( bHandled )
		return lRes;
	if( m_pManager->MessageHandler(uMsg, wParam, lParam, lRes) )
		return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

void CFormTestWnd::Init()
{
	m_pManager->Init(m_hWnd);
	m_pManager->AttachDialog(m_pRoot);
	m_pManager->AddNotifier(this);
	//m_pManager->SetBackgroundTransparent(true);

	SIZE szInitWindowSize = m_pManager->GetInitSize();
	if( szInitWindowSize.cx != 0 ) {
		::SetWindowPos(m_hWnd, NULL, 0, 0, szInitWindowSize.cx, szInitWindowSize.cy, SWP_NOMOVE | SWP_NOZORDER);
	}
}

//////////////////////////////////////////////////////////////////////////
//CLayoutManager

CLayoutManager::CLayoutManager(void)
: m_pFormUI(NULL), m_bShowGrid(false), m_bShowAuxBorder(true)
{
}

CLayoutManager::~CLayoutManager(void)
{
}

class CUserDefineUI : public CControlUI
{
public:
	virtual void SetClass(LPCTSTR className)
	{
		mClassName = className;
		mClassName+=_T("UI");
	}
	virtual LPCTSTR GetClass() const
	{
		return mClassName;
	}

	virtual LPVOID GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("UserDefinedControl")) == 0 )
			return static_cast<CUserDefineUI*>(this);
		return NULL;
	}
protected:
	virtual bool SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		TRACE(_T("%s:%s\n"),pstrName,pstrValue);
		m_pAttributeList[CDuiString(pstrName)]=CDuiString(pstrValue);

		return true;
	}
public:
	std::map<CDuiString,CDuiString> m_pAttributeList;
protected:
	CDuiString mClassName;
};

CControlUI* CLayoutManager::CreateControl(LPCTSTR pstrClass)
{
	CUserDefineUI* pResult = new CUserDefineUI;
	if (pResult)
	{
		pResult->SetClass(pstrClass);
	}
	return pResult;
}

void CLayoutManager::Init(HWND hWnd,LPCTSTR pstrLoad)
{
	m_pFormUI = static_cast<CWindowUI*>(NewUI(_T("Form"), CRect(0,0,FORM_INIT_WIDTH,FORM_INIT_HEIGHT),NULL, NULL)->GetInterface(_T("Form")));

	ASSERT(m_pFormUI);
	m_pFormUI->SetManager(&m_Manager, NULL);
	m_pFormUI->SetInitSize(FORM_INIT_WIDTH,FORM_INIT_HEIGHT);

	m_Manager.Init(hWnd);
	if(*pstrLoad!='\0')
	{
		m_strSkinDir = pstrLoad;
		int nPos = m_strSkinDir.ReverseFind(_T('\\'));
		if(nPos != -1)
			m_strSkinDir = m_strSkinDir.Left(nPos + 1);
		CString sFile = pstrLoad;
		if (nPos != -1)
			sFile = sFile.Right(sFile.GetLength() - nPos-1);
		//TODO:
		
		CPaintManagerUI::SetResourcePath(m_strSkinDir);
		CPaintManagerUI::SetResourceType(DuiLib::UILIB_FILE);
		

		g_HookAPI.SetSkinDir(m_strSkinDir);
		g_HookAPI.EnableCreateFile(true);

		CDialogBuilder builder;
		CControlUI* pRoot = builder.Create(sFile.GetString(), (UINT)0, this, &m_Manager);
		if(pRoot)
			m_pFormUI->Add(pRoot);

		SIZE size = m_Manager.GetInitSize();
		m_pFormUI->SetInitSize(size.cx,size.cy);
		m_pFormUI->SetOpacity(m_Manager.GetOpacity());
		m_pFormUI->SetLayered(m_Manager.IsLayered());
		m_pFormUI->SetLayeredImage(m_Manager.GetLayeredImage());
		m_pFormUI->SetLayeredOpacity(m_Manager.GetLayeredOpacity());

		const DuiLib::CDuiStringPtrMap& attrs = m_Manager.GetXMLAttributeMap();
		for (int i = 0; i < attrs.GetSize(); i++) {
			LPCTSTR key = attrs.GetAt(i);
			CDuiString* pAttrValue = static_cast<CDuiString*>(attrs.Find(key));

			m_pFormUI->SetXMLAttribute(key, pAttrValue->GetData());
		}
	}
	m_Manager.AttachDialog(m_pFormUI);
}

void CLayoutManager::Draw(CDC* pDC)
{
	CSize szForm = GetForm()->GetInitSize();
	CRect rcPaint(0, 0, szForm.cx, szForm.cy);
	CControlUI* pForm = m_Manager.GetRoot();
	//TODO:
	pForm->DoPaint(pDC->GetSafeHdc(), rcPaint, NULL);

	CContainerUI* pContainer = static_cast<CContainerUI*>(pForm->GetInterface(DUI_CTR_CONTAINER));
	ASSERT(pContainer);
	DrawAuxBorder(pDC, pContainer->GetItemAt(0));
	DrawGrid(pDC, rcPaint);
}

void CLayoutManager::DrawAuxBorder(CDC* pDC,CControlUI* pControl)
{
	if(!m_bShowAuxBorder || pControl==NULL || !pControl->IsVisible())
		return;

	CContainerUI* pContainer = static_cast<CContainerUI*>(pControl->GetInterface(DUI_CTR_CONTAINER));

	//draw auxBorder TODO:
	if(pControl->GetBorderColor()==0 )//||pControl->GetBorderSize()<=0)
	{
		pDC->SetBkMode(TRANSPARENT);
		CPen DotedPen(PS_SOLID,1,pContainer?RGB(255,0,0):RGB(0,255,0));
		CPen* pOldPen;
		CBrush* pOldBrush;
		pOldPen = pDC->SelectObject(&DotedPen);
		pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
		CRect rect = pControl->GetPos();
		pDC->Rectangle(&rect);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
	}

	if(pContainer == NULL)
		return;
	for(int i=0;i<pContainer->GetCount();i++)
	{
		DrawAuxBorder(pDC,pContainer->GetItemAt(i));
	}
}

void CLayoutManager::DrawGrid(CDC* pDC, CRect& rect)
{
	if(!m_bShowGrid)
		return;

	for(int i=rect.left; i<rect.right; i+=10)
	{
		for(int j=rect.top; j<rect.bottom; j+=10)
			pDC->SetPixel(i, j, RGB(0,0,0));
	}
}

CControlUI* CLayoutManager::NewUI(LPCTSTR szClassName, CRect& rect,CControlUI* pParent, CLayoutManager* pLayout)
{
	CControlUI* pControl = NULL;
	
	ExtendedAttributes* pExtended = new ExtendedAttributes;

	if (0 == _tcscmp(szClassName, _T("Form")))
	{
		pControl = new CWindowUI;
		pExtended->nDepth = 0;
		pExtended->strClass = _T("Form");
		pControl->SetXMLAttribute(_T("name"), pControl->GetClass());
	}
	else
	{
		TCHAR pstrClassName[200] = { 0 };
		wsprintf(pstrClassName, _T("C%sUI"), szClassName);
		pControl = DuiLib::CreateControl<CControlUI*>(pstrClassName);
		if (pControl) {
			pControl->SetXMLAttribute(_T("float"), _T("true"));
		}
		else {
			typedef CControlUI* (*MyLPCREATECONTROL)(LPCTSTR pstrType);
			MyLPCREATECONTROL lpCreateControl = NULL;
			CDuiStringPtrMap* pPlugins = CPaintManagerUI::GetPlugins();
			if (pControl == NULL) {
				for (int i = 0; i < pPlugins->GetSize(); i++) {
					if (LPCTSTR key = pPlugins->GetAt(i)) {
						lpCreateControl = static_cast<MyLPCREATECONTROL>(pPlugins->Find(key));
						if (lpCreateControl != NULL) {
							pControl = lpCreateControl(szClassName);
							if (pControl) {
								pControl->SetXMLAttribute(_T("float"), _T("true"));
								break;
							}
						}
					}
				}
			}
		}
	}

	if (!pControl) {
		delete pExtended;
		return NULL;
	}
	pExtended->strClass = szClassName;
	pControl->SetTag((UINT_PTR)pExtended);

	//pos
	CString strPos;
	CRect rcPos;
	CRect rcParent = pParent ? pParent->GetPos() : CRect(0,0,0,0);
	rcPos.left = rect.left - rcParent.left;
	rcPos.top = rect.top - rcParent.top;
	rcPos.right = rect.right - rcParent.left;
	rcPos.bottom = rect.bottom - rcParent.top;
	strPos.Format(_T("%d,%d,%d,%d"), rcPos.left, rcPos.top, rcPos.right, rcPos.bottom);

	pControl->SetXMLAttribute(_T("pos"), strPos);

	//close the delayed destory function
	CContainerUI* pContainer = static_cast<CContainerUI*>(pControl->GetInterface(DUI_CTR_CONTAINER));
	if(pContainer)
		pContainer->SetDelayedDestroy(false);

	if(pParent)
	{
		CContainerUI* pParentContainer = static_cast<CContainerUI*>(pParent->GetInterface(DUI_CTR_CONTAINER));
		ASSERT(pParentContainer);
		if(!pParentContainer->Add(pControl))
		{
			delete pExtended;
			pControl->Delete();
			return NULL;
		}
		ExtendedAttributes* pParentExtended = (ExtendedAttributes*)pParentContainer->GetTag();
		pExtended->nDepth = pParentExtended ? pParentExtended->nDepth + 1 : pExtended->nDepth;
		rcPos = pParent->GetPos();
		strPos.Format(_T("%d,%d,%d,%d"), rcPos.left, rcPos.top, rcPos.right, rcPos.bottom);
		pParent->SetXMLAttribute(_T("pos"), strPos);

		if (!pControl->IsFloat())
		{
			CString sTmp;
			sTmp.Format(_T("%d"), pParent->GetFixedWidth());
			pControl->SetXMLAttribute(_T("width"), sTmp);
			sTmp.Format(_T("%d"), pParent->GetFixedHeight());
			pControl->SetXMLAttribute(_T("height"), sTmp);
		}
	}

	if(pLayout)
	{
		CPaintManagerUI* pManager = pLayout->GetManager();
		pControl->SetManager(pManager,pParent);

		//default attributes
		CString strClass = pControl->GetClass();
		LPCTSTR pDefaultAttributes = pManager->GetDefaultAttributeList(strClass);
		if (pDefaultAttributes)
		{
			pControl->SetAttributeList(pDefaultAttributes);
		}

		//name
		pLayout->SetDefaultUIName(pControl);
	}

	return pControl;
}

BOOL CLayoutManager::DeleteUI(CControlUI* pControl)
{
	if(pControl == NULL)
		return FALSE;

	ReleaseExtendedAttrib(pControl, pControl->GetManager());
	CControlUI* pParent = pControl->GetParent();
	if (pParent)
	{
		CContainerUI* pContainer = static_cast<CContainerUI*>(pParent->GetInterface(DUI_CTR_CONTAINER));
		pContainer->Remove(pControl);
	}
	else
		pControl->Delete();

	return TRUE;
}

void CLayoutManager::ReleaseExtendedAttrib(CControlUI* pControl, CPaintManagerUI* pManager)
{
	if(pControl == NULL)
		return;

	ExtendedAttributes* pExtended = (ExtendedAttributes*)pControl->GetTag();
	delete pExtended;
	pControl->SetTag(NULL);
	if(pManager)
		pManager->ReapObjects(pControl);

	CContainerUI* pContainer = static_cast<CContainerUI*>(pControl->GetInterface(DUI_CTR_CONTAINER));
	if(pContainer != NULL)
	{
		int nCount = pContainer->GetCount();
		for(int i=0; i<nCount; i++)
			ReleaseExtendedAttrib(pContainer->GetItemAt(i), pManager);
	}
}

CPaintManagerUI* CLayoutManager::GetManager()
{
	return &m_Manager;
}

CWindowUI* CLayoutManager::GetForm() const
{ 
	return m_pFormUI;
}

CControlUI* CLayoutManager::FindControl(CPoint point) const
{
	return m_Manager.FindControl(point);
}

CLayoutManager::CDelayRepos::CDelayRepos()
{
}

CLayoutManager::CDelayRepos::~CDelayRepos()
{
	m_arrDelay.RemoveAll();
}

BOOL CLayoutManager::CDelayRepos::AddParent(CControlUI* pControl)
{
	if(pControl == NULL)
		return FALSE;

	for(int i=0;i<m_arrDelay.GetSize();i++)
	{
		CControlUI* pParent=m_arrDelay.GetAt(i);

		if(pControl == pParent)
			return FALSE;
	}
	m_arrDelay.Add(pControl);

	return TRUE;
}

void CLayoutManager::CDelayRepos::Repos()
{
	for(int i=0;i<m_arrDelay.GetSize();i++)
	{
		CControlUI* pParent=m_arrDelay.GetAt(i);

		pParent->SetPos(pParent->GetPos());
	}
}

void CLayoutManager::TestForm(LPCTSTR pstrFile)
{
	CFormTestWnd* pFrame=new CFormTestWnd();
	CPaintManagerUI* pManager=new CPaintManagerUI();
	SIZE size=m_Manager.GetInitSize();
	pManager->SetInitSize(size.cx,size.cy);
	pManager->SetSizeBox(m_Manager.GetSizeBox());
	RECT rect=m_Manager.GetCaptionRect();
	if (rect.bottom==0)
	{
		rect.bottom=30;
	}
	pManager->SetCaptionRect(rect);
	size=m_Manager.GetRoundCorner();
	pManager->SetRoundCorner(size.cx,size.cy);
	size=m_Manager.GetMinInfo();
	pManager->SetMinInfo(size.cx,size.cy);
	size=m_Manager.GetMaxInfo();
	pManager->SetMaxInfo(size.cx,size.cy);
	pManager->SetShowUpdateRect(m_Manager.IsShowUpdateRect());

	if( pFrame == NULL )
		return;

	g_HookAPI.EnableInvalidate(false);

	pFrame->SetManager(pManager);
	HWND h_wnd =pFrame->Create(m_Manager.GetPaintWindow(),_T("FormTest"),UI_WNDSTYLE_FRAME,0,0,0,size.cx,size.cy);

	// CControlUI* pRoot=CloneControls(GetForm()->GetItemAt(0));
	// 使用新建的XML树来预览，不会挂掉
	pManager->Init(h_wnd);
	CDialogBuilder builder;
	CContainerUI* pRoot=static_cast<CContainerUI*>(builder.Create(pstrFile,(UINT)0,NULL,pManager));
	if(pRoot==NULL)
		return;

	//pRoot->SetManager(NULL,NULL);
	pFrame->SetRoot(pRoot);
	pFrame->Init();


	pFrame->CenterWindow();

	HWND m_hWnd=pFrame->GetHWND();
	MSG msg = { 0 };
	while( ::IsWindow(m_hWnd) && ::GetMessage(&msg, NULL, 0, 0) ) {
		if( msg.hwnd != m_hWnd ) {
			if( (msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST) )
				continue;
			if( msg.message >= WM_KEYFIRST && msg.message <= WM_KEYLAST ) 
				continue;
			if( msg.message == WM_SETCURSOR )
				continue;
		}
		if( !CPaintManagerUI::TranslateMessage(&msg) ) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		if( msg.message == WM_QUIT ) break;
	}
	if( msg.message == WM_QUIT ) ::PostQuitMessage(msg.wParam);

	g_HookAPI.EnableInvalidate(true);

	DeleteFile(pstrFile);
}

BOOL CLayoutManager::IsEmptyForm() const
{
	return GetForm()->GetItemAt(0)==NULL;
}

CControlUI* CLayoutManager::CloneControls(CControlUI* pControl)
{
	if(pControl == NULL)
		return NULL;

	CContainerUI* pContainer = static_cast<CContainerUI*>(pControl->GetInterface(DUI_CTR_CONTAINER));
	if(pContainer==NULL)
		return CloneControl(pControl);

	CContainerUI* pCopyContainer = static_cast<CContainerUI*>(CloneControl(pContainer)->GetInterface(DUI_CTR_CONTAINER));
	pCopyContainer->SetAutoDestroy(false);
	pCopyContainer->RemoveAll();
	pCopyContainer->SetAutoDestroy(true);
	for(int i=0;i<pContainer->GetCount();i++)
	{
		CControlUI* pCopyControl = CloneControls(pContainer->GetItemAt(i));
		pCopyControl->SetManager(NULL,pCopyContainer);
		pCopyContainer->Add(pCopyControl);
	}

	return pCopyContainer;
}

CControlUI* CLayoutManager::CloneControl(CControlUI* pControl)
{
	CControlUI* pCopyControl = NULL;
	CString strClass = ((ExtendedAttributes*)pControl->GetTag())->strClass;

	if (pControl->IsClass(DUI_CTR_LIST))
	{
		CListUI& copyList = *static_cast<CListUI*>(pControl->GetInterface(DUI_CTR_LIST));
		if (copyList.GetHorizontalScrollBar() || copyList.GetVerticalScrollBar())
		{//测试窗体中，暂不支持滚动条
			copyList.EnableScrollBar(false, false);
		}
		pCopyControl = new CListUI();
		*(((CListUI*)pCopyControl)->GetHeader()) = *(copyList.GetHeader());
		*((CVerticalLayoutUI*)((CListUI*)pCopyControl)->GetList()) = *static_cast<CVerticalLayoutUI*>(copyList.GetList());
	}
	else {
		pCopyControl = NewUI(pControl->GetClass(), CRect(0, 0, 0, 0), pControl->GetParent(), NULL);
		if (pCopyControl) {
			const DuiLib::CDuiStringPtrMap& attrs = pControl->GetXMLAttributeMap();
			for (int i = 0; i < attrs.GetSize(); i++) {
				LPCTSTR key = attrs.GetAt(i);
				CDuiString* pAttrValue = static_cast<CDuiString*>(attrs.Find(key));
				pCopyControl->SetXMLAttribute(key, pAttrValue->GetData());
			}
		}
	}

	if (!pCopyControl) {
		pCopyControl = new CUserDefineUI(*static_cast<CUserDefineUI*>(pControl));
	}

	return pCopyControl;
}

void CLayoutManager::AlignLeft(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected)
{
	SIZE szXYLeft = pFocused->GetFixedXY();
	CString strTmp;
	for (int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl = arrSelected.GetAt(i);
		if(pControl->GetParent() != pFocused->GetParent())
			continue;

		int fw = pControl->GetFixedWidth();
		int fh = pControl->GetFixedWidth();
		SIZE szXY = pControl->GetFixedXY();
		szXY.cx = szXYLeft.cx;
		strTmp.Format(_T("%d,%d,%d,%d"), szXY.cx, szXY.cy, szXY.cx+fw, szXY.cy+fh);
		pControl->SetXMLAttribute(_T("pos"), strTmp);
	}

	CControlUI* pParent = pFocused->GetParent();
	if (pParent)
	{
		pParent->SetPos(pParent->GetPos());
		RECT pos = pParent->GetPos();
		strTmp.Format(_T("%d,%d,%d,%d"), pos.left, pos.top, pos.right, pos.bottom);
		pParent->SetXMLAttribute(_T("pos"), strTmp);
	}
}

void CLayoutManager::AlignRight(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected)
{
	SIZE szXYRight=pFocused->GetFixedXY();
	int nWidth=pFocused->GetFixedWidth();

	for (int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);
		if(pControl->GetParent()!=pFocused->GetParent())
			continue;

		SIZE szXY=pControl->GetFixedXY();
		szXY.cx=szXYRight.cx+nWidth-pControl->GetFixedWidth();
		pControl->SetFixedXY(szXY);
	}

	CControlUI* pParent=pFocused->GetParent();
	if(pParent)
		pParent->SetPos(pParent->GetPos());
}

void CLayoutManager::AlignTop(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected)
{
	SIZE szXYTop=pFocused->GetFixedXY();

	for (int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);
		if(pControl->GetParent()!=pFocused->GetParent())
			continue;

		SIZE szXY=pControl->GetFixedXY();
		szXY.cy=szXYTop.cy;
		pControl->SetFixedXY(szXY);
	}

	CControlUI* pParent=pFocused->GetParent();
	if(pParent)
		pParent->SetPos(pParent->GetPos());
}

void CLayoutManager::AlignBottom(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected)
{
	SIZE szXYBottom=pFocused->GetFixedXY();
	int nHeight=pFocused->GetFixedHeight();

	for (int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);
		if(pControl->GetParent()!=pFocused->GetParent())
			continue;

		SIZE szXY=pControl->GetFixedXY();
		szXY.cy=szXYBottom.cy+nHeight-pControl->GetFixedHeight();
		pControl->SetFixedXY(szXY);
	}

	CControlUI* pParent=pFocused->GetParent();
	if(pParent)
		pParent->SetPos(pParent->GetPos());
}

void CLayoutManager::AlignCenterVertically(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected)
{
	CControlUI* pParent=pFocused->GetParent();
	if(!pParent)
		return;

	RECT rcParent=pParent->GetPos();

	CRect rectUnion;
	rectUnion.SetRectEmpty();
	for(int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);
		if(pControl->GetParent()!=pParent)
		{
			arrSelected.RemoveAt(i);
			continue;
		}

		rectUnion.UnionRect(&rectUnion,&pControl->GetPos());
	}

	int nOffsetY=(rcParent.top+rcParent.bottom)/2-(rectUnion.top+rectUnion.bottom)/2;
	for(int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);

		SIZE szXY=pControl->GetFixedXY();
		szXY.cy+=nOffsetY;
		pControl->SetFixedXY(szXY);
	}

	pParent->SetPos(pParent->GetPos());
}

void CLayoutManager::AlignCenterHorizontally(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected)
{
	CControlUI* pParent=pFocused->GetParent();
	if(!pParent)
		return;

	RECT rcParent=pParent->GetPos();

	CRect rectUnion;
	rectUnion.SetRectEmpty();
	for(int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);
		if(pControl->GetParent()!=pParent)
		{
			arrSelected.RemoveAt(i);
			continue;
		}

		rectUnion.UnionRect(&rectUnion,&pControl->GetPos());
	}

	int nOffsetX=(rcParent.left+rcParent.right)/2-(rectUnion.left+rectUnion.right)/2;
	for(int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);

		SIZE szXY=pControl->GetFixedXY();
		szXY.cx+=nOffsetX;
		pControl->SetFixedXY(szXY);
	}

	pParent->SetPos(pParent->GetPos());
}

void CLayoutManager::AlignHorizontal(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected)
{
	CControlUI* pParent=pFocused->GetParent();
	if(!pParent)
		return;

	CRect rectUnion;
	rectUnion.SetRectEmpty();
	int nTotalWidth=0;
	for(int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);
		if(pControl->GetParent()!=pParent)
		{
			arrSelected.RemoveAt(i);
			continue;
		}
		nTotalWidth+=pControl->GetWidth();
		rectUnion.UnionRect(&rectUnion,&pControl->GetPos());
	}

	int nCount=arrSelected.GetSize();
	if(nCount<3)
		return;
	int nSpaceX=(rectUnion.Width()-nTotalWidth)/(nCount-1);
	int nMin;
	for(int i=0;i<nCount-1;i++)
	{
		nMin=i;
		int j;
		for(j=i+1;j<nCount;j++)
		{
			if(arrSelected[j]->GetX()<arrSelected[nMin]->GetX())
				nMin=j;
		}
		if(i!=nMin)
		{
			CControlUI* pControl=arrSelected[i];
			arrSelected[i]=arrSelected[nMin];
			arrSelected[nMin]=pControl;
		}
	}

	for(int i=1;i<nCount-1;i++)
	{
		int right=arrSelected[i-1]->GetFixedXY().cx+arrSelected[i-1]->GetWidth();
		SIZE szXY=arrSelected[i]->GetFixedXY();
		szXY.cx=right+nSpaceX;
		arrSelected[i]->SetFixedXY(szXY);
	}

	pParent->SetPos(pParent->GetPos());
}

void CLayoutManager::AlignVertical(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected)
{
	CControlUI* pParent=pFocused->GetParent();
	if(!pParent)
		return;

	CRect rectUnion;
	rectUnion.SetRectEmpty();
	int nTotalHeight=0;
	for(int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);
		if(pControl->GetParent()!=pParent)
		{
			arrSelected.RemoveAt(i);
			continue;
		}
		nTotalHeight+=pControl->GetHeight();
		rectUnion.UnionRect(&rectUnion,&pControl->GetPos());
	}

	int nCount=arrSelected.GetSize();
	if(nCount<3)
		return;
	int nSpaceY=(rectUnion.Height()-nTotalHeight)/(nCount-1);
	int nMin;
	for(int i=0;i<nCount-1;i++)
	{
		nMin=i;
		int j;
		for(j=i+1;j<nCount;j++)
		{
			if(arrSelected[j]->GetY()<arrSelected[nMin]->GetY())
				nMin=j;
		}
		if(i!=nMin)
		{
			CControlUI* pControl=arrSelected[i];
			arrSelected[i]=arrSelected[nMin];
			arrSelected[nMin]=pControl;
		}
	}

	for(int i=1;i<nCount-1;i++)
	{
		int bottom=arrSelected[i-1]->GetFixedXY().cy+arrSelected[i-1]->GetHeight();
		SIZE szXY=arrSelected[i]->GetFixedXY();
		szXY.cy=bottom+nSpaceY;
		arrSelected[i]->SetFixedXY(szXY);
	}

	pParent->SetPos(pParent->GetPos());
}

void CLayoutManager::AlignSameWidth(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected)
{
	int nWidth=pFocused->GetFixedWidth();

	CDelayRepos DelayPos;
	for (int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);
		if(pControl==pFocused)
			continue;

		pControl->SetFixedWidth(nWidth);
		DelayPos.AddParent(pControl->GetParent());
	}
	DelayPos.Repos();
}

void CLayoutManager::AlignSameHeight(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected)
{
	int nHeight=pFocused->GetFixedHeight();

	CDelayRepos DelayPos;
	for (int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);
		if(pControl==pFocused)
			continue;

		pControl->SetFixedHeight(nHeight);
		DelayPos.AddParent(pControl->GetParent());
	}
	DelayPos.Repos();
}

void CLayoutManager::AlignSameSize(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected)
{
	int nWidth=pFocused->GetFixedWidth();
	int nHeight=pFocused->GetFixedHeight();

	CDelayRepos DelayPos;
	for (int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);
		if(pControl==pFocused)
			continue;

		pControl->SetFixedWidth(nWidth);
		pControl->SetFixedHeight(nHeight);
		DelayPos.AddParent(pControl->GetParent());
	}
	DelayPos.Repos();
}

void CLayoutManager::ShowGrid(bool bShow)
{
	m_bShowGrid=bShow;
}

bool CLayoutManager::IsShowGrid() const
{
	return m_bShowGrid;
}

void CLayoutManager::ShowAuxBorder(bool bShow)
{
	m_bShowAuxBorder=bShow;
}

bool CLayoutManager::IsShowAuxBorder() const
{
	return m_bShowAuxBorder;
}

void CLayoutManager::MicoMoveUp(CArray<CControlUI*,CControlUI*>& arrSelected,int nMoved)
{
	CDelayRepos DelayPos;
	for(int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);

		SIZE szXY=pControl->GetFixedXY();
		szXY.cy-=nMoved;
		pControl->SetFixedXY(szXY);
		DelayPos.AddParent(pControl->GetParent());
	}
	DelayPos.Repos();
}

void CLayoutManager::MicoMoveDown(CArray<CControlUI*,CControlUI*>& arrSelected,int nMoved)
{
	CDelayRepos DelayPos;
	for(int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);

		SIZE szXY=pControl->GetFixedXY();
		szXY.cy+=nMoved;
		pControl->SetFixedXY(szXY);
		DelayPos.AddParent(pControl->GetParent());
	}
	DelayPos.Repos();
}

void CLayoutManager::MicoMoveLeft(CArray<CControlUI*,CControlUI*>& arrSelected,int nMoved)
{
	CDelayRepos DelayPos;
	for(int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);

		SIZE szXY=pControl->GetFixedXY();
		szXY.cx-=nMoved;
		pControl->SetFixedXY(szXY);
		DelayPos.AddParent(pControl->GetParent());
	}
	DelayPos.Repos();
}

void CLayoutManager::MicoMoveRight(CArray<CControlUI*,CControlUI*>& arrSelected,int nMoved)
{
	CDelayRepos DelayPos;
	for(int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);

		SIZE szXY=pControl->GetFixedXY();
		szXY.cx+=nMoved;
		pControl->SetFixedXY(szXY);
		DelayPos.AddParent(pControl->GetParent());
	}
	DelayPos.Repos();
}

void CLayoutManager::SaveControlProperty(CControlUI* pControl, TiXmlElement* pNode)
{
	TCHAR szBuf[MAX_PATH] = {0};

	const DuiLib::CDuiStringPtrMap& attrs = pControl->GetXMLAttributeMap();
	for (int i = 0; i < attrs.GetSize(); i++) {
		CDuiString key = attrs.GetAt(i);
		CDuiString* pAttrValue = static_cast<CDuiString*>(attrs.Find(key));
		key.MakeLower();
		if (key.CompareNoCase("visible") == 0) {
			if (!pControl->IsVisible() 
				&& !((static_cast<IContainerUI*>(pControl->GetInterface(DUI_CTR_ICONTAINER)) != NULL) 
					&& (static_cast<CContainerUI*>(pControl->GetInterface(DUI_CTR_CONTAINER)) != NULL)))
			{
				CControlUI* pParent = pControl->GetParent();
				if ((pParent != NULL) 
					&& ((static_cast<IContainerUI*>(pParent->GetInterface(DUI_CTR_ICONTAINER)) != NULL) 
						&& (static_cast<CContainerUI*>(pParent->GetInterface(DUI_CTR_CONTAINER)) != NULL)))
				{
					// 如果同一层中所有元素都是不可见的，则不设置属性
					bool bVisible = false;
					CContainerUI* pContainerUI = static_cast<CContainerUI*>(pParent->GetInterface(DUI_CTR_CONTAINER));
					for (int it = 0; it < pContainerUI->GetCount(); it++)
					{
						CControlUI* pControl = static_cast<CControlUI*>(pContainerUI->GetItemAt(it));
						bVisible = pControl->IsVisible();
						if (bVisible)
							break;
					}
					if (bVisible)
						pNode->SetAttribute("visible", "false");
				}
			}
		}
		else
		{
			pNode->SetAttribute(TO_ATTRIBUTE(key), TO_ATTRIBUTE(pAttrValue->GetData()));
		}
	}
}

void CLayoutManager::SaveProperties(CControlUI* pControl, TiXmlElement* pParentNode
									, BOOL bSaveChildren/* = TRUE*/)
{
	if((pControl == NULL) || (pParentNode == NULL))
		return;

	CString strClass = pControl->GetClass();
	TiXmlElement* pNode = new TiXmlElement(TO_ATTRIBUTE(strClass.GetBuffer()));
	CUserDefineUI* pUserDefinedControl = static_cast<CUserDefineUI*>(pControl->GetInterface(_T("UserDefinedControl")));
	if (pUserDefinedControl != NULL)
	{
		std::map<CDuiString,CDuiString>::iterator iter = pUserDefinedControl->m_pAttributeList.begin();
		for (;iter!=pUserDefinedControl->m_pAttributeList.end();iter++)
		{
			pNode->SetAttribute(TO_ATTRIBUTE(iter->first),TO_ATTRIBUTE(iter->second));
		}
	}

	ExtendedAttributes* pExtended = (ExtendedAttributes*)pControl->GetTag();

	ExtendedAttributes mDummy;
	if (pExtended == NULL)
	{
		pExtended = &mDummy;
	}

	SaveControlProperty(pControl, pNode);

	TiXmlNode* pNodeElement = pParentNode->InsertEndChild(*pNode);
	delete pNode;

	if(bSaveChildren == FALSE)
		return;

	CContainerUI* pContainer = static_cast<CContainerUI*>(pControl->GetInterface(DUI_CTR_CONTAINER));
	if(pContainer == NULL)
		return;
	for( int it = 0; it < pContainer->GetCount(); it++ )
	{
		CControlUI* pControl = static_cast<CControlUI*>(pContainer->GetItemAt(it));
		SaveProperties(pControl, pNodeElement->ToElement());
	}

	CListUI* pList = static_cast<CListUI*>(pControl->GetInterface(DUI_CTR_LIST));
	if (pList != NULL && pList->GetHeader()) {
		SaveProperties(pList->GetHeader(), pNodeElement->ToElement());
	}
}

bool CLayoutManager::SaveSkinFile( LPCTSTR pstrPathName )
{
	CString strPathName(pstrPathName);
	int nPos = strPathName.ReverseFind(_T('\\'));
	if(nPos == -1)
		return false;
	m_strSkinDir = strPathName.Left(nPos + 1);

	TCHAR szBuf[MAX_PATH] = {0};
	TiXmlDocument xmlDoc(CSTRING_TO_ANI(pstrPathName));
	TiXmlDeclaration Declaration("1.0","utf-8","yes");
	xmlDoc.InsertEndChild(Declaration);

	TiXmlElement* pFormElm = new TiXmlElement("Window");

	CWindowUI* pForm = GetForm();
	ASSERT(pForm);
	const DuiLib::CDuiStringPtrMap& attrs = pForm->GetXMLAttributeMap();
	for (int i = 0; i < attrs.GetSize(); i++) {
		LPCTSTR key = attrs.GetAt(i);
		CDuiString* pAttrValue = static_cast<CDuiString*>(attrs.Find(key));
		pFormElm->SetAttribute(TO_ATTRIBUTE(key), TO_ATTRIBUTE(pAttrValue->GetData()));
	}

	TiXmlNode* pNode = xmlDoc.InsertEndChild(*pFormElm);
	if(m_Manager.GetCustomFontCount() > 0)
	{
		TFontInfo* default_info = m_Manager.GetDefaultFontInfo();
		HFONT hDefaultFont = default_info->hFont;
		LOGFONT lfDefault = {0};
		GetObject(hDefaultFont, sizeof(LOGFONT), &lfDefault);

		std::vector<LOGFONT> cachedFonts;

		for (DWORD index = 0; index < m_Manager.GetCustomFontCount(); ++index)
		{
			HFONT hFont = m_Manager.GetFont(index);
			LOGFONT lf = {0};
			GetObject(hFont, sizeof(LOGFONT), &lf);

			bool bSaved = false;
			for (std::vector<LOGFONT>::const_iterator citer = cachedFonts.begin(); citer != cachedFonts.end(); ++citer)
			{
				if((citer->lfWeight == lf.lfWeight) && (citer->lfItalic == lf.lfItalic) && (citer->lfHeight == lf.lfHeight)
					&& (_tcsicmp(lf.lfFaceName, citer->lfFaceName) == 0))
				{
					bSaved = true;
					break;
				}
			}
			if(bSaved)
				continue;

			cachedFonts.push_back(lf);

			TiXmlElement* pFontElem = new TiXmlElement("Font");
			pFontElem->SetAttribute("name", TO_ATTRIBUTE(lf.lfFaceName));

			_stprintf_s(szBuf, _T("%d"), -lf.lfHeight);
			pFontElem->SetAttribute("size", TO_ATTRIBUTE(szBuf));

			pFontElem->SetAttribute("bold", (lf.lfWeight >= FW_BOLD)?"true":"false");
			pFontElem->SetAttribute("italic", lf.lfItalic?"true":"false");

			if((lfDefault.lfWeight == lf.lfWeight) && (lfDefault.lfItalic == lf.lfItalic) && (lfDefault.lfHeight == lf.lfHeight)
				&& (_tcsicmp(lf.lfFaceName, lfDefault.lfFaceName) == 0))
				pFontElem->SetAttribute("default", "true");

			pNode->ToElement()->InsertEndChild(*pFontElem);

			delete pFontElem;
			pFontElem = NULL;
		}
	}

	DWORD nCount = m_Manager.GetDefaultAttributeListCount(false);
	DWORD nCount2 = m_Manager.GetDefaultAttributeListCount(true);
	for (int i = 0; i < nCount; ++i)
	{
		LPCTSTR lpstrKey = m_Manager.GetDefaultAttributeListName(i, false);
		LPCTSTR lpstrAttribute = m_Manager.GetDefaultAttributeList(lpstrKey);

		TiXmlElement* pAttributeElem = new TiXmlElement("Default");
		pAttributeElem->SetAttribute("name", TO_ATTRIBUTE(lpstrKey));

		CString strAttrib(lpstrAttribute);
		pAttributeElem->SetAttribute("value", TO_ATTRIBUTE(strAttrib));

		pNode->ToElement()->InsertEndChild(*pAttributeElem);

		delete pAttributeElem;
		pAttributeElem = NULL;
	}

	SaveProperties(pForm->GetItemAt(0), pNode->ToElement());

	delete pFormElm;
	return xmlDoc.SaveFile();
}

void CLayoutManager::SetDefaultUIName(CControlUI* pControl)
{
	m_Manager.ReapObjects(pControl);

	BOOL bNeedName = FALSE;
	CString strName = pControl->GetName();
	if(strName.IsEmpty())
		bNeedName = TRUE;
	else
	{
		if(m_Manager.FindControl(strName))
			bNeedName = TRUE;
		else
			m_Manager.InitControls(pControl);
	}
	if(!bNeedName)
		return;

	int nCount = 0;
	CString strUIName;
	do 
	{
		nCount++;
		strUIName.Format(_T("%s%d"), pControl->GetClass(), nCount);
	}while(m_Manager.FindControl(strUIName));
	pControl->SetXMLAttribute(_T("name"),strUIName);
	m_Manager.InitControls(pControl);
}

CString CLayoutManager::ConvertImageFileName(LPCTSTR pstrImageAttrib)
{
	CString strImageAttrib(pstrImageAttrib);
	strImageAttrib.Replace(m_strSkinDir,_T(""));
// 	CStdString sItem;
// 	CStdString sValue;
// 	LPTSTR pstr = (LPTSTR)pstrImageAttrib;
// 	while( *pstr != _T('\0') )
// 	{
// 		sItem.Empty();
// 		sValue.Empty();
// 		while( *pstr != _T('\0') && *pstr != _T('=') )
// 		{
// 			LPTSTR pstrTemp = ::CharNext(pstr);
// 			while( pstr < pstrTemp)
// 			{
// 				sItem += *pstr++;
// 			}
// 		}
// 		if( *pstr++ != _T('=') ) break;
// 		if( *pstr++ != _T('\'') ) break;
// 		while( *pstr != _T('\0') && *pstr != _T('\'') )
// 		{
// 			LPTSTR pstrTemp = ::CharNext(pstr);
// 			while( pstr < pstrTemp)
// 			{
// 				sValue += *pstr++;
// 			}
// 		}
// 		if( *pstr++ != _T('\'') ) break;
// 		if( !sValue.IsEmpty() ) {
// 			if( sItem == _T("file"))
// 				break;
// 		}
// 		if( *pstr++ != _T(' ') ) break;
// 	}
// 
// 	if(sValue.IsEmpty())
// 		sValue = sItem;
// 	CString strFileName = sValue;
// 	int nPos = strFileName.ReverseFind(_T('\\'));
// 	if(nPos != -1)
// 	{
// 		strFileName = strFileName.Right(strFileName.GetLength() - nPos - 1);
// 		strImageAttrib.Replace(sValue, strFileName);
// 	}

	return strImageAttrib;
}

CString CLayoutManager::m_strSkinDir=_T("");
