#pragma once
#include "afxwin.h"


// CDialogRootDir 对话框

class CDialogRootDir : public CDialog
{
	DECLARE_DYNAMIC(CDialogRootDir)

public:
	CDialogRootDir(const CString& initDir, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogRootDir();

// 对话框数据
	enum { IDD = IDD_DIALOG_ROOT_DIR };

	CString m_strRootDir;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
