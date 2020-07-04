// OutuptWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "DuiDesigner.h"
#include "OutuptWnd.h"

//////////////////////////////////////////////////////////////////////////
// COutuptWnd

IMPLEMENT_DYNAMIC(COutuptWnd, CDockablePane)

COutuptWnd::COutuptWnd()
{
	g_pOutputWnd = this;
}

COutuptWnd::~COutuptWnd()
{
}


BEGIN_MESSAGE_MAP(COutuptWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// COutuptWnd 消息处理程序

int COutuptWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CRect rectDummy;
	rectDummy.SetRectEmpty();
	if (!m_ctlOutputEdit.Create(WS_VISIBLE | WS_CHILD | WS_HSCROLL | ES_AUTOHSCROLL | ES_MULTILINE, rectDummy, this, 13))
	{
		TRACE0("Failed to create ToolBox\n");
		return -1;      // fail to create
	}
	m_ctlOutputEdit.SetReadOnly(TRUE);
	m_ctlOutputEdit.ModifyStyle(0, ES_MULTILINE);//添加多行属性

	return 0;
}

void COutuptWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	AdjustLayout();
}

void COutuptWnd::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	m_ctlOutputEdit.SetWindowPos(NULL, rectClient.left,rectClient.top,rectClient.Width(),rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

void COutuptWnd::AppendLog(const CString& log, COLORREF color)
{
	CString str = log;
	str.Replace(_T("\n"), _T("\r\n"));

	CHARFORMAT cf;
	m_ctlOutputEdit.GetSelectionCharFormat(cf);
	// Initialize character format structure
	cf.cbSize = sizeof(CHARFORMAT);
	//static int iHeight = 1000;
	cf.dwEffects &= ~CFE_AUTOCOLOR;
	cf.dwMask |= CFM_COLOR;
	cf.crTextColor = color;
	cf.dwMask |= CFM_FACE;
	_tcscpy_s(cf.szFaceName, _T("微软雅黑"));//设置字体
	// Set insertion point to end of text
	long nInsertionPoint = m_ctlOutputEdit.GetWindowTextLength();
	m_ctlOutputEdit.SetSel(nInsertionPoint, -1);

	// Set the character format
	m_ctlOutputEdit.SetSelectionCharFormat(cf);

	// Replace selection. Because we have nothing
	// selected, this will simply insert
	// the string at the current caret position.
	m_ctlOutputEdit.ReplaceSel(str);
}