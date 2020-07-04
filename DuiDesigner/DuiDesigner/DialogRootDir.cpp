// CDialogRootDir.cpp: 实现文件
//

#include "stdafx.h"
#include "DialogRootDir.h"
#include "afxdialogex.h"


// CDialogRootDir 对话框

IMPLEMENT_DYNAMIC(CDialogRootDir, CDialog)

CDialogRootDir::CDialogRootDir(const CString& initDir, CWnd* pParent /*=nullptr*/)
	: CDialog(CDialogRootDir::IDD, pParent)
	, m_strRootDir(initDir)
{
	if (m_strRootDir.Right(1) != _T("\\"))
		m_strRootDir += _T("\\");
}

CDialogRootDir::~CDialogRootDir()
{
}

void CDialogRootDir::DoDataExchange(CDataExchange* pDX)
{
	DDX_Text(pDX, IDC_EDIT_ROOT_PATH, m_strRootDir);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogRootDir, CDialog)
	ON_BN_CLICKED(IDOK, &CDialogRootDir::OnBnClickedOk)
END_MESSAGE_MAP()


// CDialogRootDir 消息处理程序
BOOL CDialogRootDir::OnInitDialog()
{
	CDialog::OnInitDialog();
	UpdateData(FALSE);
	return TRUE;
}

void CDialogRootDir::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	if (m_strRootDir.IsEmpty())
	{
		MessageBox(_T("项目根路径不能设置为空。"), _T("提示"));
		return;
	}
	if (!PathIsDirectory(m_strRootDir))
	{
		MessageBox(_T("所设路径已不存在，可能被移除。"), _T("提示"));
		return;
	}
	if (m_strRootDir.Right(1) != _T("\\"))
		m_strRootDir += _T("\\");

	UpdateData(FALSE);
	OnOK();
}
