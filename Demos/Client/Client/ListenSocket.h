


#pragma once


/////////////////////////////////////////////////////////////////////////////
// CListenSocket command target
class CMainWindow;

class CListenSocket : public CSocket
{
//		DECHARE_DYNAMIC(CChatSocket);
// Attributes
public:
	CMainWindow* m_pDlg;

// Operations
public:
	CListenSocket();
	CListenSocket(CMainWindow* pDlg);
	virtual ~CListenSocket();

	CDuiString m_strPeerIP;
	CDuiString m_strPeerPort;
// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListenSocket)
	public:
	virtual void OnReceive(int nErrorCode);

	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CListenSocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	BOOL MyGetSockName(CDuiString& rSocketAddress, UINT& rSocketPort);
	BOOL MyGetPeerName(CDuiString& rSocketAddress, UINT& rSocketPort);
// Implementation
protected:
};

