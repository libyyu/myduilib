// DialogUIAttribEdit.cpp : 实现文件
//
#include "stdafx.h"
#include "DuiDesigner.h"
#include "DialogUIAttribEdit.h"


// CDialogUIAttribEdit 对话框

IMPLEMENT_DYNAMIC(CDialogUIAttribEdit, CDialog)

CDialogUIAttribEdit::CDialogUIAttribEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogUIAttribEdit::IDD, pParent), m_pControl(NULL)
{

}

CDialogUIAttribEdit::CDialogUIAttribEdit(CControlUI* pControl)
	: CDialog(CDialogUIAttribEdit::IDD, NULL), m_pControl(pControl)
{
}

CDialogUIAttribEdit::~CDialogUIAttribEdit()
{
}

void CDialogUIAttribEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_UI_PROP_LOCATION, m_wndUIPropLocation);
}


BEGIN_MESSAGE_MAP(CDialogUIAttribEdit, CDialog)
	ON_REGISTERED_MESSAGE(WM_UI_PROPERTY_CHANGED, OnUIPropChanged)
END_MESSAGE_MAP()


// CDialogUIAttribEdit 消息处理程序

BOOL CDialogUIAttribEdit::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect rectUIProp;
	m_wndUIPropLocation.GetClientRect(&rectUIProp);
	m_wndUIPropLocation.MapWindowPoints(this, &rectUIProp);
	m_wndUIProperties.Create(WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER, rectUIProp, this, ID_UI_PROP_WND);
	m_wndUIProperties.ShowProperty(m_pControl);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

LRESULT CDialogUIAttribEdit::OnUIPropChanged(WPARAM wp, LPARAM lp)
{
	CMFCPropertyGridProperty* pProp = (CMFCPropertyGridProperty *)lp;
	SetUIValue(pProp, pProp->GetData());

	return TRUE;
}

void CDialogUIAttribEdit::SetUIValue(CMFCPropertyGridProperty* pProp,int nTag)
{
	CControlUI* pControl = m_wndUIProperties.GetCurUI();
	if(pControl == NULL)
		return;

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
}
