#pragma once
#include "ToolBoxCtrl.h"

// COutuptWnd

class COutuptWnd : public CDockablePane
{
	DECLARE_DYNAMIC(COutuptWnd)

public:
	COutuptWnd();
	virtual ~COutuptWnd();

protected:
	CRichEditCtrl m_ctlOutputEdit;
public:
	//CToolElement* GetCurSel() const { return m_ctlToolList.GetCurSel(); }
	//void SetCurSel(CString strClass,BOOL bRedraw=TRUE) { m_ctlToolList.SetCurSel(strClass,bRedraw); }

	void AppendLog(const CString& log, COLORREF color = RGB(0, 0, 0));
protected:
	void AdjustLayout();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


