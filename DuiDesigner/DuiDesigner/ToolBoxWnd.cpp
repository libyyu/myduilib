// ToolBoxWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "DuiDesigner.h"
#include "ToolBoxWnd.h"

//////////////////////////////////////////////////////////////////////////
// CToolBoxWnd

IMPLEMENT_DYNAMIC(CToolBoxWnd, CDockablePane)

CToolBoxWnd::CToolBoxWnd()
{
	g_pToolBoxWnd=this;
}

CToolBoxWnd::~CToolBoxWnd()
{
}


BEGIN_MESSAGE_MAP(CToolBoxWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CToolBoxWnd 消息处理程序

int CToolBoxWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CRect rectDummy;
	rectDummy.SetRectEmpty();
	if (!m_ctlToolList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 12))
	{
		TRACE0("Failed to create ToolBox\n");
		return -1;      // fail to create
	}
	InitToolList();

	return 0;
}

void CToolBoxWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	AdjustLayout();
}

void CToolBoxWnd::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	m_ctlToolList.SetWindowPos(NULL, rectClient.left,rectClient.top,rectClient.Width(),rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

void CToolBoxWnd::InitToolList()
{

	{
		CToolElement* pControlTab = new CToolElement(_T("General"));
		pControlTab->AddSubTool(new CToolElement(_T("指针"), _T(""), IDI_TOOLBOX_POINTER));
		m_ctlToolList.AddToolTab(pControlTab);
	}

	{
		const std::map<DuiLib::CDuiString, std::vector<DuiLib::CDuiString> >& controls = DuiLib::CControlFactory::GetAllControls();
		for (std::map<DuiLib::CDuiString, std::vector<DuiLib::CDuiString> >::const_iterator it = controls.begin(); it != controls.end(); ++it)
		{
			DuiLib::CDuiString category = it->first;
			CString sCategory = category.GetData();
			CToolElement* pControlTab2 = new CToolElement(sCategory);
			for (std::vector<DuiLib::CDuiString>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
			{
				category = *it2;
				CString sControlInterfaceName = category.GetData();
				pControlTab2->AddSubTool(new CToolElement(sControlInterfaceName, sControlInterfaceName, glb_ControlICON(sControlInterfaceName)));
			}
			m_ctlToolList.AddToolTab(pControlTab2);
		}
	}

	m_ctlToolList.ExpandAll(FALSE);

	m_ctlToolList.SetCurSel(_T(""));
}