// ChatSocket.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "ListenSocket.h"
#include "mainwindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListenSocket

CListenSocket::CListenSocket()
{
}

CListenSocket::CListenSocket(CMainWindow* pDlg)
{
	m_pDlg=pDlg;
	
}
CListenSocket::~CListenSocket()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CListenSocket, CSocket)
	//{{AFX_MSG_MAP(CListenSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CListenSocket member functions

void CListenSocket::OnReceive(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CSocket::OnReceive(nErrorCode);
	m_pDlg->RecvFrom(this);
}

BOOL CListenSocket::MyGetSockName(CDuiString& rSocketAddress, UINT& rSocketPort)
{
	CString address;
	BOOL b = this->GetSockName(address, rSocketPort);
	rSocketAddress = address.GetBuffer();
	return b;
}
BOOL CListenSocket::MyGetPeerName(CDuiString& rSocketAddress, UINT& rSocketPort)
{
	CString address;
	BOOL b = this->GetPeerName(address, rSocketPort);
	rSocketAddress = address.GetBuffer();
	return b;
}

