// ImagePreview.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DuiDesigner.h"
#include "ImagePreview.h"


// CImagePreview

IMPLEMENT_DYNAMIC(CImagePreview, CStatic)

CImagePreview::CImagePreview()
{
	m_strImage.Empty();
	m_drawInfo.Clear();
	m_pManager=NULL;
	m_rcImage.SetRectEmpty();
}

CImagePreview::~CImagePreview()
{
}


BEGIN_MESSAGE_MAP(CImagePreview, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

// CImagePreview ��Ϣ�������

void CImagePreview::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CStatic::OnPaint()
	CMemDC memDC(dc, this);
	CDC* pDC = &memDC.GetDC();

	CRect rcClient;
	this->GetClientRect(&rcClient);
	CBrush* pOldBrush=pDC->SelectObject(&afxGlobalData.brBlack);
	pDC->Rectangle(&rcClient);
	pOldBrush=pDC->SelectObject((CBrush*)pDC->SelectStockObject(NULL_BRUSH));
	CRect rcBorder=m_rcImage;
	rcBorder.DeflateRect(-1,-1);
	pDC->Rectangle(rcBorder);
	pDC->SelectObject(pOldBrush);

	//TODO:
	CRenderEngine::DrawImage(pDC->GetSafeHdc(), m_pManager, m_rcImage, rcClient,
		m_drawInfo);
}

void CImagePreview::SetImageProperty(LPCTSTR pstrImage,CRect& rcDest,CRect& rcSource,CRect& rcCorner,COLORREF clrMask,int nFade,BOOL bHole)
{
	if(!m_strImage.IsEmpty())
		m_pManager->RemoveImage(m_strImage);

	m_strImage=pstrImage;
	m_drawInfo.Clear();
	m_rcImage.SetRectEmpty();
	if(!m_strImage.IsEmpty())
	{
		CDuiString strProperty;
		BOOL bFlag=FALSE;
		CDuiString str;
		if(!rcDest.IsRectNull())
		{
			str.Format(_T(" dest='%d,%d,%d,%d'"),rcDest.left,rcDest.top,rcDest.right,rcDest.bottom);
			strProperty +=str;
			bFlag=TRUE;
		}
		if(!rcSource.IsRectNull())
		{
			str.Format(_T(" source='%d,%d,%d,%d'"),rcSource.left,rcSource.top,rcSource.right,rcSource.bottom);
			strProperty +=str;
			bFlag=TRUE;
		}
		if(!rcCorner.IsRectNull())
		{
			str.Format(_T(" corner='%d,%d,%d,%d'"),rcCorner.left,rcCorner.top,rcCorner.right,rcCorner.bottom);
			strProperty +=str;
			bFlag=TRUE;
		}
		if(clrMask!=0xFF000000)
		{
			str.Format(_T(" mask='0x%08x'"),clrMask);
			strProperty +=str;
			bFlag=TRUE;
		}
		if(nFade!=255)
		{
			str.Format(_T(" fade='%d'"),nFade);
			strProperty +=str;
			bFlag=TRUE;
		}
		if(bHole==TRUE)
		{
			str=_T(" hole='true'");
			strProperty +=str;
			bFlag=TRUE;
		}
		if(bFlag)
		{
			str.Format(_T("file='%s'"),pstrImage);
			strProperty =str+ strProperty;
		}
		else
			strProperty =pstrImage;

		m_drawInfo.sDrawString = strProperty;
	}
	else
		m_drawInfo.Clear();

	const TImageInfo* pImage=m_pManager->GetImageEx(pstrImage,NULL,clrMask);
	if(pImage)
	{
		CRect rcClient;
		this->GetClientRect(&rcClient);
		int nX=(rcClient.Width()-pImage->nX)/2;
		int nY=(rcClient.Height()-pImage->nY)/2;

		m_rcImage.SetRect(nX,nY,nX+pImage->nX,nY+pImage->nY);
	}
	else
		m_drawInfo.Clear();

 	this->Invalidate(FALSE);
}

void CImagePreview::SetManager(CPaintManagerUI* pManager)
{
	m_pManager=pManager;
}