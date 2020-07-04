#include "stdafx.h"
#include "PropertiesWnd.h"
#include "Resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CPropertiesWnd

CPropertiesWnd::CPropertiesWnd()
{
	g_pPropertiesWnd = this;
}

CPropertiesWnd::~CPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(WM_UI_PROPERTY_CHANGED, OnUIPropChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertiesWnd 消息处理程序

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	m_wndUIProperties.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, ID_UI_PROP_WND);

	AdjustLayout();

	return 0;
}
void CPropertiesWnd::OnDestroy()
{
	m_wndUIProperties.DestroyWindow();
}
void CPropertiesWnd::AdjustLayout()
{
	if(GetSafeHwnd() == NULL)
		return;

	CRect rectClient;
	GetClientRect(rectClient);
	m_wndUIProperties.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);

}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	AdjustLayout();
}

void CPropertiesWnd::SetVSDotNetLook(BOOL bSet)
{
	m_wndUIProperties.SetVSDotNetLook(bSet);
}

void CPropertiesWnd::ShowProperty(CControlUI* pControl)
{
	m_wndUIProperties.ShowProperty(pControl);
}

LRESULT CPropertiesWnd::OnUIPropChanged(WPARAM wp, LPARAM lp)
{
	CMFCPropertyGridProperty* pProp = (CMFCPropertyGridProperty *)lp;
	SetUIValue(pProp, pProp->GetData());

	return TRUE;
}

void CPropertiesWnd::SetPropValue(CControlUI* pControl,int nTag)
{
	CControlUI* pCurControl = m_wndUIProperties.GetCurUI();
	if(pCurControl != pControl)
		return;

	m_wndUIProperties.SetPropValue(nTag);
}

void CPropertiesWnd::SetUIValue(CMFCPropertyGridProperty* pProp,int nTag)
{
	CControlUI* pControl = m_wndUIProperties.GetCurUI();
	if(pControl == NULL)
		return;

	int nUpdate = UPDATE_REDRAW_CONTROL;
	CString strName = pProp->GetName();
	strName.MakeLower();
	CString strNewVal;
	CString strOldVal;
	strNewVal = pProp->FormatProperty();
	strOldVal = m_wndUIProperties.FormatOrigProperty(pProp, nTag);
	
	bool bChangeName = false;
	if (nTag >= typeBase) {
		CMFCPropertyGridPropertyBase* pBaseItem = (CMFCPropertyGridPropertyBase*)pProp;
		if (pBaseItem->item.title == "Name") {
			bChangeName = true;
			CPaintManagerUI* pManager = g_pMainFrame->GetActiveUIView()->GetPaintManager();
			if (strNewVal.IsEmpty() || pManager->FindControl(strNewVal))
			{
				if (!strNewVal.IsEmpty())
					MessageBox(strNewVal + _T(" 名称已被其他控件使用！"));
				pProp->SetValue(pProp->GetOriginalValue());
				return;
			}
			else
				pManager->ReapObjects(pControl);

			g_pClassView->RenameUITreeItem(pControl, strNewVal);
		}
	}

	CUIDesignerView* pUIView = g_pMainFrame->GetActiveUIView();
	ASSERT(pUIView);
	TNotifyUI Msg;
	UIAttribute oldAttrib = {strName, strOldVal};
	Msg.pSender = pControl;
	Msg.sType = _T("PropertyBeginChanged");
	Msg.wParam = 0;
	Msg.lParam = (LPARAM)&oldAttrib;
	pUIView->Notify(Msg);

	if (nTag == typeColor) {
		CMFCPropertyGridColor32Property* pColorItem = (CMFCPropertyGridColor32Property*)pProp;
		pColorItem->UpdateToControl(pControl);
	}
	else if (nTag >= typeBase) {
		CMFCPropertyGridPropertyBase* pBaseItem = (CMFCPropertyGridPropertyBase*)pProp;
		pBaseItem->UpdateToControl(pControl);
	}
	else {
		ASSERT(FALSE && "Unsupported type");
	}

	UIAttribute newAttrib = {strName, strNewVal};
	Msg.sType = _T("PropertyEndChanged");
	Msg.lParam = (LPARAM)&newAttrib;
	pUIView->Notify(Msg);

	if (pControl->IsClass(_T("Form")))
	{
		if (nTag >= typeBase) {
			CMFCPropertyGridPropertyBase* pBaseItem = (CMFCPropertyGridPropertyBase*)pProp;
			if (pBaseItem->item.title == "Size") {
				Msg.sType = _T("formsize");
				Msg.lParam = NULL;
				pUIView->Notify(Msg);
			}
		}
	}

	if (bChangeName)
	{
		CPaintManagerUI* pManager = pUIView->GetPaintManager();
		pManager->InitControls(pControl);
	}

	CControlUI* pParent = pControl->GetParent();
	if(pParent == NULL) pParent = pControl;

	pParent->SetPos(pParent->GetPos());
	pControl->NeedUpdate();
	pParent->NeedUpdate();
	/*switch(nUpdate)
	{
	case UPDATE_POS:
		pParent->SetPos(pParent->GetPos());
		break;
	case UPDATE_REDRAW_CONTROL:
		pControl->NeedUpdate();

		break;
	case UPDATE_REDRAW_PARENT:
		pParent->NeedUpdate();
	}*/
}