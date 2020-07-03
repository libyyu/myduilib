#pragma once
#include "Core/UIRender.h"

//////////////////////////////////////////////////////////////////////////
// CImagePreview

class CImagePreview : public CStatic
{
	DECLARE_DYNAMIC(CImagePreview)

public:
	CImagePreview();
	virtual ~CImagePreview();

public:
	void SetImageProperty(LPCTSTR pstrImage,CRect& rcDest,CRect& rcSource,CRect& rcCorner,COLORREF clrMask,int nFade,BOOL bHole);
	LPCTSTR GetImageProperty() const { return m_drawInfo.sDrawString; }
	void SetManager(CPaintManagerUI* pManager);

private:
	CString m_strImage;
	DuiLib::TDrawInfo m_drawInfo;
	CRect m_rcImage;
	CPaintManagerUI* m_pManager;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};


